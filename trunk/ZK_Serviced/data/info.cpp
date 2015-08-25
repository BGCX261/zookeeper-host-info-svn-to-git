#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include <sys/vfs.h>
#include <error.h>



#define Gsize (1024.00*1024.00*1024.00)
#define Msize (1024.00*1024.00)

#ifndef EXT2_SUPER_MAGIC
#define EXT2_SUPER_MAGIC 0xef53
#endif


/*cpu information*/
float get_cpu()
{
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

	return cpu;
}

/*memory information*/ 
unsigned int get_mem_total()
{
	unsigned int mem_total=0;
	char c[20],d[20];

	FILE* f1 = fopen("/proc/meminfo","r");  
	fscanf(f1, "%s\t%d\t%s",c,&mem_total,d);  
	fclose(f1); 

	return mem_total/1024;//MB
}

unsigned int get_mem_free()
{
	unsigned int mem_free=0;
	char c[20],d[20];

	FILE* f1 = fopen("/proc/meminfo","r");  
	fscanf(f1, "%s\t%d\t%s",c,&mem_free,d);  
	fclose(f1); 

	return mem_free/1024;//MB
}

float get_disk_total()
{
	float disk_total=0;

	long long blocks;  
	struct statfs fs;  
	if(statfs("/",&fs)<0)  
	{  
		perror("statfs");  
		exit(0);  
	}

	blocks=fs.f_blocks;  

	if(fs.f_type==EXT2_SUPER_MAGIC)  
	{  
		disk_total=blocks*fs.f_bsize/Gsize;
	}
	
	return disk_total;
}

float get_disk_free()
{
	float disk_free=0;

	long long bfree;  
	struct statfs fs;  
	if(statfs("/",&fs)<0)  
	{  
		perror("statfs");  
		exit(0);  
	}  

	bfree=fs.f_bfree;  

	if(fs.f_type==EXT2_SUPER_MAGIC)  
	{  
		disk_free = bfree*fs.f_bsize/Gsize;
	}  

	return disk_free;
}

int main(int argc, char** argv){

	printf("cpu:%f\n",get_cpu());
	printf("mem_total:%f",get_mem_total());
	printf("mem_free:%f",get_mem_free());
	printf("disk_total:%f",get_disk_total());
	printf("disk_free:%f",get_disk_free());
	return 0;
}

