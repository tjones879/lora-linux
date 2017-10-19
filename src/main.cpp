#include <iostream>

#include "inc/utils.hpp"
#include "inc/sodium_header.hpp"
#include "inc/init.hpp"
#include <iostream>

static int box();

static int box(void)
{
    unsigned char bob_pk[crypto_box_PUBLICKEYBYTES]; /* Bob's public key */
    unsigned char bob_sk[crypto_box_SECRETKEYBYTES]; /* Bob's secret key */

    unsigned char alice_pk[crypto_box_PUBLICKEYBYTES]; /* Alice's public key */
    unsigned char alice_sk[crypto_box_SECRETKEYBYTES]; /* Alice's secret key */

    unsigned char nonce[crypto_box_NONCEBYTES];
    unsigned char message[MAX_INPUT_LEN];
    unsigned char ciphertext[crypto_box_MACBYTES + MAX_INPUT_LEN];
    size_t        message_len;
    size_t        ciphertext_len;
    int           ret;

    puts("Example: crypto_box_easy\n");

    puts("Generating keypairs...\n");
    sodium::crypto_box_keypair(bob_pk, bob_sk);     /* generate Bob's keys */
    sodium::crypto_box_keypair(alice_pk, alice_sk); /* generate Alice's keys */

    puts("Bob");
    printf("Public key: ");
    utils::print_hex(bob_pk, sizeof bob_pk, std::cout);
    printf("Secret key: ");
    utils::print_hex(bob_sk, sizeof bob_sk, std::cout);

    puts("Alice");
    printf("Public key: ");
    utils::print_hex(alice_pk, sizeof alice_pk, std::cout);
    printf("Secret key: ");
    utils::print_hex(alice_sk, sizeof alice_sk, std::cout);

    /* nonce must be unique per (key, message) - it can be public and deterministic */
    puts("Generating nonce...");
    sodium::randombytes_buf(nonce, sizeof nonce);
    printf("Nonce: ");
    utils::print_hex(nonce, sizeof nonce, std::cout);

    /* read input */
    message_len = utils::prompt_input("a message", message, sizeof(message), 1);

    utils::print_hex(message, message_len, std::cout);

    /* encrypt and authenticate the message */
    printf("Encrypting and authenticating with %s\n\n", sodium::crypto_box_primitive());
    sodium::crypto_box_easy(ciphertext, message, message_len, nonce, alice_pk, bob_sk);
    ciphertext_len = crypto_box_MACBYTES + message_len;

    /* send the nonce and the ciphertext */
    puts("Bob sends the nonce and the ciphertext...\n");
    printf("Ciphertext len: %zu bytes - Original message length: %zu bytes\n",
           ciphertext_len, message_len);
    puts("Notice the prepended 16 byte authentication token\n");
    printf("Nonce: ");
    utils::print_hex(nonce, sizeof nonce, std::cout);
    printf("Ciphertext: ");
    utils::print_hex(ciphertext, ciphertext_len, std::cout);

    /* decrypt the message */
    puts("Alice verifies and decrypts the ciphertext...");
    ret = sodium::crypto_box_open_easy(message, ciphertext, ciphertext_len, nonce, bob_pk,
                               alice_sk);
    utils::print_hex(message, message_len, std::cout);

    utils::print_verification(ret);
    if (ret == 0) {
        printf("Plaintext: ");
        fwrite(message, 1U, message_len, stdout);
        putchar('\n');
    }
    sodium::sodium_memzero(bob_sk, sizeof bob_sk); /* wipe sensitive data */
    sodium::sodium_memzero(alice_sk, sizeof alice_sk);
    sodium::sodium_memzero(message, sizeof message);
    sodium::sodium_memzero(ciphertext, sizeof ciphertext);

    return ret;
}

int main(void)
{
    int ret = 0;
    unsigned char priv_key[crypto_box_SECRETKEYBYTES];
    ret = init::initialize(priv_key);
    std::cout << "Private key in hex: ";
    utils::print_hex(priv_key, sizeof(priv_key), std::cout);
    if (ret < 0)
        return ret;
    utils::init();


    return box() != 0;
}
