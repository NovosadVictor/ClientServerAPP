#ifndef _UPDATE_H
#define _UPDATE_H

#include "date.h"
#include "phone.h"


class UPDATE {
public:
    UPDATE();
    UPDATE(int service,
           Phone phone,
           int serviceFilter,
           Phone phoneFilter,
           Date dateMinFilter,
           Date dateMaxFilter
    );
    int GetService() const;
    Phone GetPhone() const;
    int GetFilterService() const;
    Phone GetFilterPhone() const;
    Date GetFilterDateMin() const;
    Date GetFilterDateMax() const;
private:
    int _service;
    Phone _phone;
    int _serviceFilter;
    Phone _phoneFilter;
    Date _dateMinFilter;
    Date _dateMaxFilter;
};

#endif