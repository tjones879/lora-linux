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
    std::shared_ptr<TableSpec> table;
    std::vector<std::tuple<sql::sqlite3_values, ColumnSpec &>> entries;
    virtual std::vector<ColumnSpec> buildColSpecs() const =0;
    virtual void buildTableSpec() =0;
public:
    const std::vector<std::tuple<sql::sqlite3_values, ColumnSpec &>> &getEntries() { return entries; }
    std::shared_ptr<TableSpec> getTable() { return table; }
    virtual ~Row() {}
};

class Contact : public Row {
private:
    void setValues(std::string name, std::vector<unsigned char> pubKey, uint32_t nonce);
protected:
    std::vector<ColumnSpec> buildColSpecs() const;
    void buildTableSpec();
public:
    Contact(std::shared_ptr<TableSpec> table, std::string name, std::vector<unsigned char> pubKey, uint32_t nonce);
    Contact(std::string name, std::vector<unsigned char> pubKey, uint32_t nonce);
    void setNonce(uint32_t nonce);
    const std::vector<unsigned char> &getPubKey() const;
    const std::string &getName() const;
    uint32_t getNonce() const;
    std::vector<unsigned char> encryptMessage(const std::vector<unsigned char> message) const;
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

void createNewContact(sql::Database &db) {
    auto first = std::make_tuple("bob", name);
    std::vector<unsigned char> list;
    list.push_back(97);
    list.push_back(98);
    list.push_back(99);
    auto second = std::make_tuple(list, pubkey);
    auto third = std::make_tuple(987, nonce);

    std::vector<std::tuple<sql::sqlite3_values, model::ColumnSpec>> values;
    values.push_back(first);
    values.push_back(second);
    values.push_back(third);

    db->prepInsertStatement(table, cols, values);
}

}
#endif /* DB_H */
