#include <iostream>

#include "inc/utils.hpp"
#include "inc/sodium_header.hpp"
#include "inc/init.hpp"
#include "inc/db.hpp"
#include <iostream>
#include "inc/driver.hpp"
#include <future>

int main(void)
{
    int ret = 0;
    unsigned char priv_key[crypto_box_SECRETKEYBYTES];
    std::unique_ptr<sql::Database> db = init::initialize(priv_key);
    auto sp = driver::SerialPort("/dev/ttyACM0");
    auto message = std::vector<unsigned char>();
    message.push_back('1');
    sp.send(message);
    auto read_handle = std::async(std::launch::async, driver::poll, sp);
    auto send_handle = std::async(std::launch::async, driver::ping, sp);
    /*
    auto first = std::make_tuple("bob", name);
    std::vector<unsigned char> list;
    list.push_back(97);
    list.push_back(98);
    list.push_back(99);
    auto second = std::make_tuple(list, pubkey);
    auto third = std::make_tuple(987, nonce);

    std::vector<std::tuple<sql::sqlite3_values, model::ColumnSpec>> values;
    values.push_back(first);
    values.push_back(second);
    values.push_back(third);

    db->prepInsertStatement(table, cols, values);
    */
    read_handle.get();
    send_handle.get();
    sp.close();
    return ret;
}
