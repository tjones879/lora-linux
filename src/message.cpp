#include "inc/message.hpp"

uint32_t MsgHeader::serialize() const {
    uint32_t retval = 0;
    retval = static_cast<uint8_t>(type) << 3;
    retval |= position << 2;
    retval |= length;

    return retval;
}

uint64_t MsgTail::serialize() const {
    uint64_t retval = 0;
    retval = 0x1C << 7;
    retval |= hash << 3;
    retval |= 0x2A << 2;
    retval |= position << 1;
    retval |= 0x00; // Explicitly should be 0

    return retval;
}
