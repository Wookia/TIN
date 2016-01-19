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
	
    std::ifstream in;
	std::string line;
    in.open(path.c_str());
    if (in.is_open())
    {
        while(getline(in, line))
        {
            std::string ssr(line);
			std::string key;
			std::string value;
            if(ssr.back()=='\r') ssr.pop_back();
            std::size_t i = 0;
            std::size_t j = 0;
            j = i;
            i = ssr.find(':', j);
            key = ssr.substr(j, i-j);
            i++;
			j = i;
            i = ssr.find(':', j);
            value = ssr.substr(j, i-j);
			std::cout<<value<<std::endl;
			if("size_limit" == key)size_limit = std::stoi(value);
			else if("port_number" == key)port_number = std::stoi(value);
			else if("ip_address" == key)ip_address = value;
			else if("max_ttl" == key)max_ttl = std::stoi(value);
			else if("max_packets_per_ttl" == key)max_packets_per_ttl = std::stoi(value);
			else if("timeout" == key)timeout = std::stoi(value);
			else if("freq" == key)freq = std::stoi(value);
			else if("repo_path" == key)repo_path = value;
        }
        in.close();
    }
	std::cout<<size_limit<<" "<<port_number<<" "<<ip_address <<" "<<max_ttl <<" "<<max_packets_per_ttl <<" "<<timeout <<" "<< freq<<" "<<repo_path<<std::endl;
}
