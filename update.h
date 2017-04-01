#ifndef _UPDATE_H
#define _UPDATE_H

#include "date.h"
#include "phone.h"


class UPDATE {
public:
    UPDATE();
    UPDATE(int service, Phone phone, int serviceFilter, Phone phoneFilter);
    int GetService() const;
    Phone GetPhone() const;
    int GetFilterService() const;
    Phone GetFilterPhone() const;
private:
    int _service;
    Phone _phone;
    int _serviceFilter;
    Phone _phoneFilter;
};

#endif