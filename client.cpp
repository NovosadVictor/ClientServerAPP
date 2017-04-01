#include <iostream>
#include <stdexcept>
#include <vector>
#include "socket.h"


int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "You should write port and host..." << std::endl;
        return -1;
    }
    int port = atoi(argv[1]);
    char *host = new char[strlen(argv[2] + 1)];
    memcpy(host, argv[2], strlen(argv[2]));
    host[strlen(argv[2])] = '\0';
    try {
        Socket client;
        client.CreateClient();
        client.Connect(port, host);
        char message[1024];
        for (;;) {
            std::cout << "You can request\n>>>> ";
            fgets(message, 1024, stdin);
            size_t len = strlen(message);
            message[len - 1] = '\0';
            if (client.WriteAll(&len, sizeof(len)) != sizeof(len))
                throw std::invalid_argument("Cant write length");
            if (client.WriteAll(message, len) != len)
                throw std::invalid_argument("Cant write message");
            int status;
            int quantity;
            for (size_t i = 0; i < 2; ++i) {
                size_t length;
                if (client.ReadAll(&length, sizeof(length)) != sizeof(length))
                    throw std::invalid_argument("Can`t read length");
                std::vector<char> response(length);
                memset(&response[0], 0, length);
                if (client.ReadAll(&response[0], length) != length)
                    throw std::invalid_argument("Can`t read message");
                if (i == 0)
                    status = atoi(&response[20]);
                for (size_t i = 0; i < length; ++i)
                    std::cout << response[i];
                std::cout << std::endl;
                if (status == -1)
                    break;
                if (i == 1 && status == 1)
                    sscanf(&response[4], "quantity = %d", &quantity);
            }
            if (status == 1)  {
                for (size_t i = 0; i < quantity; ++i) {
                    size_t length;
                    if (client.ReadAll(&length, sizeof(length)) != sizeof(length))
                        throw std::invalid_argument("Can`t read length");
                    std::vector<char> response(length);
                    memset(&response[0], 0, length);
                    if (client.ReadAll(&response[0], length) != length)
                        throw std::invalid_argument("Can`t read message");
                    for (size_t i = 0; i < length; ++i)
                        std::cout << response[i];
                    std::cout << std::endl;
                }
            }
        }

    }
    catch (std::exception &err) {
        std::cout << "Exception: " << err.what() << std::endl;
        return -1;
    }
    delete host;
    return 0;
}
