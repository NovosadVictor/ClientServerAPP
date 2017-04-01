#ifndef _SELECT_H_
#define _SELECT_H_

#include "date.h"


class SELECT {
public:
	SELECT();
	SELECT(int service, Date dateMin, Date dateMax);
	int GetService() const;
	Date GetMinDate() const;
	Date GetMaxDate() const;
private:
	int _service;
	Date _dateMin;
	Date _dateMax;
};


#endif
