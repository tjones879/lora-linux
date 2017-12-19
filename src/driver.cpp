#include "inc/driver.hpp"
#include <chrono>
#include <thread>

namespace driver {

bool SerialPort::isValid() {
    return serialFD > 0;
}

SerialPort::SerialPort(const std::string& name) {
    serialFD = open(name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY | O_CLOEXEC);
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

int SerialPort::send(std::vector<unsigned char> message) {
    if (!isValid())
        return -1;

    int err = write(serialFD, &message[0], message.size());
    if (err < 0)
        std::cout << "Write failed" << std::endl;

    return err;
}

std::vector<unsigned char> SerialPort::receive() {
    auto ret = std::vector<unsigned char>();
    // TODO
    return ret;
}

int SerialPort::selectRead() const {
    struct timeval timeout{};
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    fd_set read_fds{};
    FD_ZERO(&read_fds);
    FD_SET(serialFD, &read_fds);

    return ::select(serialFD + 1, &read_fds, nullptr, nullptr, &timeout);
};

int SerialPort::selectWrite() const {
    struct timeval timeout{};
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    fd_set write_fds{};
    FD_ZERO(&write_fds);
    FD_SET(serialFD, &write_fds);

    return select(serialFD + 1, nullptr, &write_fds, nullptr, &timeout);
};

int SerialPort::close() {
    auto retval = ::close(serialFD);
    return retval;
}

void poll(SerialPort sp) {
    unsigned char buff[1024];
    char hex[1024];
    memset(&buff, 0, sizeof(buff[0]) * 1024);
    while (1) {
        int err = sp.selectRead();
        if (err == 1) {
            auto len = read(sp.serialFD, &buff, 1024);
            if (len > 0) {
                std::cout << "READ: ";
                for (int i = 0; i < len; i++)
                    printf("%02X", buff[i]);
                //std::cout.write(reinterpret_cast<char *>(&buff), len);
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

void ping(SerialPort sp) {
    auto zero = std::vector<unsigned char>();
    zero.push_back('0');
    zero.push_back('1');
    zero.push_back('1');
    auto one = std::vector<unsigned char>();
    one.push_back('1');
    one.push_back('0');
    one.push_back('0');
    while(1) {
        int status = sp.selectWrite();
        if (status == 1) {
            int err = sp.send(one);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            err = sp.send(zero);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else if (status < 0) {
            perror("Error in ping");
        }
    }
}
} // namespace driver
