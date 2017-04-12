#include <iostream>
#include <fcntl.h>
#include <cinttypes>
#include <csignal>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <vector>

#include "readWrite.h"
#include "parser.h"


static int signalIgnoring();
static void loop(int sd, int fd);

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "You should write port and host" << std::endl;
        return -1;
    }

    int fd = open("database.db", O_RDONLY);
    if (fd == -1) {
        std::cout << "Cant open file database.db" << std::endl;
        return -1;
    }

    char *host = new char[strlen(argv[2]) + 1];
    memcpy(host, argv[2], strlen(argv[2]));
    host[strlen(argv[2])] = '\0';
    struct sockaddr_in addr;
    int sd; // Socket Descriptor
    int port = atoi(argv[1]);

    if(signalIgnoring() != 0)
        return -1;

    memset(&addr, 0, sizeof addr);    
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    if(inet_pton(AF_INET, host, &addr.sin_addr) < 1) {
        fprintf(stderr, "Wrong host\n");
        return -1;
    }

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd == -1) {
        fprintf(stderr, "Can't create socket\n");
        return -1;
    }

    int on = 1;
    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on) == -1) {
        fprintf(stderr, "Can't setsockopt\n");
        if(close(sd))
            fprintf(stderr, "Can't release file descriptor\n");
        return -1;
    }

    if(bind(sd, (sockaddr *)&addr, sizeof addr) == -1) {
        fprintf(stderr, "Can't bind\n"); 
        if(close(sd))
            fprintf(stderr, "Can't release file descriptor\n");
        return -1;
    }

    if(listen(sd, 5) == -1) {
        fprintf(stderr, "Can't listen\n");
        if(close(sd))
            fprintf(stderr, "Can't release file descriptor\n");
        return -1;
    }

    try {
        loop(sd, fd);
    }
    catch (const char *err) {
        fprintf(stderr, "%s\n", err);
    }
    catch (std::exception &err) {
        fprintf(stderr, "Exception:\n   %s\n", err.what());
    }

    if(close(sd)) {
        fprintf(stderr, "Can't release file descriptor\n");
        return -1;
    }

    puts("Done.");

    return 0;
}

static void handler(int signo);

static int signalIgnoring() {
    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags   = 0;
    sigemptyset(&act.sa_mask);

    if(sigaction(SIGINT, &act, 0) == -1) {
        fprintf(stderr, "Can't setup SIGINT ignoring\n");
        return -1;
    }

    if(sigaction(SIGPIPE, &act, 0) == -1) {
        fprintf(stderr, "Can't setup SIGPIPE ignoring\n");
        return -1;
    }

    return 0;
}

static void handler(int signo) {
    (void)signo;
}

