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
    /**
     * Attempt to get a contact by id.
     *
     * @return A valid contact on success. A contact with id 0 on failure.
     */
    Contact getContactById(int64_t id);
    /**
     * Attempt to add a contact to the database.
     *
     * @param contact Instace to insert into the table. id does not need to be set.
     * @return        Inserted contact with correct id.
     */
    Contact addContact(Contact contact);
    /**
     * Attempt to update a contact inside the db.
     *
     * @param contact Instance to update inside the table. Must have id set.
     * @return        Contact that was passed in if successful.
     */
    Contact updateContact(Contact contact);
    /// Remove a contact by instance. Must have id set.
    void removeContact(Contact contact);
    /// Remove a contact by id.
    void removeContact(int64_t id);
};
