#include "update.h"


UPDATE::UPDATE() {
    _service = 0;
    _phone = Phone();
}

UPDATE::UPDATE(int service,
               Phone phone,
               int serviceFilter,
               Phone phoneFilter,
               Date dateMinFilter,
               Date dateMaxFilter) {
    _service = service;
    _phone = phone;
    _serviceFilter = serviceFilter;
    _phoneFilter = phoneFilter;
    _dateMinFilter = dateMinFilter;
    _dateMaxFilter = dateMaxFilter;
}

int UPDATE::GetService() const {
    return _service;
}

Phone UPDATE::GetPhone() const {
    return _phone;
}

int UPDATE::GetFilterService() const {
    return _serviceFilter;
}

Phone UPDATE::GetFilterPhone() const {
    return _phoneFilter;
}

Date UPDATE::GetFilterDateMin() const {
    return _dateMinFilter;
}

Date UPDATE::GetFilterDateMax() const {
    return _dateMaxFilter;
}