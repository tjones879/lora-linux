#pragma once

#include <string>
#include <memory>
#include <functional>
#include <optional>
#include <vector>
#include <variant>

#include "sqlite3.h"

template <typename T>
using SQLHandler = std::unique_ptr<T, std::function<void(T *)>>;
using SQLCallback = std::function<int(void *, int, char **, char **)>;

class Database {
private:
    SQLHandler<sqlite3> db;
    SQLHandler<sqlite3_stmt> prepStmt;
    std::string statement;
public:
    Database(const std::string &dbFile);
    ~Database();
    friend Database &operator <<(Database &db, const std::string &txt);
};

class Table {
private:
    std::shared_ptr<Database> db;
    const std::string tableName;
protected:
    void buildTable();
    Table(Database *database, const std::string &name) : db(database), tableName(name) {}
    Table(const std::string &name);
public:
    bool tableExists();
};
