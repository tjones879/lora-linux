#include <iostream>

#include "inc/compress.hpp"
#include "lib/shoco/shoco.h"

std::vector<unsigned char>
Compressor::compress(std::vector<unsigned char> message, DataType messageType)
{
    size_t compressedSize = 0;
    std::vector<unsigned char> outputBuffer(message.size());

    switch (messageType) {
    case DataType::TEXT: {
        // TODO: Check message size first, if large then use zlib.
        compressedSize = shoco_compress(reinterpret_cast<char* >(&message[0]),
                                        message.size(),
                                        reinterpret_cast<char *>(&outputBuffer[0]),
                                        outputBuffer.size());
        outputBuffer.resize(compressedSize);
        std::cout << "Original size: " << message.size() << std::endl;
        std::cout << "Compressed size: " << compressedSize << std::endl;
        break;
    }
    case DataType::OTHER: {
        // TODO: Implement zlib usage here.
        break;
    }
    default:
        break;
    }

    return outputBuffer;
}

std::vector<unsigned char>
Compressor::expand(std::vector<unsigned char> message, DataType messageType)
{
    size_t expandedSize = 0;
    std::vector<unsigned char> outputBuffer(message.size()*2);

    switch (messageType) {
    case DataType::TEXT: {
        // TODO: Check the message size and use zlib if too large.
        expandedSize = shoco_decompress(reinterpret_cast<char *>(&message[0]),
                                        message.size(),
                                        reinterpret_cast<char *>(&outputBuffer[0]),
                                        outputBuffer.size());
        outputBuffer.resize(expandedSize);
        std::cout << "Compressed size: " << message.size() << std::endl;
        std::cout << "Expanded size: " << expandedSize << std::endl;
        break;
    }
    case DataType::OTHER: {
        // TODO: Implement zlib usage here.
        break;
    }
    default:
        break;
    }

    return outputBuffer;
}
