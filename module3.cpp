#include "module3.h"

Module3::Module3():repoPath("")
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    this->repoPath = std::string(cwd)+std::string("/data/");
    this->createFolder();
    
}

Module3::Module3(std::string repoPath):repoPath(repoPath+std::string("/data/"))
{
    this->createFolder();
}

void Module3::createFolder()
{
    struct stat info;

    if( stat( this->repoPath.c_str(), &info ) != 0 )
        mkdir(this->repoPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    else if( info.st_mode & S_IFDIR );
    else
        mkdir(this->repoPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void Module3::saveData(Result result)
{
    std::ostringstream ss;
    ss << this->repoPath << result.taskNr << ".txt";
    std::ofstream out((ss.str()).c_str());
    while (!result.addresses.empty())
    {
        Traceroute traceroute = result.addresses.front();
        while(!traceroute.road.empty())
        {
            std::string address = traceroute.road.front();
            traceroute.road.pop_front();
            if(traceroute.road.empty())
                out << address << std::endl;
            else
                out << address << ";";
        }
        result.addresses.pop_front();
    }
    out.close();
}
Result Module3::getData(int taskNr)
{
    Result result;
    result.taskNr = taskNr;
    std::string line;
    std::ostringstream ss;
    ss << this->repoPath << taskNr << ".txt";
    std::ifstream in((ss.str()).c_str());
    if (in.is_open())
    {
        while(getline(in, line))
        {
            Traceroute traceroute;
            std::string ssr(line);
            std::cout<<ssr.c_str()<<std::endl;
            std::size_t i = 0;
            std::size_t j = 0;
            while (i!=std::string::npos)
            {
                j = i;
                i = ssr.find(';', j);
                if(i!=std::string::npos)
                {
                    traceroute.road.push_back(ssr.substr(j, i-j));
                }
                else
                {
                    traceroute.road.push_back(ssr.substr(j, i-j));
                    break;
                }
                i++;
                
            }
            result.addresses.push_back(traceroute);
            
        }
        in.close();
    }
    else
    {
        result.taskNr = -1;
    }
    return result;
}
