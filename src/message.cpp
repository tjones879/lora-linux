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


std::vector<unsigned char> Message::serialize() const {
    // We must calculate the size of the message being sent, this is
    // simply the packed sum of all parts.
    auto start = header.serialize();
    auto end = tail.serialize();
    auto retVec = std::vector<unsigned char>(body.size() + sizeof(start) + sizeof(end));

    size_t offset = 0;

    // Since we know the size of each component from above, just
    // directly copy values into the underlying array of values.
    memcpy(&retVec[0], &start, sizeof(start));
    offset += sizeof(start);
    memcpy(&retVec[offset], &body, body.size());
    offset += body.size();
    memcpy(&retVec[offset], &end, sizeof(end));

    return retVec;
}
