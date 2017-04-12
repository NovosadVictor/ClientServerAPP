#include "delete.h"

/*
class DELETE {
public:
    DELETE();
    DELETE(int service);
    DELETE(Phone phone);
    DELETE(Date dateMin, Date dateMax);
    DELETE(int service, Phone phone);
    DELETE(int service, Phone phone, Date dateMin, Date dateMax);
    int GetService() const;
    Date GetMinDate() const;
    Date GetMaxDate() const;
    Phone GetPhone();
private:
    Date _dateMin;
    Date _dateMax;
    Phone _phone;
    int _service;
};
*/

DELETE::DELETE() {
    _service = 0;
    _dateMin = Date();
    _dateMax = Date(2100, 12, 30, 23, 59, 59);
    _phone = Phone("7777777777");
}

DELETE::DELETE(int service, Phone phone, Date dateMin, Date dateMax) {
    _service = service;
    _phone = phone;
    _dateMin = _dateMin;
    _dateMax = dateMax;
}

int DELETE::GetService() const {
    return _service;
}

Date DELETE::GetMinDate() const {
    return _dateMin;
}

Date DELETE::GetMaxDate() const {
    return _dateMax;
}

Phone DELETE::GetPhone() {
    return _phone;
}