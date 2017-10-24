#include "inc/db.hpp"
#include "inc/utils.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <vector>
#include <tuple>

namespace model {
TableSpec::TableSpec(const std::string &name, const std::vector<ColumnSpec> &cols)
{
    this->name = name;
    columns = std::vector<ColumnSpec>(cols.size());
    std::copy(cols.begin(), cols.end(), std::back_inserter(columns));
}

std::ostream &operator<<(std::ostream &output, const std::vector<ColumnSpec> columns)
{
    output << "(";
    for (auto col = columns.begin(); col != columns.end(); col++) {
        output << col->name;
        if (col < columns.end() - 1)
            output << ", ";
    }
    output << ")";
    return output;
}
}

namespace sql {
SqliteType TypeConverter::strToSqliteType(const std::string &str)
{
    SqliteType ret;
    if (str == "int")
        ret = SqliteType(SqliteType::INT);
    else if (str == "text")
        ret = SqliteType(SqliteType::TEXT);
    else if (str == "blob")
        ret = SqliteType(SqliteType::BLOB);
    else
        ret = SqliteType(SqliteType::UNRECOGNIZED);
    return ret;
}

TypeConverter::operator std::string() const
{
    std::string text;
    switch (type) {
        case SqliteType::INT:
            text = "int";
            break;
        case SqliteType::TEXT:
            text = "text";
            break;
        case SqliteType::BLOB:
            text = "blob";
            break;
        case SqliteType::UNRECOGNIZED:
            text = "";
            break;
    }
    return text;
}

Database::Database(const std::string &dbFile)
{
    sqlite3 *temp_sqlite3;
    int rc = sqlite3_open(dbFile.c_str(), &temp_sqlite3);
    db = sqlite3_handler<sqlite3> (temp_sqlite3, [](sqlite3 *closing){ sqlite3_close(closing);});
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
    db.executeStatement(NULL, NULL, NULL);
    return db;
}

void Database::bindValues(const std::vector<std::tuple<sqlite3_values, model::ColumnSpec>> &values)
{
    int index = 1;
    for (auto val : values) {
        auto var = std::get<0>(val);
        auto spec = std::get<1>(val);

        switch(spec.type) {
        case SqliteType::TEXT: {
            auto tmp = std::get<std::string>(var);
            sqlite3_bind_text(prepStmt.get(), index, tmp.c_str(), tmp.size() + 1, SQLITE_TRANSIENT);
            break;
        }
        case SqliteType::INT:
            sqlite3_bind_int(prepStmt.get(), index, std::get<int>(var));
            break;
        case SqliteType::BLOB: {
            auto tmp = std::get<std::vector<unsigned char>>(var);
            sqlite3_bind_blob(prepStmt.get(), index, &tmp[0], tmp.size(), SQLITE_TRANSIENT);
            break;
        }
        case SqliteType::UNRECOGNIZED:
            break;
        }
        index++;
    }
}

void Database::prepInsertStatement(const model::TableSpec &table,
                                   const std::vector<model::ColumnSpec> &columns,
                                   const std::vector<std::tuple<sqlite3_values, model::ColumnSpec>> &values)
{
    std::stringstream ss;
    ss << "INSERT INTO " << table.name << columns;
    ss << " VALUES(";
    for (auto col = columns.begin(); col < columns.end(); col++) {
        ss << "?";
        if (col < columns.end() - 1)
            ss << ", ";
    }
    ss << ")";
    sqlite3_stmt *tmp;
    int rc = sqlite3_prepare_v2(db.get(), ss.str().c_str(), ss.str().size(), &tmp, nullptr);
    if (rc != SQLITE_OK)
        std::cout << "sqlite3_prepare_v2 error: " << rc << std::endl;

    prepStmt = sqlite3_handler<sqlite3_stmt> (tmp, [](auto *fin){ sqlite3_finalize(fin); });
    bindValues(values);
    rc = sqlite3_step(prepStmt.get());
    if (rc != SQLITE_DONE)
        std::cout << "sqlite3_step error: " << rc << std::endl;
}
} /* namespace sqlite */
