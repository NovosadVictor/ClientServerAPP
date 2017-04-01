#ifndef _DELETE_H
#define _DELETE_H

#include "date.h"
#include "phone.h"


class DELETE {
public:
    DELETE();
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

#endif
