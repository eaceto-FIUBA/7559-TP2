#include "LockFile.h"
#include <stdlib.h>

LockFile::LockFile(const std::string nombre) {

    this->nombre = nombre;
    this->fl.l_type = F_RDLCK;
    this->fl.l_whence = SEEK_SET;
    this->fl.l_start = 0;
    this->fl.l_len = 0;
    //File open for read/write
    this->fd = open(this->nombre.c_str(), O_CREAT | O_RDWR, 0777);
}

int LockFile::tomarLockLectura() {
    this->fl.l_type = F_RDLCK;
    return fcntl(this->fd, F_SETLKW, &(this->fl));
}

int LockFile::tomarLockEscritura() {
    this->fl.l_type = F_WRLCK;
    return fcntl(this->fd, F_SETLKW, &(this->fl));
}

int LockFile::liberarLock() {
    this->fl.l_type = F_UNLCK;
    return fcntl(this->fd, F_SETLK, &(this->fl));
}

ssize_t LockFile::escribir(const void *buffer, const ssize_t buffsize) const {
    lseek(this->fd, 0, SEEK_END);
    return write(this->fd, buffer, buffsize);
}

void LockFile::setInit() {
    lseek(this->fd, 0, SEEK_SET);
}

ssize_t LockFile::leer(void *buffer, const ssize_t buffsize) const {
    return read(this->fd, buffer, buffsize);
}

int LockFile::leerLinea(char **line, const ssize_t buffsize) {
    char c;
    ssize_t red;
    size_t l = 0;
    size_t s = 0;
    *line = NULL;
    while ((red = read(this->fd, &c, sizeof(char))) > 0) {
        if (s >= l) {
            if (l < buffsize) l += buffsize;
            else l *= 2;
            *line = (char *) realloc(*line, (l + 1) * sizeof(char));
        }
        if (c == '\n') {
            (*line)[s] = '\0';
            return s;
        }
        (*line)[s++] = c;
    }
    if (*line) {
        (*line)[s] = '\0';
        return s;
    }
    return red;
}


std::string LockFile::nombreArchivo() const {
    return this->nombre;
}

LockFile::~LockFile() {
    close(this->fd);
}
