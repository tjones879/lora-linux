#ifndef DRIVER_H
#define DRIVER_H

#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>

namespace driver {

class SerialPort {
    struct termios tio{};
    bool isValid();
    std::vector<unsigned char> receive();
public:
    int serialFD;
    SerialPort(const std::string& name);

    int send(std::vector<unsigned char> message);
    int selectRead() const;
    int selectWrite() const;
    int close();
};

void poll(SerialPort sp);
void ping(SerialPort sp);

} /* namespace driver */
#endif /* DRIVER_H */
