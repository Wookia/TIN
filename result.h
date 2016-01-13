#ifndef result_h_
#define result_h_

#include "traceroute.h"

class Result
{
    public:
        long long int taskNr;
        std::list<Traceroute> addresses;
        Result& operator= (Result copiedResult)
        {
            this->taskNr = copiedResult.taskNr;
            this->addresses = copiedResult.addresses;
            return *this;
        }
};
#endif
