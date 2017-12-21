#include "inc/db.hpp"
#include "inc/utils.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>

Database::Database(const std::string &dbFile)
{
    sqlite3 *temp_sqlite3;
    int rc = sqlite3_open(dbFile.c_str(), &temp_sqlite3);
    db = std::shared_ptr<sqlite3>(temp_sqlite3, sqlite3_close);
    if (rc != 0)
        db.reset();
}

Database::~Database() = default;

sqlite3 *Database::connection() {
    return db.get();
}

SQLPrepStatement::SQLPrepStatement(std::shared_ptr<Database> db, std::string query)
{
    sqlite3_prepare_v2(db->connection(), query.c_str(), -1, &stmt, NULL);
}

SQLPrepStatement::~SQLPrepStatement()
{
    sqlite3_finalize(stmt);
}

int SQLPrepStatement::step()
{
    int status = sqlite3_step(stmt);
    // Keep attempting to step until the engine can obtain a lock.
    while (status == SQLITE_BUSY) {
        status = sqlite3_step(stmt);
    }
    return status;
}

int SQLPrepStatement::bind(SQLData data, int index)
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

SQLData SQLPrepStatement::column(int index)
{
    SQLData ret;
    switch (sqlite3_column_type(stmt, index)) {
    case SQLITE_BLOB: {
        int size = sqlite3_column_bytes(stmt, index);
        ret = std::vector<unsigned char>(size);
        const void *contents = sqlite3_column_blob(stmt, index);
        memcpy(&std::get<std::vector<unsigned char>>(ret)[0], contents, size);
        break;
    }
    case SQLITE_TEXT: {
        int size = sqlite3_column_bytes(stmt, index);
        auto text = (const char*) sqlite3_column_text(stmt, index);
        ret = std::string(text, size + 1);
        break;
    }
    case SQLITE_INTEGER:
        ret = static_cast<int64_t>(sqlite3_column_int64(stmt, index));
        break;
    }

    return ret;
}

Table::Table(std::shared_ptr<Database> database, const std::string &name,
             std::function<void(std::shared_ptr<Database> database)> callback)
    : db(database), tableName(name)
{
    const std::string query("SELECT name FROM sqlite_master WHERE type='table' AND name=?");
    SQLPrepStatement stmt(db, query);
    stmt.bind(tableName, 1);

    if (stmt.step() != SQLITE_ROW)
        callback(db);
}
