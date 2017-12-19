#include "inc/db.hpp"
#include "inc/utils.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>

void closeSQLite(sqlite3 *conn) {
    sqlite3_close(conn);
}

Database::Database(const std::string &dbFile)
{
    sqlite3 *temp_sqlite3;
    int rc = sqlite3_open(dbFile.c_str(), &temp_sqlite3);
    db = SQLHandler<sqlite3> (temp_sqlite3, closeSQLite);
    if (rc != 0)
        db.reset();
}

Database::~Database() = default;

/*
bool Database::tableExists(const std::string &table)
{
    this->statement = "SELECT name from sqlite_master WHERE type='table' AND name='" + table + "'";
    char *error_msg{};
    int tables = 0;
    executeStatement(+[](void *count, int argc, char **, char **){
                            int *c = reinterpret_cast<int *>(count);
                            *c = argc;
                            return 0;
                        }, &tables, &error_msg);
    if (error_msg)
        std::cout << error_msg << std::endl;
    sqlite3_free(error_msg);
    return tables > 0;
}
*/
