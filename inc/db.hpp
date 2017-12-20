#pragma once

#include <string>
#include <memory>
#include <functional>
#include <optional>
#include <vector>
#include <variant>
#include <iostream>
#include <type_traits>

#include "sqlite3.h"

template<class T> struct always_false : std::false_type {};

template <typename T>
using SQLHandler = std::shared_ptr<T>;
using SQLCallback = std::function<int(void *, int, char **, char **)>;
using SQLBlob = std::vector<unsigned char>;
using SQLData = std::variant<SQLBlob, int32_t, int64_t, std::string>;

class Database {
private:
    std::shared_ptr<sqlite3> db;
    std::string statement;
public:
    sqlite3 *connection() {
        return db.get();
    }
    Database(const std::string &dbFile);
    ~Database();
    friend Database &operator <<(Database &db, const std::string &txt);
};

struct SQLPrepStatement {
    sqlite3_stmt *stmt;

    SQLPrepStatement(std::shared_ptr<Database> db, std::string query)
    {
        sqlite3_prepare_v2(db->connection(), query.c_str(), -1, &stmt, NULL);
    }

    /// Automatically deallocate the slqite3_stmt on dtor
    ~SQLPrepStatement()
    {
        sqlite3_finalize(stmt);
    }

    /**
     * Bind data to a prepared statement.
     *
     * @param data  The actual data that should be inserted into the query.
     * @param index The position where the value will be bound
     *              (leftmost parameter is 1).
     * @return      SQLITE_OK on success, an error code otherwise.
     */
    int bind(SQLData data, int index)
    {
        int ret = std::visit([&] (auto &&arg) -> int {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int32_t>) {
                return sqlite3_bind_int(stmt, index, arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                auto sqlInt = static_cast<sqlite3_int64>(arg);
                return sqlite3_bind_int64(stmt, index, sqlInt);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return sqlite3_bind_text(stmt, index, arg.c_str(), -1, SQLITE_TRANSIENT);
            } else if constexpr (std::is_same_v<T, SQLBlob>) {
                return sqlite3_bind_blob(stmt, index, &arg[0], arg.size(), SQLITE_TRANSIENT);
            } else {
                static_assert(always_false<T>::value, "non-exhaustive visitor!");
            }
        }, data);

        return ret;
    }

    /**
     * Attempt to step through the statement in a blocking manner.
     *
     * @return  SQLITE_ROW if data is available, SQLITE_DONE if the execution
     *          was successful. Returns an SQLITE error code otherwise.
     */
    int step()
    {
        int status = sqlite3_step(stmt);
        // Keep attempting to step until the engine can obtain a lock.
        while (status == SQLITE_BUSY) {
            status = sqlite3_step(stmt);
        }
        return status;
    }
};

class Table {
private:
    std::shared_ptr<Database> db;
    const std::string tableName;
protected:
    void buildTable();
    /**
     * Construct an interface with the database for this specific table.
     *
     * We must check to see if the table exists and callback if it doesn't.
     * @param database  Pointer to a database connection.
     * @param name      Name of table in database
     * @param callback  Function to be called if the table does not exist.
     */
    Table(std::shared_ptr<Database> database, const std::string &name,
          std::function<void(std::shared_ptr<Database> database)> callback)
        : db(database), tableName(name)
    {
        const std::string query("SELECT name FROM sqlite_master WHERE type='table' AND name=?");
        SQLPrepStatement stmt(db, query);
        stmt.bind(tableName, 1);

        if (stmt.step() != SQLITE_ROW)
            callback(db);
    }
public:
    bool tableExists();
};
