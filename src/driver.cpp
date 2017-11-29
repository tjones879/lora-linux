#include "inc/driver.hpp"

namespace driver {

bool SerialPort::isValid() {
    return serialFD > 0;
}

SerialPort::SerialPort(std::string name) {
    serialFD = open(name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (serialFD < 0) {
        std::cout << "Open failed" << std::endl;
    } else {
        fcntl(serialFD, F_SETFL, FNDELAY);
    }
}

SerialPort::~SerialPort() {
    if (isValid())
        close(serialFD);
}

int SerialPort::send(std::vector<unsigned char> message) {
    if (!isValid())
        return -1;

    int err = write(serialFD, &message[0], message.size());
    if (err < 0)
        std::cout << "Write failed" << std::endl;
    else
        std::cout << "Write successful" << std::endl;

    return err;
}

std::vector<unsigned char> SerialPort::receive() {
    auto ret = std::vector<unsigned char>();
    if (!isValid()) {
        return ret;
    } else {
        FD_ZERO(&input);
        FD_SET(serialFD, &input);
        int max_fd = serialFD + 1;
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        int retval = select(max_fd, &input, NULL, NULL, &timeout);
        if (retval == -1) {
            perror("select()");
        } else if (retval) {
            std::cout << "Ready to be read" << std::endl;
        } else {
            std::cout << "No output in 10s" << std::endl;
        }
    }
    /*
    unsigned char buffer[1024];
    int err;
    while ((err = read(serialFD, buffer, 1024)) >= 0) {
        std::cout << "Reading" << std::endl;
        ret.insert(ret.end(), &buffer[0], &buffer[err]);
        //if (err == 1024)
        //    err = read(serialFD, buffer, 1024);
        //else
            err = 0;
    }
    if (err < 0) {
        perror("receive");
    }
    */
    return ret;
}

}
