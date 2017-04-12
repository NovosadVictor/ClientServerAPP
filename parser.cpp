#include "parser.h"

#include <ctime>


std::vector<char> DeleteSpaces(const char *request) {
    std::vector<char> v;
    v.clear();
	for (size_t i = 0; i < strlen(request); ++i) {
        if (request[i] != ' ')
            v.push_back(request[i]);
    }
    return v;
}

Parser::Parser() {
	_type = 0;
}

Parser::Parser(int &fd) {
    lseek(fd, 0, SEEK_SET);
    _responseVector.clear();
    _AllVector.clear();
    for (;;) {
        Billing billing;
        int re = billing.ReadBin(fd);
        if (re == 0)
            break;
        if (re == -1)
            throw std::invalid_argument("Error in read");
        _AllVector.push_back(billing);
    }
    _type = 0;
}

void Parser::ParseRequest(const char *request, int &fd) {
    std::vector<char> v = DeleteSpaces(request);
    int service;
    char number[14];
    int yearMin, monthMin, dayMin, yearMax, monthMax, dayMax;
    int x;
    // SAVE
    sscanf(&v[0], "SAVE%n", &x);
    if (x == 4) {
        _type = 7;
        GetSave(fd);
        return;
    }
    // SELECTS
    if (sscanf(&v[0], "SELECTphone=[%13s]service=[%1d]period=[%4d-%02d-%02d,%4d-%02d-%02d]",
               number,
               &service,
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 8) {
        _type = 1;
        number[13] = '\0';
        GetSelect(SELECT(Phone(number),
                         service, Date(yearMin, monthMin, dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 1);
        return;
    }
    if (sscanf(&v[0], "SELECTphone=[%13s]service=[%1d]", number, &service) == 2) {
        _type = 1;
        number[13] = '\0';
        GetSelect(SELECT(Phone(number), service, Date(), Date()), 2);
        return;
    }
    if (sscanf(&v[0], "SELECTphone=[%13s]period=[%4d-%02d-%02d,%4d-%02d-%02d]",
               number,
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 7) {
        _type = 1;
        number[13] = '\0';
        GetSelect(SELECT(Phone(number),
                         0, Date(yearMin, monthMin, dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 3);
        return;
    }
    if (sscanf(&v[0], "SELECTservice=[%1d]period=[%4d-%02d-%02d,%4d-%02d-%02d]",
               &service,
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 7) {
        _type = 1;
        GetSelect(SELECT(Phone(),
                         service, Date(yearMin, monthMin, dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 4);
        return;
    }
    if (sscanf(&v[0], "SELECTphone=[%13s]", number) == 1) {
        number[13] = '\0';
        _type = 1;
        GetSelect(SELECT(Phone(number), 0, Date(), Date()), 5);
        return;
    }
    if (sscanf(&v[0], "SELECTservice=[%1d]", &service) == 1) {
        _type = 1;
        GetSelect(SELECT(Phone(), service, Date(), Date()), 6);
        return;
    }
    if (sscanf(&v[0], "SELECTperiod=[%4d-%02d-%02d,%4d-%02d-%02d]",
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 6) {
        _type = 1;
        GetSelect(SELECT(Phone(),
                         0,
                         Date(yearMin, monthMin, dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 7);
        return;
    }
    sscanf(&v[0], "SELECTALL%n", &x);
    if (x == 9) {
        _type = 1;
        GetSelect(SELECT(Phone(), 0, Date(), Date(2100, 12, 30, 23, 59, 59)), 8);
        return;
    }
    // END OF SELECTS
    // INSERT
    int year, month, day, hour, minute, second;
    double sum;
    if (sscanf(&v[0], "INSERTphone=[%13s]service=[%1d]sum=[%lf]", number, &service, &sum) == 3) {
        number[13] = '\0';
        _type = 2;
        time_t now = time(NULL);
        tm *timeInfo = localtime(&now);
        Date date(1900 + timeInfo->tm_year,
                  1 + timeInfo->tm_mon,
                  timeInfo->tm_mday,
                  timeInfo->tm_hour,
                  timeInfo->tm_min,
                  timeInfo->tm_sec
        );
        GetInsert(INSERT(Phone(number), service, Date(date), sum));
        return;
    }
    if (sscanf(&v[0], "INSERTphone=[%13s]service=[%1d]date=[%4d-%02d-%02d:%02d:%02d:%02d]sum=[%lf]",
                   number,
                   &service,
                   &year,
                   &month,
                   &day,
                   &hour,
                   &minute,
                   &second,
                   &sum) == 9) {
            number[13] = '\0';
            _type = 2;
            GetInsert(INSERT(Phone(number), service, Date(year, month, day, hour, minute, second), sum));
            return;
    }
    // END OF INSERT
    // DELETES
    if (sscanf(&v[0], "DELETEphone=[%13s]service=[%1d]period=[%4d-%02d-%02d,%4d-%02d-%02d]",
               number,
               &service,
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 8) {
        number[13] = '\0';
        _type = 3;
        GetDelete(DELETE(service, Phone(number), Date(yearMin, monthMin, dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 1);
        return;
    }
    if (sscanf(&v[0], "DELETEphone=[%13s]service=[%1d]", number, &service) == 2) {
        number[13] = '\0';
        _type = 3;
        GetDelete(DELETE(service, Phone(number), Date(), Date()), 2);
        return;
    }
    if (sscanf(&v[0], "DELETEphone=[%13s]period=[%4d-%02d-%02d,%4d-%02d-%02d]",
               number,
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 7) {
        number[13] = '\0';
        _type = 3;
        GetDelete(DELETE(0, Phone(number), Date(yearMin, monthMin, dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 3);
        return;
    }
    if (sscanf(&v[0], "DELETEservice=[%1d]period=[%4d-%02d-%02d,%4d-%02d-%02d]",
               &service,
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 7) {
        number[13] = '\0';
        _type = 3;
        GetDelete(DELETE(service, Phone(), Date(yearMin, monthMin, dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 4);
        return;
    }
    if (sscanf(&v[0], "DELETEphone=[%13s]", number) == 1) {
        number[13] = '\0';
        _type = 3;
        GetDelete(DELETE(0, Phone(number), Date(), Date(2100, 12, 30, 23, 59, 59)), 5);
        return;
    }
    if (sscanf(&v[0], "DELETEservice=[%1d]", &service) == 1) {
        _type = 3;
        GetDelete(DELETE(service, Phone(), Date(), Date(2100, 12, 30, 23, 59, 59)), 6);
        return;
    }
    if (sscanf(&v[0], "DELETEperiod=[%4d-%02d-%02d,%4d-%02d-%02d]",
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 6) {
        _type = 3;
        GetDelete(DELETE(0, Phone(), Date(yearMin, monthMin, dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 7);
        return;
    }
    sscanf(&v[0], "DELETEALL%n", &x);
    if (x == 9) {
        _type = 3;
        GetDelete(DELETE(0, Phone(), Date(), Date(2100, 12, 30, 23, 59, 59)), 8);
        return;
    }
    // END DELETES
    // UPDATES
    char numberFilter[14];
    int serviceFilter;
    if (sscanf(&v[0], "UPDATEphone=[%13s]WHEREphone=[%13s]service=[%1d]period=[%4d-%02d-%02d,%4d-%02d-%02d]",
               number,
               numberFilter,
               &serviceFilter,
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 9) {
        _type = 4;
        number[13] = '\0';
        numberFilter[13] = '\0';
        GetUpdate(UPDATE(0,
                         Phone(number),
                         serviceFilter,
                         Phone(numberFilter),
                         Date(yearMin, monthMin,dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 1);
        return;
    }
    if (sscanf(&v[0], "UPDATEphone=[%13s]WHEREphone=[%13s]service=[%1d]",
               number,
               numberFilter,
               &serviceFilter) == 3) {
        _type = 4;
        number[13] = '\0';
        numberFilter[13] = '\0';
        GetUpdate(UPDATE(0,
                         Phone(number),
                         serviceFilter,
                         Phone(numberFilter),
                         Date(), Date()), 2);
        return;
    }
    if (sscanf(&v[0], "UPDATEphone=[%13s]WHEREphone=[%13s]period=[%4d-%02d-%02d,%4d-%02d-%02d]",
               number,
               numberFilter,
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 8) {
        _type = 4;
        number[13] = '\0';
        numberFilter[13] = '\0';
        GetUpdate(UPDATE(0,
                         Phone(number),
                         0,
                         Phone(numberFilter),
                         Date(yearMin, monthMin,dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 3);
        return;
    }
    if (sscanf(&v[0], "UPDATEphone=[%13s]WHEREservice=[%1d]period=[%4d-%02d-%02d,%4d-%02d-%02d]",
               number,
               &serviceFilter,
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 8) {
        _type = 4;
        number[13] = '\0';
        GetUpdate(UPDATE(0,
                         Phone(number),
                         serviceFilter,
                         Phone(),
                         Date(yearMin, monthMin,dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 4);
        return;
    }
    if (sscanf(&v[0], "UPDATEphone=[%13s]WHEREphone=[%13s]", number, numberFilter) == 2) {
        _type = 4;
        number[13] = '\0';
        numberFilter[13] = '\0';
        GetUpdate(UPDATE(0, Phone(number), 0, Phone(numberFilter), Date(), Date()), 5);
        return;
    }
    if (sscanf(&v[0], "UPDATEphone=[%13s]WHEREservice=[%1d]", number, &serviceFilter) == 2) {
        _type = 4;
        number[13] = '\0';
        GetUpdate(UPDATE(0, Phone(number), serviceFilter, Phone(), Date(), Date()), 6);
        return;
    }
    if (sscanf(&v[0], "UPDATEphone=[%13s]WHEREperiod=[%4d-%02d-%02d,%4d-%02d-%02d]",
               number,
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 7) {
        _type = 4;
        number[13] = '\0';
        GetUpdate(UPDATE(0,
                         Phone(number),
                         0,
                         Phone(),
                         Date(yearMin, monthMin,dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 7);
        return;
    }
    if (sscanf(&v[0], "UPDATEservice=[%1d]WHEREphone=[%13s]service=[%1d]period=[%4d-%02d-%02d,%4d-%02d-%02d]",
               &service,
               numberFilter,
               &serviceFilter,
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 9) {
        _type = 4;
        numberFilter[13] = '\0';
        GetUpdate(UPDATE(service,
                         Phone(),
                         serviceFilter,
                         Phone(numberFilter),
                         Date(yearMin, monthMin,dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 8);
        return;
    }
    if (sscanf(&v[0], "UPDATEservice=[%1d]WHEREphone=[%13s]service=[%1d]",
               &service,
               numberFilter,
               &serviceFilter) == 3) {
        _type = 4;
        numberFilter[13] = '\0';
        GetUpdate(UPDATE(service,
                         Phone(),
                         serviceFilter,
                         Phone(numberFilter),
                         Date(), Date()), 9);
        return;
    }
    if (sscanf(&v[0], "UPDATEservice=[%1d]WHEREphone=[%13s]period=[%4d-%02d-%02d,%4d-%02d-%02d]",
               &service,
               numberFilter,
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 8) {
        _type = 4;
        numberFilter[13] = '\0';
        GetUpdate(UPDATE(service,
                         Phone(),
                         0,
                         Phone(numberFilter),
                         Date(yearMin, monthMin,dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 10);
        return;
    }
    if (sscanf(&v[0], "UPDATEservice=[%1d]WHEREservice=[%1d]period=[%4d-%02d-%02d,%4d-%02d-%02d]",
               &service,
               &serviceFilter,
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 8) {
        _type = 4;
        GetUpdate(UPDATE(service,
                         Phone(),
                         serviceFilter,
                         Phone(),
                         Date(yearMin, monthMin,dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 11);
        return;
    }
    if (sscanf(&v[0], "UPDATEservice=[%1d]WHEREphone=[%13s]", &service, numberFilter) == 2) {
        _type = 4;
        numberFilter[13] = '\0';
        GetUpdate(UPDATE(service, Phone(), 0, Phone(numberFilter), Date(), Date()), 12);
        return;
    }
    if (sscanf(&v[0], "UPDATEservice=[%1d]WHEREservice=[%d]", &service, &serviceFilter) == 2) {
        _type = 4;
        GetUpdate(UPDATE(service, Phone(), serviceFilter, Phone(), Date(), Date()), 13);
        return;
    }
    if (sscanf(&v[0], "UPDATEservice=[%1d]WHEREperiod=[%4d-%02d-%02d,%4d-%02d-%02d]",
               &service,
               &yearMin,
               &monthMin,
               &dayMin,
               &yearMax,
               &monthMax,
               &dayMax) == 7) {
        _type = 4;
        GetUpdate(UPDATE(service,
                         Phone(),
                         0,
                         Phone(),
                         Date(yearMin, monthMin,dayMin, 0, 0, 0),
                         Date(yearMax, monthMax, dayMax, 23, 59, 59)), 14);
        return;
    }
    if (sscanf(&v[0], "UPDATEphone=[%13s]", number) == 1) {
        _type = 4;
        number[13] = '\0';
        GetUpdate(UPDATE(0, Phone(number), 0, Phone(), Date(), Date()), 15);
        return;
    }
    if (sscanf(&v[0], "UPDATEservice=[%1d]", &service) == 1) {
        _type = 4;
        GetUpdate(UPDATE(service, Phone(), 0, Phone(), Date(), Date()), 16);
        return;
    }
    // END UPDATES

    // END REQUESTS
    throw 1;
}

void Parser::GetSelect(SELECT select, int flag) {
    _responseVector.clear();
	if (_type == 1) {
        if (flag == 8) {
            _responseVector = _AllVector;
            return;
        }
        if (flag == 1) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetPhone() == select.GetPhone() &&
                    _AllVector[i].GetService() == select.GetService() &&
                    _AllVector[i].GetDate() <= select.GetMaxDate() &&
                    _AllVector[i].GetDate() >= select.GetMinDate()
                        )
                    _responseVector.push_back(_AllVector[i]);
            return;
        }
        if (flag == 2) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetPhone() == select.GetPhone() &&
                    _AllVector[i].GetService() == select.GetService()
                        )
                    _responseVector.push_back(_AllVector[i]);
            return;
        }
        if (flag == 3) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetPhone() == select.GetPhone() &&
                    _AllVector[i].GetDate() <= select.GetMaxDate() &&
                    _AllVector[i].GetDate() >= select.GetMinDate()
                        )
                    _responseVector.push_back(_AllVector[i]);
            return;
        }
        if (flag == 4) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetService() == select.GetService() &&
                    _AllVector[i].GetDate() <= select.GetMaxDate() &&
                    _AllVector[i].GetDate() >= select.GetMinDate()
                        )
                    _responseVector.push_back(_AllVector[i]);
            return;
        }
        if (flag == 5) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetPhone() == select.GetPhone())
                    _responseVector.push_back(_AllVector[i]);
            return;
        }
        if (flag == 6) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetService() == select.GetService())
                    _responseVector.push_back(_AllVector[i]);
            return;
        }
        if (flag == 7) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetDate() >= select.GetMinDate() &&
                    _AllVector[i].GetDate() <= select.GetMaxDate()
                        )
                    _responseVector.push_back(_AllVector[i]);
            return;
        }
    }
	else
		throw std::invalid_argument("Its not select");
}

void Parser::GetInsert(INSERT insert) {
	if (_type == 2) {
        Billing billing(insert.GetPhone(), insert.GetService(), insert.GetDate(), insert.GetSum());
        _AllVector.push_back(billing);
        return;
    }
	else
		throw std::invalid_argument("Its not insert");
}

void Parser::GetDelete(DELETE delete_, int flag) {
    if (_type == 3) {
        if (flag == 8) {
            _AllVector.clear();
            return;
        }
        if (flag == 1) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetPhone() == delete_.GetPhone() &&
                    _AllVector[i].GetService() == delete_.GetService() &&
                    _AllVector[i].GetDate() <= delete_.GetMaxDate() &&
                    _AllVector[i].GetDate() >= delete_.GetMinDate()
                        ) {
                    _AllVector.erase(_AllVector.begin() + i);
                    --i;
                }
            return;
        }
        if (flag == 2) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetPhone() == delete_.GetPhone() &&
                    _AllVector[i].GetService() == delete_.GetService()
                        ) {
                    _AllVector.erase(_AllVector.begin() + i);
                    --i;
                }
            return;
        }
        if (flag == 3) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetPhone() == delete_.GetPhone() &&
                    _AllVector[i].GetDate() <= delete_.GetMaxDate() &&
                    _AllVector[i].GetDate() >= delete_.GetMinDate()
                        ) {
                    _AllVector.erase(_AllVector.begin() + i);
                    --i;
                }
            return;
        }
        if (flag == 4) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetService() == delete_.GetService() &&
                    _AllVector[i].GetDate() <= delete_.GetMaxDate() &&
                    _AllVector[i].GetDate() >= delete_.GetMinDate()
                        ) {
                    _AllVector.erase(_AllVector.begin() + i);
                    --i;
                }
            return;
        }
        if (flag == 5) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetPhone() == delete_.GetPhone()) {
                    _AllVector.erase(_AllVector.begin() + i);
                    --i;
                }
            return;
        }
        if (flag == 6) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetService() == delete_.GetService()) {
                    _AllVector.erase(_AllVector.begin() + i);
                    --i;
                }
            return;
        }
        if (flag == 7) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetDate() <= delete_.GetMaxDate() &&
                    _AllVector[i].GetDate() >= delete_.GetMinDate()
                        ) {
                    _AllVector.erase(_AllVector.begin() + i);
                    --i;
                }
            return;
        }
    }
    else
        throw std::invalid_argument("Its not delete");
}

void Parser::GetUpdate(UPDATE update, int flag) {
    if (_type == 4) {
        if (flag == 1) {
            Billing billing;
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetPhone() == update.GetFilterPhone() &&
                    _AllVector[i].GetService() == update.GetFilterService() &&
                    _AllVector[i].GetDate() >= update.GetFilterDateMin() &&
                    _AllVector[i].GetDate() <= update.GetFilterDateMax()
                        )
                    _AllVector[i].ChangePhone(update.GetPhone());
            return;
        }
        if (flag == 2) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetService() == update.GetFilterService() &&
                    _AllVector[i].GetPhone() == update.GetFilterPhone()
                        )
                    _AllVector[i].ChangePhone(update.GetPhone());
            return;
        }
        if (flag == 3) {
            Billing billing;
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetPhone() == update.GetFilterPhone() &&
                    _AllVector[i].GetDate() >= update.GetFilterDateMin() &&
                    _AllVector[i].GetDate() <= update.GetFilterDateMax()
                        )
                    _AllVector[i].ChangePhone(update.GetPhone());
            return;
        }
        if (flag == 4) {
            Billing billing;
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetService() == update.GetFilterService() &&
                    _AllVector[i].GetDate() >= update.GetFilterDateMin() &&
                    _AllVector[i].GetDate() <= update.GetFilterDateMax()
                        )
                    _AllVector[i].ChangePhone(update.GetPhone());
            return;
        }
        if (flag == 5) {
            Billing billing;
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetPhone() == update.GetFilterPhone())
                    _AllVector[i].ChangePhone(update.GetPhone());
            return;
        }
        if (flag == 6) {
            Billing billing;
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetService() == update.GetFilterService())
                    _AllVector[i].ChangePhone(update.GetPhone());
            return;
        }
        if (flag == 7) {
            Billing billing;
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetDate() >= update.GetFilterDateMin() &&
                    _AllVector[i].GetDate() <= update.GetFilterDateMax()
                        )
                    _AllVector[i].ChangePhone(update.GetPhone());
            return;
        }
        if (flag == 8) {
            Billing billing;
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetPhone() == update.GetFilterPhone() &&
                    _AllVector[i].GetService() == update.GetFilterService() &&
                    _AllVector[i].GetDate() >= update.GetFilterDateMin() &&
                    _AllVector[i].GetDate() <= update.GetFilterDateMax()
                        )
                    _AllVector[i].ChangeService(update.GetService());
            return;
        }
        if (flag == 9) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetService() == update.GetFilterService() &&
                    _AllVector[i].GetPhone() == update.GetFilterPhone()
                        )
                    _AllVector[i].ChangeService(update.GetService());
            return;
        }
        if (flag == 10) {
            Billing billing;
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetPhone() == update.GetFilterPhone() &&
                    _AllVector[i].GetDate() >= update.GetFilterDateMin() &&
                    _AllVector[i].GetDate() <= update.GetFilterDateMax()
                        )
                    _AllVector[i].ChangeService(update.GetService());
            return;
        }
        if (flag == 11) {
            Billing billing;
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetService() == update.GetFilterService() &&
                    _AllVector[i].GetDate() >= update.GetFilterDateMin() &&
                    _AllVector[i].GetDate() <= update.GetFilterDateMax()
                        )
                    _AllVector[i].ChangeService(update.GetService());
            return;
        }
        if (flag == 12) {
            Billing billing;
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetPhone() == update.GetFilterPhone())
                    _AllVector[i].ChangeService(update.GetService());
            return;
        }
        if (flag == 13) {
            Billing billing;
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetService() == update.GetFilterService())
                    _AllVector[i].ChangeService(update.GetService());
            return;
        }
        if (flag == 14) {
            Billing billing;
            for (size_t i = 0; i < _AllVector.size(); ++i)
                if (_AllVector[i].GetDate() >= update.GetFilterDateMin() &&
                    _AllVector[i].GetDate() <= update.GetFilterDateMax()
                        )
                    _AllVector[i].ChangeService(update.GetService());
            return;
        }
        if (flag == 15) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                _AllVector[i].ChangePhone(update.GetPhone());
            return;
        }
        if (flag == 16) {
            for (size_t i = 0; i < _AllVector.size(); ++i)
                _AllVector[i].ChangeService(update.GetService());
            return;
        }
    }
    else
        throw std::invalid_argument("Its not update");

}

void Parser::GetSave(int &fd) {
    if (_type == 7) {
        if (fd) {
            if (close(fd) == -1)
                throw std::invalid_argument("Cant close file in parser");
            fd = open("database.db", 577, 384);
            if (fd == -1)
                throw std::invalid_argument("Cant open file in parser");
            for (size_t i = 0; i < _AllVector.size(); ++i)
                _AllVector[i].WriteBin(fd);
            return;
        } else
            throw std::invalid_argument("File does not open");
    }
    else
        throw std::invalid_argument("Its not save");
}

std::vector<Billing> Parser::GetResponse() const {
    return _responseVector;
}

int Parser::GetType() const {
    return _type;
}

