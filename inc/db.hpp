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
using SQLCallback = std::function<int(void *, int, char **, char **)>;
using SQLBlob = std::vector<unsigned char>;
using SQLData = std::variant<SQLBlob, int32_t, int64_t, std::string>;

class Database {
private:
    std::shared_ptr<sqlite3> db;
    std::string statement;
public:
    sqlite3 *connection();
    Database(const std::string &dbFile);
    ~Database();
    friend Database &operator <<(Database &db, const std::string &txt);
};

struct SQLPrepStatement {
    sqlite3_stmt *stmt{};

    /// Create a safe sqlite3_stmt with a db and query.
    SQLPrepStatement(const std::shared_ptr<Database>& db, const std::string& query);
    /// Automatically deallocate the slqite3_stmt on dtor.
    ~SQLPrepStatement();
    /**
     * Bind data to a prepared statement.
     *
     * @param data  The actual data that should be inserted into the query.
     * @param index The position where the value will be bound
     *              (leftmost parameter is 1).
     * @return      SQLITE_OK on success, an error code otherwise.
     */
    int bind(SQLData data, int index);
    /**
     * Attempt to step through the statement in a blocking manner.
     *
     * @return  SQLITE_ROW if data is available, SQLITE_DONE if the execution
     *          was successful. Returns an SQLITE error code otherwise.
     */
    int step();
    /**
     * Attempt to get a column value when a new row is available.
     *
     * @param index The index of the column (leftmost column is 0).
     */
    SQLData column(int index);
};

class Table {
protected:
    std::shared_ptr<Database> db;
    const std::string tableName;
    /**
     * Construct an interface with the database for this specific table.
     *
     * We must check to see if the table exists and callback if it doesn't.
     * @param database  Pointer to a database connection.
     * @param name      Name of table in database
     * @param callback  Function to be called if the table does not exist.
     */
    Table(std::shared_ptr<Database> database, std::string name,
          const std::function<void(std::shared_ptr<Database> database)>& callback);
public:
};
