#include "traceroute.h"

class Result
{
    public:
        int taskNr;
        std::list<Traceroute> addresses;
        Result& operator= (Result copiedResult)
        {
            this->taskNr = copiedResult.taskNr;
            this->addresses = copiedResult.addresses;
            return *this;
        }
};
