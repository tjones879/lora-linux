#include "inc/db.hpp"
#include "inc/utils.hpp"
#include <iostream>
#include <vector>
#include <tuple>

namespace sql {
/*
class Database {
    public:
        Database(const std::string &db);
        ~Database();
};
*/

Database::Database(const std::string &dbFile)
{
    sqlite3 *temp;
    int rc = sqlite3_open(dbFile.c_str(), &temp);
    db = sqlite3_handler(temp, [](sqlite3 *closing){ sqlite3_close(closing);});
    if (rc)
        db.reset();
}

Database::~Database()
{
}

std::string Database::getStatement() const
{
    return statement;
}

int Database::executeStatement(int (*callback)(void *, int, char **, char **), void *first_arg, char **error_msg)
{
    return sqlite3_exec(db.get(), this->statement.c_str(), callback, first_arg, error_msg);
}

bool Database::tableExists(const std::string &table)
{
    this->statement = "SELECT name from sqlite_master WHERE type='table' AND name='" + table + "'";
    char *error_msg{};
    int count = 0;
    executeStatement(+[](void *count, int argc, char **, char **){
                            int *c = reinterpret_cast<int *>(count);
                            *c = argc;
                            return 0;
                        }, &count, &error_msg);
    if (error_msg)
        std::cout << error_msg << std::endl;
    sqlite3_free(error_msg);
    return count > 0;
}

void Database::createTable(const std::string &name, const std::vector<std::string> &cols)
{
    statement = "CREATE TABLE " + name + "(";
    for (auto column = cols.begin(); column != cols.end(); column++) {
        statement += *column;
        statement += (column < cols.end() - 1) ? "," : ")";
    }
    executeStatement(NULL, NULL, NULL);
}

Database &operator <<(Database &db, const std::string &txt)
{
    db.statement = txt;
    sqlite3_exec(db.db.get(), db.statement.c_str(), NULL, NULL, NULL);
    return db;
}
} /* namespace sqlite */
