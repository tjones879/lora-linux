#pragma once

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include "inc/db.hpp"

struct Contact {
    int64_t id;
    std::vector<unsigned char> pubKey;
    std::string firstName;
    std::string lastName;
    std::string notes;

    Contact();
    Contact(int64_t id, std::vector<unsigned char> publicKey, std::string fName, std::string lName, std::string note);
};

void createContactsTable(std::shared_ptr<Database> db);

class ContactTable : public Table {
public:
    ContactTable(std::shared_ptr<Database> db)
        : Table(db, "contacts", createContactsTable) {}
};
