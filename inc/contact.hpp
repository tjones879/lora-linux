#pragma once

#include <vector>
#include <string>
#include <memory>
#include "inc/db.hpp"
/*
class Contact : public Row {
private:
    void setValues(std::string name, std::vector<unsigned char> pubKey, uint32_t nonce);
protected:
public:
    Contact(std::shared_ptr<TableSpec> table, std::string name, std::vector<unsigned char> pubKey, uint32_t nonce);
    Contact(std::string name, std::vector<unsigned char> pubKey, uint32_t nonce);
    void setNonce(uint32_t nonce);
    const std::vector<unsigned char> &getPubKey() const;
    const std::string &getName() const;
    uint32_t getNonce() const;
    std::vector<unsigned char> encryptMessage(const std::vector<unsigned char> message) const;
};
*/


struct Contact {
    int64_t id;
    std::vector<unsigned char> pubKey;
    std::string firstName;
    std::string lastName;
    std::string notes;

    Contact();
    Contact(int64_t id, std::vector<unsigned char> publicKey, std::string fName, std::string lName, std::string note);
};

class ContactTable : public Table {
public:
    ContactTable(Database *db) : Table(db, "contacts") {}
};
