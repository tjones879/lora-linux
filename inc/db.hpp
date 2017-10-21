#ifndef DB_H
#define DB_H

#include <string>
#include <memory>
#include <functional>
#include <vector>

namespace sql {
#ifdef __cpluplus
extern "C" {
#endif

#include "sqlite3.h"

#ifdef __cpluplus
}
#endif

class RowSpec {
public:
    std::string name;
    std::string typeName;
    std::string flags;
};

class TableSpec {
public:
    std::string name;
    std::vector<RowSpec> rows;
};


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
#endif /* DB_H */
