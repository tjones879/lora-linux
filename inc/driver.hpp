#ifndef DRIVER_H
#define DRIVER_H

#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

namespace driver {
/*
    if (fd < 0) {
        std::cout << "open failed: " << fd << std::endl;
    } else {
        int err = write(fd, "0", 1);
        if (err < 0) {
            std::cout << "write failed: " << err << std::endl;
        }
        close(fd);
    }
 */
class SerialPort {
    int serialFD;
    bool isValid() {
        return serialFD > 0;
    }
public:
    SerialPort(std::string name) {
        serialFD = open(name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if (serialFD < 0) {
            std::cout << "Open failed" << std::endl;
        }
    }

    ~SerialPort() {
        close(serialFD);
    }

    int send(std::vector<unsigned char> message) {
        if (!isValid())
            return -1;

        int err = write(serialFD, &message[0], message.size());
        if (err < 0)
            std::cout << "Write failed" << std::endl;

        return err;
    }
};

}; /* namespace driver */
#endif /* DRIVER_H */
