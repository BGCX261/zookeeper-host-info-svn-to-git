#ifndef __PSYSINFO_H__
#define __PSYSINFO_H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/param.h>
#include <ctype.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;
std::string getSysInfo(std::string& infoType);

#endif
