#include "module3.cpp"

int main()
{
    Module3 module3;
    Result result;
    result.taskNr = 1;
    Traceroute traceroute1;
    traceroute1.road.push_back(std::string("121.10.1.2"));
    traceroute1.road.push_back(std::string("121.20.1.2"));
    traceroute1.road.push_back(std::string("133.10.3.2"));
    traceroute1.road.push_back(std::string("123.10.1.2"));
    Traceroute traceroute2;
    traceroute2.road.push_back(std::string("123.10.1.2"));
    traceroute2.road.push_back(std::string("131.20.15.2"));
    traceroute2.road.push_back(std::string("133.410.3.2"));
    traceroute2.road.push_back(std::string("163.190.1.02"));
    traceroute2.road.push_back(std::string("133.10.3.2"));
    Traceroute traceroute3;
    traceroute3.road.push_back(std::string("1216.10.51.2"));
    traceroute3.road.push_back(std::string("1321.20.61.2"));
    traceroute3.road.push_back(std::string("133.310.3.2"));
    result.addresses.push_back(traceroute1);
    result.addresses.push_back(traceroute2);
    result.addresses.push_back(traceroute3);
    module3.saveData(result);
    Result res = module3.getData(1);
    res.taskNr = 2;
    module3.saveData(res);
    
}
