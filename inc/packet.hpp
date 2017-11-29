#ifndef PACKET_H
#define PACKET_H

#include <cstdint>
#include <vector>

/**
 * PacketTypes defines all acceptable types of packets being sent
 * between the uC and user's device.
 *
 * Values below 0x7F should only flow uC -> Host,
 *        above 0x7F should only flow Host -> uC.
 */
enum class PacketTypes : uint8_t {
    NIL_PACKET = 0x00,
    ACK = 0x01,
    RECV_MSG = 0x02,
    EMPTY_PACKET = 0x7F,
    SEND_MSG = 0x80,
};

/**
 * Packet defines the expected protocol between the
 * uC and the user's device. The structure of this
 * will match both on the uC and the host device.
 */
class Packet {
    PacketTypes type;
    std::vector<unsigned char> body;
public:
    std::vector<unsigned char> serialize() const;
};

#endif
