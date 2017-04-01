#include "socket.h"
#include "parser.h"


static void loop(int fd, const Socket &server);


int main(int argc, char **argv) {
	if (argc != 3) {
		std::cout << "You should write port and host..." << std::endl;
		return -1;
	}
	int fd = open("database.db", O_RDONLY);
	if (fd == -1) {
		std::cout << "Error, cant open file" << std::endl;
		return -1;
	}
	int port = atoi(argv[1]);
	char *host = new char[strlen(argv[2] + 1)];
	memcpy(host, argv[2], strlen(argv[2]));
	host[strlen(argv[2])] = '\0';
	try {
		Socket server;
		server.CreateServer();
		server.Bind(port, host);
		server.Listen();
		loop(fd, server);
	}
	catch (std::exception &err) {
		std::cout << "Exception" << err.what() << std::endl;
        return -1;
	}
	delete host;
	return 0;	
}

static void loop(int fd, const Socket &server) {

	for (;;) {
		Socket newSock;
		server.Accept(newSock);
		std::cout << "Someone connected..." << std::endl;
		Parser response(fd);
		for (;;) {
			try {
				size_t length;
				if (newSock.ReadAll(&length, sizeof(length)) != sizeof(length))
					throw std::invalid_argument("Can`t read length");
                std::vector<char> buf(length);
                memset(&buf[0], 0, length);
				if (newSock.ReadAll(&buf[0], length) != length)
					throw std::invalid_argument("Can`t read message");
                char *request = new char[length];
                memcpy(request, &buf[0], length - 1);
                request[length - 1] = '\0';
				response.ParseRequest(request, fd);
				char firstAnswer[22];
				snprintf(firstAnswer, 22, "Server:\n   status = %d", response.GetType());
				firstAnswer[21] = '\0';
                size_t len = 22;
                if (newSock.WriteAll(&len, sizeof(len)) != sizeof(len))
                    throw std::invalid_argument("Cant write length");
				if (newSock.WriteAll(firstAnswer, len) != len)
                    throw std::invalid_argument("Cant write firstAnswer");
				if (response.GetType() == 7) {
					char successAnswer[24];
					snprintf(successAnswer, 24, "\n    All changes saved\n");
					successAnswer[23] = '\0';
					len = 24;
					if (newSock.WriteAll(&len, sizeof(len)) != sizeof(len))
						throw std::invalid_argument("Cant write length");
					if (newSock.WriteAll(successAnswer, len) != len)
						throw std::invalid_argument("Cant write successAnswer");
				}
				if (response.GetType() == 1) {
					char selectAnswer[24];
					snprintf(selectAnswer, 24, "    quantity = %07lu\n", (response.GetResponse()).size());
					selectAnswer[23] = '\0';
					len = 24;
					if (newSock.WriteAll(&len, sizeof(len)) != sizeof(len))
						throw std::invalid_argument("Cant write length");
					if (newSock.WriteAll(selectAnswer, len) != len)
						throw std::invalid_argument("Cant write selectAnswer");
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
						if (newSock.WriteAll(&len, sizeof(len)) != sizeof(len))
							throw std::invalid_argument("Cant write length");
						if (newSock.WriteAll(selectBillings, len) != len)
							throw std::invalid_argument("Cant write selectBillings");
					}
				}
				if (response.GetType() == 2) {
					char successAnswer[30];
					snprintf(successAnswer, 30, "\n    User successfully added\n");
					successAnswer[29] = '\0';
                    len = 30;
                    if (newSock.WriteAll(&len, sizeof(len)) != sizeof(len))
                        throw std::invalid_argument("Cant write length");
                    if (newSock.WriteAll(successAnswer, len) != len)
                        throw std::invalid_argument("Cant write successAnswer");
				}
                if (response.GetType() == 3) {
                    char successAnswer[35];
                    snprintf(successAnswer, 35, "\n    User(s) successfully deleted\n");
                    successAnswer[34] = '\0';
                    len = 35;
                    if (newSock.WriteAll(&len, sizeof(len)) != sizeof(len))
                        throw std::invalid_argument("Cant write length");
                    if (newSock.WriteAll(successAnswer, len) != len)
                        throw std::invalid_argument("Cant write successAnswer");
                }
                if (response.GetType() == 4) {
                    char successAnswer[35];
                    snprintf(successAnswer, 35, "\n    User(s) successfully updated\n");
                    successAnswer[34] = '\0';
                    len = 35;
                    if (newSock.WriteAll(&len, sizeof(len)) != sizeof(len))
                        throw std::invalid_argument("Cant write length");
                    if (newSock.WriteAll(successAnswer, len) != len)
                        throw std::invalid_argument("Cant write successAnswer");
                }
                delete request;
			}
			catch (std::exception &err) {
				char errorMessage[63];
				snprintf(errorMessage, 63, "Server:\n   status = -1\n    It was wrong request, try again...\n");
				errorMessage[62] = '\0';
                size_t len = 63;
                if (newSock.WriteAll(&len, sizeof(len)) != sizeof(len))
                    throw std::invalid_argument("Cant write length");
                if (newSock.WriteAll(errorMessage, len) != len)
                    throw std::invalid_argument("Cant write errorMessage");
			}
		}
	}
}

























