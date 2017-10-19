#ifndef INIT_H
#define INIT_H
#include <vector>

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
int initialize(unsigned char *priv_key);
} /* namespce init */
#endif /* INIT_H */
