
#ifndef module3_h_
#define module3_h_
#include "result.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "module3.h"
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <sstream>

class Module3
{
    private:
        std::string repoPath;
    public:
        Module3();
        Module3(std::string repoPath);
        void createFolder();
        void saveData(Result result);
        Result getData(long long int taskNr);
        
};
#endif
