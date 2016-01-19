
#ifndef params_h_
#define params_h_
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
class Params
{
    public:
        int port_number;
		int size_limit;
        std::string ip_address;
        int max_ttl;
        int max_packets_per_ttl;
        int timeout;
        int freq;
        std::string repo_path;
        Params();
        Params(std::string path);
};
#endif
