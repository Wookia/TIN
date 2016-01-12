#ifndef traceroute_h_
#define traceroute_h_

#include <list>
#include <string>

class Traceroute
{
    public:
        std::list<std::string> road;
        Traceroute& operator= (Traceroute copiedTraceroute)
        {
            this->road = copiedTraceroute.road;
            return *this;
        }
};
#endif
