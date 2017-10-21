#ifndef DB_H
#define DB_H

#include <string>
#include <memory>
#include <functional>
#include <optional>
#include <vector>
#include <variant>

namespace sql {
#ifdef __cpluplus
extern "C" {
#endif

#include "sqlite3.h"

#ifdef __cpluplus
}
#endif

enum class SqliteType {
    TEXT,
    BLOB,
    INT,
    UNRECOGNIZED
};

class TypeConverter {
private:
    const SqliteType type;
    SqliteType strToSqliteType(const std::string &str);
public:
    TypeConverter(SqliteType t) : type(t) {}
    TypeConverter(const std::string &str) : type(strToSqliteType(str)) {}
    operator std::string () const;
    operator SqliteType () const { return type; }
};

using sqlite3_values = std::variant<std::vector<unsigned char> &, std::vector<char> &>;
using sqlite3_handler = std::unique_ptr<sqlite3, std::function<void(sqlite3 *)>>;

class Database {
    private:
        sqlite3_handler db;
        std::string statement;
        int executeStatement(int (*callback)(void *, int, char **, char **), void *first_arg, char **error_msg);
   public:
        Database(const std::string &db);
        ~Database();
        bool tableExists(const std::string &table);
        void createTable(const std::string &name, const std::vector<std::string> &cols);
        std::string getStatement() const;
        friend Database &operator <<(Database &db, const std::string &txt);
};

} /* namespace db */

namespace model {
class ColumnSpec {
public:
    std::string name;
    sql::TypeConverter type;
    std::string flags;
};

class TableSpec {
public:
    std::string name;
    std::vector<ColumnSpec> columns;
};

class Row {
private:
    TableSpec table;
    std::vector<std::tuple<sql::sqlite3_values, ColumnSpec &>> entries;
public:
    virtual TableSpec &getTable();
    virtual std::string values();
    virtual ~Row();
};

}
#endif /* DB_H */
