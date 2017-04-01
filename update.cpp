#include "update.h"


UPDATE::UPDATE() {
    _service = 0;
    _phone = Phone();
}

UPDATE::UPDATE(int service, Phone phone, int serviceFilter, Phone phoneFilter) {
    _service = service;
    _phone = phone;
    _serviceFilter = serviceFilter;
    _phoneFilter = phoneFilter;
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
