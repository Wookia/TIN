#include "Params.h"
Params::Params()
{
	size_limit = 100000;
	port_number = 8080;
	ip_address = "127.0.0.1";
    max_ttl = 20;
    max_packets_per_ttl = 8;
    timeout = 20;
    freq = 3;
    repo_path = "";
}

Params::Params(std::string path)
{

}
