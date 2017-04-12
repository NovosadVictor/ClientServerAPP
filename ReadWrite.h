#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


ssize_t WriteAll(int fd, const void *message, size_t length) {
    const char *buf = (const char*)message;
    size_t res, alreadyWritten;
    for (alreadyWritten = 0; length; buf += res, alreadyWritten += res, length -= res)
        if ((res = write(fd, buf, length)) <= 0)
            break;
    return alreadyWritten ? alreadyWritten : res;
}

ssize_t ReadAll(int fd, void *buf, size_t length) {
    char *message = (char *)buf;
    size_t res, alreadyRead;
    for (alreadyRead = 0; length; message += res, alreadyRead += res, length -= res)
        if ((res = read(fd, message, length)) <= 0)
            break;
    return alreadyRead ? alreadyRead : res;
}
