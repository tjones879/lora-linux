#include <utility>

#include "inc/contact.hpp"

void checkStatus(int status)
{
    std::cout << "STATUS CODE: " << status << std::endl;
}

void createContactsTable(std::shared_ptr<Database> db) {
    SQLPrepStatement stmt(std::move(db),
            "CREATE TABLE IF NOT EXISTS contacts\
             (id INTEGER PRIMARY KEY,\
              fname TEXT,\
              lname TEXT,\
              pubkey BLOB,\
              notes TEXT);");
    stmt.step();
}

Contact::Contact()
    : id(0)
    , firstName("N/A")
    , lastName("N/A")
    , notes("N/A")
{}

Contact ContactTable::getContactById(int64_t id)
{
    Contact contact;
    SQLPrepStatement stmt(db,
            "SELECT * FROM contacts WHERE id=?;");

    stmt.bind(id, 1);
    int status = stmt.step();

    if (status == SQLITE_ROW) {
        contact.id = std::get<int64_t>(stmt.column(0));
        contact.firstName = std::get<std::string>(stmt.column(1));
        contact.lastName = std::get<std::string>(stmt.column(2));
        contact.pubKey = std::get<std::vector<unsigned char>>(stmt.column(3));
        contact.notes = std::get<std::string>(stmt.column(4));
    }

    return contact;
}

Contact ContactTable::addContact(Contact contact)
{
    int status = 0;

    SQLPrepStatement stmt(db,
            "INSERT INTO contacts (fname, lname, pubkey, notes)\
            VALUES (?, ?, ?, ?);");

    stmt.bind(contact.firstName, 1);
    stmt.bind(contact.lastName, 2);
    stmt.bind(contact.pubKey, 3);
    stmt.bind(contact.notes, 4);
    stmt.step();

    contact.id = sqlite3_last_insert_rowid(db->connection());
    return contact;
}

Contact ContactTable::updateContact(Contact contact)
{
    SQLPrepStatement stmt(db,
            "INSERT OR REPLACE INTO contacts (id, fname, lname, pubkey, notes)\
            VALUES (?, ?, ?, ?, ?);");

    if (contact.id == 0)
        return Contact();


    stmt.bind(contact.id, 1);
    stmt.bind(contact.firstName, 2);
    stmt.bind(contact.lastName, 3);
    stmt.bind(contact.pubKey, 4);
    stmt.bind(contact.notes, 5);
    stmt.step();

    return contact;
}

void ContactTable::removeContact(int64_t id)
{
    SQLPrepStatement stmt(db,
            "DELETE FROM contacts WHERE id=?");

    stmt.bind(id, 1);
    stmt.step();
}

void ContactTable::removeContact(Contact contact)
{
    removeContact(contact.id);
}
