#ifndef _SOCKET_H_
#define _SOCKET_H

#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


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






#endif
