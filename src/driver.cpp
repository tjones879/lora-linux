#include "inc/driver.hpp"
#include <thread>
#include <chrono>

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
        memset(&tio, 0, sizeof(tio));
        tio.c_cflag = CS8 | CREAD | CLOCAL;
        tio.c_cc[VMIN] = 1;
        tio.c_cc[VTIME] = 10;

        if (cfsetospeed(&tio, B115200) < 0 || cfsetispeed(&tio, B115200) < 0)
            std::cout << "ERROR";
        int err = tcsetattr(serialFD, TCSANOW, &tio);
        if (err < 0)
            perror("Error in creation");
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

int SerialPort::select() {
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(serialFD, &read_fds);

    return ::select(serialFD + 1, &read_fds, NULL, NULL, &timeout);
};

void poll(SerialPort sp) {
    unsigned char buff[1024];
    memset(&buff, 0, sizeof(buff[0]) * 1024);
    while (1) {
        int err = sp.select();
        if (err == 1) {
            auto len = read(sp.serialFD, &buff, 1024);
            if (len > 0) {
                std::cout.write(reinterpret_cast<char *>(&buff), len);
                std::cout << std::endl;
            } else if (len < 0) {
                std::cout << "FD: " << sp.serialFD << std::endl;
                perror("Error in poll");
            }
        } else if (err < 0) {
            perror("Error in select");
        } else {
            std::cout << "Timed out: " << err << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

}
