#include "inc/packet.hpp"

std::vector<unsigned char> Packet::serialize() const {
    auto retVec = std::vector<unsigned char>(sizeof(PacketTypes) + body.size());
    auto offset = 0;

    memcpy(&retVec[offset], &type, sizeof(type));
    offset += sizeof(type);
    memcpy(&retVec[offset], &body[0], body.size());

    return retVec;
}
