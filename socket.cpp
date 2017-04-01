#include "socket.h"
#include <iostream>
#include <stdexcept>


/*
class Socket {
public:
	Socket(); // констурктор
	void CreateClient(); // создание сокета для клиента
	void Connect(int port, const char *host);
	void CreateServer(); // для сервера
	void Bind(int port, const char *host); // связывание сокета с адресом
	void Listen(); // установка на прослушивание
	void Accept(Socket &newSock) const; //ожидание запроса на соединение
	ssize_t WriteAll(const void *message, size_t length); // отправить message
	ssize_t ReadAll(void *buf, size_t length); // читать из сокета в buf
	bool IsWork() const; // валиден ли сокет?
	void Close(); // закрытие сокета
	~Socket(); // декоснтруктор(закрытие соединения)
private:
	int sock_; //дескриптор сокета
	sockaddr_in addr_; // адрес
};

*/

Socket::Socket() {
	sock_ = -1;
	memset(&addr_, 0, sizeof(addr_));
}

void Socket::CreateClient() {
	if ((sock_ = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw std::invalid_argument("Can`t create client sock");
}

void Socket::Connect(int port, const char *host) {
	if (!IsWork())
		throw std::invalid_argument("Client sock didn`t created");
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	if (inet_pton(AF_INET, host, &addr_.sin_addr) < 1)
		throw std::invalid_argument("Can`t inet_pton (connect)");
	if (connect(sock_, (struct sockaddr *)&addr_, sizeof(addr_)) == -1)
		throw std::invalid_argument("Can`t connect");
}

void Socket::CreateServer() {
	if ((sock_ = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw std::invalid_argument("Can`t create server sock");
}

void Socket::Bind(int port, const char *host) {
	if (!IsWork())
		throw std::invalid_argument("Sock didn`t created (bind)");
	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port);
	if (inet_pton(AF_INET, host, &addr_.sin_addr) < 1)
		throw std::invalid_argument("Can`t inet_pton");
	int on = 1;
	if (setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
		throw std::invalid_argument("Can`t setsockopt");
	if (bind(sock_, (struct sockaddr *)&addr_, sizeof(addr_)) == -1)
		throw std::invalid_argument("Can`t bind");
}

void Socket::Listen() {
	if (!IsWork())
		throw std::invalid_argument("Sock didn`t created (listen)");
	if (listen(sock_, 5) == -1)
		throw std::invalid_argument("Can`t listen");
}

void Socket::Accept(Socket &newSock) const {
	if (!IsWork())
		throw std::invalid_argument("Sock didn`t created (accept)");
	socklen_t addrlen = sizeof(addr_);
	if ((newSock.sock_ = accept(sock_, (struct sockaddr *)&addr_, &addrlen)) == -1)
		throw std::invalid_argument("Can`t accept");
}

ssize_t Socket::WriteAll(const void *message, size_t length) {
	if (!IsWork())
		throw std::invalid_argument("Sock didn`t created");
	const char *buf = (const char*)message;
	size_t res, alreadyWritten;
	for (alreadyWritten = 0; length; buf += res, alreadyWritten += res, length -= res)
        if ((res = write(sock_, buf, length)) <= 0)
            break;
	return alreadyWritten ? alreadyWritten : res;
}

ssize_t Socket::ReadAll(void *buf, size_t length) {
	if (!IsWork())
		throw std::invalid_argument("Sock didn`t created");
	char *message = (char *)buf;
	size_t res, alreadyRead;
	for (alreadyRead = 0; length; message += res, alreadyRead += res, length -= res)
        if ((res = read(sock_, message, length)) <= 0)
            break;
	return alreadyRead ? alreadyRead : res;
}

bool Socket::IsWork() const {
	return sock_ > 0;
}

void Socket::Close() {
	if (IsWork())
		if (shutdown(sock_, 2) == -1)
            throw std::invalid_argument("Can`t shutdown");
		if (close(sock_) == -1)
			throw std::invalid_argument("Can`t close sock");
}

Socket::~Socket() {
	Close();
}
