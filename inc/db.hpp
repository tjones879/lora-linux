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

enum class SqliteType : char {
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

using sqlite3_values = std::variant<std::vector<unsigned char>, std::string, int>;
template <typename T>
using sqlite3_handler = std::unique_ptr<T, std::function<void(T *)>>;
}

namespace model {
class ColumnSpec {
public:
    std::string name;
    sql::TypeConverter type;
    std::string flags;
    ColumnSpec() : type(sql::SqliteType::UNRECOGNIZED) {}
    ColumnSpec(std::string n, sql::SqliteType t, std::string f) : name(n), type(t), flags(f) {}
};
std::ostream &operator<<(std::ostream &output, const std::vector<ColumnSpec> columns);

class TableSpec {
public:
    std::string name;
    std::vector<ColumnSpec> columns;
    TableSpec() {}
    TableSpec(const std::string &name, const std::vector<ColumnSpec> &cols);
};

class Row {
protected:
    TableSpec table;
    std::vector<std::tuple<sql::sqlite3_values, ColumnSpec &>> entries;
public:
    virtual const TableSpec &getTable() const =0;
    virtual std::string values() const =0;
    virtual ~Row() {}
};

}

namespace sql {
class Database {
    private:
        sqlite3_handler<sqlite3> db;
        sqlite3_handler<sqlite3_stmt> prepStmt;
        std::string statement;
        int executeStatement(int (*callback)(void *, int, char **, char **), void *first_arg, char **error_msg);
        void bindValues(const std::vector<std::tuple<sqlite3_values, model::ColumnSpec>> &values);
   public:
        Database(const std::string &db);
        ~Database();
        bool tableExists(const std::string &table);
        void createTable(const std::string &name, const std::vector<std::string> &cols);
        void prepInsertStatement(const model::TableSpec &table,
                                 const std::vector<model::ColumnSpec> &columns,
                                 const std::vector<std::tuple<sqlite3_values, model::ColumnSpec>> &values);
        std::string getStatement() const;
        friend Database &operator <<(Database &db, const std::string &txt);
};

}
#endif /* DB_H */
