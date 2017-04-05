#ifndef _SELECT_H_
#define _SELECT_H_

#include "date.h"
#include "phone.h"


class SELECT {
public:
	SELECT();
	SELECT(Phone phone, int service, Date dateMin, Date dateMax);
	Phone GetPhone() const;
	int GetService() const;
	Date GetMinDate() const;
	Date GetMaxDate() const;
private:
	int _service;
	Date _dateMin;
	Date _dateMax;
	Phone _phone;
};


#endif
