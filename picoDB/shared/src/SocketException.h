#ifndef COMMON_SOCKETEXCEPTION_H_
#define COMMON_SOCKETEXCEPTION_H_

#include <string>

class SocketException {

public:

    SocketException(const std::string &mensaje) : mensaje(mensaje) {

    };

    ~SocketException() { };

    std::string getMensaje() const {

        return mensaje;

    };

private:

    std::string mensaje;

};

#endif /* COMMON_SOCKETEXCEPTION_H_ */
