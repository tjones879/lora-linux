#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <vector>

enum class MessageTypes : uint8_t {
    NULL_TYPE = 0x00,
    TEXT = 0x01,
    VOICE = 0x02,
};

/**
 * MsgHeader contains information that is a part of the encrypted
 * message.
 */
class MsgHeader {
    MessageTypes type;
    uint8_t position;  ///< Message number in the message stream
    uint16_t length;   ///< Length of the message in bytes
public:
    MsgHeader() {
        type = MessageTypes::NULL_TYPE;
        position = 0;
        length = 0;
    }
    MsgHeader(MessageTypes msgType, uint8_t pos, uint16_t len) :
        type(msgType), position(pos), length(len) { }
    /**
     * Tightly pack the header into a form suitable for sending
     * to a uC. Form is as expected:
     *   type:      1 byte,
     *   position:  1 bytes,
     *   length:    2 bytes
     */
    uint32_t serialize() const;
};

class MsgTail {
    uint32_t hash;
    uint8_t position; ///< Should match the position in header
public:
    MsgTail() {
        hash = 0;
        position = 0;
    }
    MsgTail(uint32_t hashVal, uint8_t pos) :
        hash(hashVal), position(pos) { }
    /**
     * Tightly pack the tail into a form suitable for sending
     * to a uC. Form is as expected:
     *   magic:    1 byte,   VALUE: 0x1C
     *   hash:     4 bytes,
     *   magic:    1 byte,   VALUE: 0x2A
     *   position: 1 byte,
     *   empty:    1 byte,   VALUE: 0x00   <Should be ignored>
     */
    uint64_t serialize() const;
};

/**
 * Message defines the full packets that will be encrypted before
 * reaching the uC. Used only in the transmission from user to user.
 */
class Message {
    MsgHeader header;
    std::vector<unsigned char> body;
    MsgTail tail;
public:
    Message() { }
    Message(MsgHeader start, std::vector<unsigned char> contents, MsgTail end) :
        header(start), body(contents), tail(end) { }
    /**
     * Pack the message in prep for encryption and being sent
     * over to the uC. The returned vector will be tightly
     * packed and be equal in size to what the other user
     * should receive.
     */
    std::vector<unsigned char> serialize() const;
};

#endif /* MESSAGE_H */
