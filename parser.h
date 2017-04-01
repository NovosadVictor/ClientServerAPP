#ifndef _PARSER_H_
#define _PARSER_H_

#include "billing.h"
#include "select.h"
#include "insert.h"
#include "delete.h"
#include "update.h"

#include <vector>


class Parser {
public:
	Parser();
	Parser(int &fd);
	void ParseRequest(const char *request, int &fd);
	int GetType() const;
    void GetSelect(SELECT select, int flag);
	void GetInsert(INSERT insert, int flag);
	void GetDelete(DELETE delete_, int flag);
    void GetUpdate(UPDATE update, int flag);
	void GetSave(int &fd);
    std::vector<Billing> GetResponse() const;
private:
    int _type;
    std::vector<Billing> _AllVector;
    std::vector<Billing> _responseVector;
};


#endif
