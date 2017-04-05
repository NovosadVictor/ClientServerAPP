#include "select.h"

#include <stdexcept>


SELECT::SELECT() {
	_phone = Phone();
	_service = 0;
	_dateMin = Date();
	_dateMax = Date(2100, 12, 30, 23, 59, 59);
}

SELECT::SELECT(Phone phone, int service, Date dateMin, Date dateMax) {
	_phone = phone;
	_service = service;
	_dateMin = dateMin;
	_dateMax = dateMax;
}

Phone SELECT::GetPhone() const {
	return  _phone;
}

int SELECT::GetService() const {
	return _service;
}

Date SELECT::GetMinDate() const {
	return _dateMin;
}

Date SELECT::GetMaxDate() const {
        return _dateMax;
}
 
