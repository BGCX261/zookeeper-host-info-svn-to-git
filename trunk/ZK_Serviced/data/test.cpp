#include "host_info.h"

#include <iostream>


int main(){
	std::string mem_info = getMemInfo();
	std::cout << mem_info << std::endl;
	return 0;
}