static void loop(int sd, int fd) {
    for (;;) {
        sockaddr_in addr;
        socklen_t addrlen;

        memset(&addr, 0, sizeof addr);
        addrlen = sizeof addr;

        int nsd = accept(sd, (struct sockaddr *) &addr, &addrlen);
        if (nsd == -1) {
            if (errno == EINTR)
                return;
            else
                throw "Can't accept";
        }
        Parser response(fd);
        for (;;) {
            try {
                size_t length;
                if (readAll(nsd, &length, sizeof(length)) != sizeof(length))
                    throw 7;//std::invalid_argument("Can`t read length");
                std::vector<char> buf(length);
                memset(&buf[0], 0, length);
                if (readAll(nsd, &buf[0], length) != (ssize_t) length)
                    throw 7;//std::invalid_argument("Can`t read message");
                char *request = new char[length];
                memcpy(request, &buf[0], length - 1);
                request[length - 1] = '\0';
                response.ParseRequest(request, fd);
                char firstAnswer[22];
                snprintf(firstAnswer, 22, "Server:\n   status = %d", response.GetType());
                firstAnswer[21] = '\0';
                size_t len = 22;
                if (writeAll(nsd, &len, sizeof(len)) != sizeof(len))
                    throw 7;//std::invalid_argument("Cant write length");
                if (writeAll(nsd, firstAnswer, len) != (ssize_t) len)
                    throw 7;//std::invalid_argument("Cant write firstAnswer");
                if (response.GetType() == 7) {
                    char successAnswer[24];
                    snprintf(successAnswer, 24, "\n    All changes saved\n");
                    successAnswer[23] = '\0';
                    len = 24;
                    if (writeAll(nsd, &len, sizeof(len)) != sizeof(len))
                        throw 7;//std::invalid_argument("Cant write length");
                    if (writeAll(nsd, successAnswer, len) != (ssize_t) len)
                        throw 7;//std::invalid_argument("Cant write successAnswer");
                }
                if (response.GetType() == 1) {
                    char selectAnswer[24];
                    snprintf(selectAnswer, 24, "    quantity = %07lu\n", (response.GetResponse()).size());
                    selectAnswer[23] = '\0';
                    len = 24;
                    if (writeAll(nsd, &len, sizeof(len)) != sizeof(len))
                        throw 7;//std::invalid_argument("Cant write length");
                    if (writeAll(nsd, selectAnswer, len) != (ssize_t) len)
                        throw 7;//std::invalid_argument("Cant write selectAnswer");
                    for (size_t i = 0; i < (response.GetResponse()).size(); ++i) {
                        char selectBillings[50];
                        snprintf(selectBillings, 50, "    %s %d %s %08f\n",
                                 (response.GetResponse()[i]).GetPhone().GetNumber(),
                                 (response.GetResponse()[i]).GetService(),
                                 (response.GetResponse()[i]).GetDate().GetDate(),
                                 (response.GetResponse()[i]).GetSum()
                        );
                        selectBillings[49] = '\0';
                        len = 50;
                        if (writeAll(nsd, &len, sizeof(len)) != sizeof(len))
                            throw 7;//std::invalid_argument("Cant write length");
                        if (writeAll(nsd, selectBillings, len) != (ssize_t) len)
                            throw 7;//std::invalid_argument("Cant write selectBillings");
                    }
                }
                if (response.GetType() == 2) {
                    char successAnswer[30];
                    snprintf(successAnswer, 30, "\n    User successfully added\n");
                    successAnswer[29] = '\0';
                    len = 30;
                    if (writeAll(nsd, &len, sizeof(len)) != sizeof(len))
                        throw 7;//std::invalid_argument("Cant write length");
                    if (writeAll(nsd, successAnswer, len) != (ssize_t) len)
                        throw 7;//std::invalid_argument("Cant write successAnswer");
                }
                if (response.GetType() == 3) {
                    char successAnswer[35];
                    snprintf(successAnswer, 35, "\n    User(s) successfully deleted\n");
                    successAnswer[34] = '\0';
                    len = 35;
                    if (writeAll(nsd, &len, sizeof(len)) != sizeof(len))
                        throw 7;//std::invalid_argument("Cant write length");
                    if (writeAll(nsd, successAnswer, len) != (ssize_t) len)
                        throw 7;//std::invalid_argument("Cant write successAnswer");
                }
                if (response.GetType() == 4) {
                    char successAnswer[35];
                    snprintf(successAnswer, 35, "\n    User(s) successfully updated\n");
                    successAnswer[34] = '\0';
                    len = 35;
                    if (writeAll(nsd, &len, sizeof(len)) != sizeof(len))
                        throw 7;//std::invalid_argument("Cant write length");
                    if (writeAll(nsd, successAnswer, len) != (ssize_t) len)
                        throw 7;//std::invalid_argument("Cant write successAnswer");
                }
                delete request;
            }
            catch (int err) {
                if (err != 7) {
                    char errorMessage[63];
                    snprintf(errorMessage, 63, "Server:\n   status = -1\n    It was wrong request, try again...\n");
                    errorMessage[62] = '\0';
                    size_t len = 63;
                    if (writeAll(nsd, &len, sizeof(len)) != sizeof(len))
                        throw std::invalid_argument("Cant write length");
                    if (writeAll(nsd, errorMessage, len) != (ssize_t) len)
                        throw std::invalid_argument("Cant write errorMessage");
                } else
                    break;
            }
        }
//        if (nsd != -1) {
        if (shutdown(nsd, 2) == -1)
            fprintf(stderr, "Can't shutdown socket\n");

        if (close(nsd))
            fprintf(stderr, "Can't release file descriptor\n");
//    }
    }
}

