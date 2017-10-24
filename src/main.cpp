#include <iostream>

#include "inc/utils.hpp"
#include "inc/sodium_header.hpp"
#include "inc/init.hpp"
#include "inc/db.hpp"
#include <iostream>

int main(void)
{
    int ret = 0;
    unsigned char priv_key[crypto_box_SECRETKEYBYTES];
    std::unique_ptr<sql::Database> db = init::initialize(priv_key);

    std::vector<model::ColumnSpec> cols;
    auto name = model::ColumnSpec("name", sql::SqliteType::TEXT, "");
    auto pubkey = model::ColumnSpec("pubkey", sql::SqliteType::BLOB, "");
    auto nonce = model::ColumnSpec("nonce", sql::SqliteType::INT, "");
    cols.push_back(name);
    cols.push_back(pubkey);
    cols.push_back(nonce);

    model::TableSpec table("contacts", cols);

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

    return ret;
}
