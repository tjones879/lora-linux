#if defined(__linux__)
#include <cerrno>
#include <fcntl.h>
#include <linux/random.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "inc/contact.hpp"
#include "inc/contact.hpp"
#include "inc/db.hpp"
#include "inc/init.hpp"
#include "inc/sodium_header.hpp"
#include "inc/utils.hpp"
#include <fstream>
#include <iostream>
#include <string>


namespace init
{
const std::string files[] = {"private.key", "public.key", "config.key"};
const std::string dirname("/.radiocom/");

static int checkLinux()
{
    int fd, c, ret = 0;
    fd = open("/dev/random", O_RDONLY | O_CLOEXEC);

    if (fd >= 0) {
        if (ioctl(fd, RNDGETENTCNT, &c) == 0 && c < 160) {
            std::cout << "This system does not provide enough entropy to generate secure random numbers.";
            std::cout << std::endl;
            ret = -1;
        } else {
            ret = 0;
        }
        close(fd);
    } else {
        perror("Open random");
        ret = fd;
    }

    return ret;
}

static int fileExists(std::string &file)
{
    struct stat info{};
    return static_cast<int>(!(stat(file.c_str(), &info) != 0 || !S_ISREG(info.st_mode)));
}

static int createFolder(const std::string &folder)
{
    int ret = mkdir(folder.c_str(), 0766);
    if (ret < 0)
        perror("Folder creation");

    return ret;
}

static int createFile(std::string &file)
{
    int fd = open(file.c_str(), O_RDWR | O_CREAT | O_CLOEXEC, 0600);

    if (fd < 0)
        perror("File creation");
    else
        close(fd);

    return fd;
}

static int checkFiles(const std::string &dir)
{
    int ret = 0;

    for (const auto& f : files) {
        std::string file(dir + f);
        if (fileExists(file) == 0) {
            ret = createFile(file);
            if (ret < 0)
                return ret;
        }
    }

    return ret;
}

static int checkFolder(const std::string &folder)
{
    struct stat info{};
    int ret = 0;

    if (stat(folder.c_str(), &info) != 0 || !S_ISDIR(info.st_mode))
        ret = createFolder(folder);

    return ret;
}

static int createKeypair(const std::string &pub_path, const std::string &priv_path)
{
    unsigned char pub_key[crypto_box_PUBLICKEYBYTES];
    unsigned char priv_key[crypto_box_SECRETKEYBYTES];
    std::ofstream pub(pub_path, std::ofstream::binary);
    std::ofstream priv(priv_path, std::ofstream::binary);

    sodium::crypto_box_keypair(pub_key, priv_key);
    pub.write(reinterpret_cast<char *>(pub_key), sizeof(pub_key));
    utils::print_hex(pub_key, sizeof(pub_key), std::cout);
    priv.write(reinterpret_cast<char *>(priv_key), sizeof(priv_key));
    utils::print_hex(priv_key, sizeof(priv_key), std::cout);

    sodium::sodium_memzero(priv_key, sizeof(priv_key));
    return 0;
}

static void getPrivateKey(const std::string &path, unsigned char *key)
{
    std::ifstream file(path, std::ifstream::binary);
    file.read(reinterpret_cast<char *>(key), crypto_box_SECRETKEYBYTES);
}

static void getPrivateKey(unsigned char *priv_key)
{
    std::string home(getenv("HOME"));
    getPrivateKey(home + dirname + "private.key", priv_key);
}

static void getPublicKey(unsigned char *pub_key)
{
    std::string home(getenv("HOME"));
    std::string path = home + dirname + "public.key";
    std::ifstream file(path, std::ifstream::binary);
    file.read(reinterpret_cast<char *>(pub_key), crypto_box_PUBLICKEYBYTES);
}

int decryptMessage(std::vector<unsigned char> &message, unsigned char *cipherText,
                   int cipherLen, unsigned char *nonce, const Contact contact) {
    int err = 0;
    std::vector<unsigned char> pubKey = contact.pubKey;
    unsigned char priv_key[crypto_box_SECRETKEYBYTES];
    getPrivateKey(priv_key);
    err = sodium::crypto_box_open_easy(&message[0], cipherText, cipherLen,
                                       nonce, &pubKey[0], priv_key);
    return err;
}

int encryptMessage(std::vector<unsigned char> &cipher,
                   std::vector<unsigned char> &message,
                   const Contact contact)
{
    int err = 0;
    unsigned char nonce[crypto_box_NONCEBYTES];
    sodium::randombytes_buf(nonce, sizeof(nonce));
    std::vector<unsigned char> pub_key;
    unsigned char priv_key[crypto_box_SECRETKEYBYTES];
    pub_key = contact.pubKey;
    getPrivateKey(priv_key);

    err = sodium::crypto_box_easy(&cipher[0], &message[0], message.size(),
                                  nonce, &pub_key[0], priv_key);
    return err;
}

static bool isFileEmpty(const std::string &path)
{
    std::ifstream file(path);
    std::streampos current = file.tellg();
    file.seekg(0, file.end);
    bool empty = file.tellg() == 0;
    file.seekg(current, file.beg);
    return empty;
}

std::shared_ptr<Database> initialize(unsigned char *priv_key)
{
    std::shared_ptr<Database> db = nullptr;
    int ret = 0;

    std::string home(getenv("HOME"));
    ret = sodium::sodium_init();
    if (ret < 0)
        return db;

    ret = checkFolder(home + dirname);
    if (ret >= 0) {
        ret = checkFiles(home + dirname);
        if (ret < 0)
            return db;
    }

    if ((checkLinux() == 0) && isFileEmpty(home + dirname + "private.key"))
        createKeypair((home + dirname + "public.key"), (home + dirname + "private.key"));

    getPrivateKey(home + dirname + "private.key", priv_key);

    db = std::make_shared<Database>((home + dirname + "test.db").c_str());

    return db;
}
} /* namespace init */
