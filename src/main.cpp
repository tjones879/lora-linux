#include <iostream>

#include "inc/utils.hpp"
#include "inc/sodium_header.hpp"
#include "inc/init.hpp"
#include "inc/db.hpp"
#include <iostream>
#include "inc/driver.hpp"
#include "inc/message.hpp"
#include <future>
#include <sstream>
#include <string>

int main(void)
{
    std::cout << "LORA LINUX" << std::endl;
    int ret = 0;
    unsigned char priv_key[crypto_box_SECRETKEYBYTES];
    std::unique_ptr<sql::Database> db = init::initialize(priv_key);
    auto sp = driver::SerialPort("/dev/ttyACM0");
    {
        auto contents = std::vector<unsigned char>();
        contents.push_back('1');
        auto msgHead = MsgHeader();
        auto msgTail = MsgTail();
        auto message = Message(msgHead, contents, msgTail);
        auto serialized = message.serialize();
        std::cout << "Writing: " << message.serialize().size() << std::endl;
        for (int i = 0; i < serialized.size(); i++)
            printf("%02X", serialized[i]);
        std::cout << "END" << std::endl;
        sp.send(message.serialize());
    }
    auto read_handle = std::async(std::launch::async, driver::poll, sp);
    std::string input;
    int receivedMessage = 0;
    while (1) {
        if (receivedMessage > 0)
            promptNewMsg(receivedMessage);
        if (promptSendMessage(sp, db) < 0)
            break;
    }
    return ret;
}
