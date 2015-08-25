#include "host_info.h"


#include <stdio.h>
#include <sys/time.h>
#include <sstream>

std::string getMemInfo(){
	FILE *fp = fopen("/proc/meminfo", "r");
	if(NULL == fp){
		return "";
	}

	size_t _BUFSIZE = 8192;

	fseek(fp, 0L, SEEK_END);
	size_t flength = ftell(fp);
	if(flength > _BUFSIZE){
		_BUFSIZE = flength + 512;
	}
	fseek(fp, 0L, SEEK_SET);

	char _buf[_BUFSIZE];

	fread((void*)_buf, _BUFSIZE, 1, fp);
	fclose(fp);

	return std::string(_buf);
}
std::string getCpuInfo(){
	float cpu=0.0;
	char c[20];
	int i1,i2,i3,i4,i5,i6;
	
	struct timeval tpstart,tpend;
	float timeuse;
	gettimeofday(&tpstart,NULL);

	FILE* f1 = fopen("/proc/stat", "r");
	fscanf(f1, "%s\t%d\t%d\t%d\n", c, &i1, &i2, &i3);
	fclose(f1);
	
	usleep(1000000);
	
	gettimeofday(&tpend,NULL);
	timeuse = 1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
	timeuse /= 1000000;

	f1 = fopen("/proc/stat", "r");
	fscanf(f1, "%s\t%d\t%d\t%d\n", c, &i4, &i5, &i6);
	fclose(f1);

	int t = (i4+i5+i6)-(i1+i2+i3);
	cpu = t /timeuse;

	std::ostringstream out; 
	if (!(out << cpu)){
		return ""; 
	}
	return out.str();  
}
std::string getDiskInfo(){
	return "";
}
std::string getNetInfo(){
	return "";
}




std::string getInfo(const std::string& type){
	if("mem" == type){
		return getMemInfo();
	}
	if("cpu" == type){
		return getCpuInfo();
	}
	if("disk" == type){
		return getDiskInfo();
	}
	if("net" == type){
		return getNetInfo();
	}

	return "";
}
