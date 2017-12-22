#pragma once

#include <stdint.h>
#include <vector>
#include <zlib.h>

enum class DataType : uint8_t {
    TEXT,
    STREAM,
    OTHER
};

class Compressor {
public:
    static std::vector<unsigned char>
    compress(std::vector<unsigned char> message, DataType messageType);
    static std::vector<unsigned char>
    expand(std::vector<unsigned char> message, DataType messageType);
};
