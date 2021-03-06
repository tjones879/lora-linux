#include <iostream>

#include "inc/contact.hpp"
#include "inc/db.hpp"
#include "inc/driver.hpp"
#include "inc/init.hpp"
#include "inc/message.hpp"
#include "inc/sodium_header.hpp"
#include "inc/utils.hpp"
#include "inc/chat.hpp"
#include <future>
#include <iostream>
#include <sstream>
#include <string>

void promptNewMsg(int &receivedMessage) {
    std::string input;
    while (receivedMessage > 0) {
        std::cout << "You've got mail!" << std::endl;
        std::cout << "Would you like to open it?" << std::endl;
        std::cin >> input;
        if (input == "Y") {
            std::cout << "MESSAGE CONTENTS" << std::endl;
            receivedMessage -= 1;
        } else if (input == "N") {
            std::cout << "OK" << std::endl;
            break;
        }
    }
}

int promptSendMessage(driver::SerialPort & /*sp*/, Database & /*db*/) {
    std::string input;
    std::cout << "Who would you like to message?" << std::endl;
    std::cin >> input;

    std::cout << "What would you like to say?" << std::endl;
    std::stringstream ss;
    while (getline(std::cin, input)) {
        if (input == "BREAK")
            return -1;
        ss << input ;
    }
    std::cout << "You want to send: '" << ss.str() << "'? (Y/N)" << std::endl;
    std::cin >> input;
    if (input == "Y") {
        std::cout << "Sending and encrypting" << std::endl;
        std::cout << "Encrypted message: " << std::endl;

    } else if (input == "N") {
        return -1;
    }
    return 0;
}

int main()
{
    std::cout << "LORA LINUX" << std::endl;
    /*
    int ret = 0;
    unsigned char priv_key[crypto_box_SECRETKEYBYTES];
    std::unique_ptr<Database> db = init::initialize(priv_key);
    auto sp = driver::SerialPort("/dev/ttyACM0");
    {
        auto contents = std::vector<unsigned char>();
        contents.push_back('1');
        auto msgHead = MsgHeader();
        auto msgTail = MsgTail();
        auto message = Message(msgHead, contents, msgTail);
        auto serialized = message.serialize();
        std::cout << "Writing: " << message.serialize().size() << std::endl;
        for (unsigned char i : serialized)
            printf("%02X", i);
        std::cout << "END" << std::endl;
        sp.send(message.serialize());
    }
    auto read_handle = std::async(std::launch::async, driver::poll, sp);
    std::string input;
    int receivedMessage = 0;
    while (true) {
        if (receivedMessage > 0)
            promptNewMsg(receivedMessage);
    }
    */
    unsigned char priv_key[crypto_box_SECRETKEYBYTES];
    std::shared_ptr<Database> db = init::initialize(priv_key);
    ContactTable contacts(db);
    SentChatTable sent(db);
    ReceivedChatTable received(db);
    int ret = 0;
    return ret;
}
