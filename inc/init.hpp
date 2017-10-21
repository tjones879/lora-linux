#ifndef INIT_H
#define INIT_H
#include <vector>
#include <memory>
#include "inc/db.hpp"

namespace init
{
struct cryptoKey {
    enum class Type {
        RSA_PUB,
        RSA_PRIVATE,
        AES_SHARED
    };
    std::vector<char> value;
};
/**
 *
 *
 */
std::unique_ptr<sql::Database> initialize(unsigned char *priv_key);
} /* namespce init */
#endif /* INIT_H */
