#include <utility>

#include "inc/contact.hpp"

void createContactsTable(std::shared_ptr<Database> db) {
    std::cout << "TABLE contacts DID NOT EXIST" << std::endl;
    SQLPrepStatement stmt(std::move(db),
            "CREATE TABLE IF NOT EXISTS contacts\
             (id INTEGER PRIMARY KEY,\
              fname VARCHAR(25),\
              lname VARCHAR(25),\
              pubkey BLOB);");
    stmt.step();
}
