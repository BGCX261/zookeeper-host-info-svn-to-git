
#include "pSysInfo.h"

#include <sstream>
#define  MAXCPU   64
#define  MAXCNT   64
typedef long long count_t;

using namespace std;


/*******************************************************************************/
struct percpu{
	int cpunr;
	count_t stime; //  system  time in clock ticks
	count_t utime; //  user    time in clock ticks
	count_t ntime; //  nice    time in clock ticks  
	count_t itime; //  idle    time in clock ticks
	count_t wtime; //  iowait  time in clock ticks
	count_t Itime; //  irq     time in clock ticks
	count_t Stime; //  softirq time in clock ticks
	count_t steal; //  steal   time in clock ticks
	count_t guest; //  guest   time in clock ticks
	count_t cfuture[4]; //reserved for future use
};

struct cpustat{
	count_t nrcpu;   //number of cpu's
	count_t devint;  //number of device interrupts
	count_t csw;     //number of context switches
	count_t nprocs;  //number of processes started
	float   lavg1;   //load average last    minute
	float   lavg5;   //load average last  5 minutes
	float   lavg15;  //load average last 15 minutes
	count_t cfuture[4]; //reserved for future use

	struct percpu all;
	struct percpu cpu[MAXCPU];

};




/*****************************************************************************/

struct cpustat *  getCpuStat(){

	register int i,nr;
	count_t  cnts[MAXCNT];
	float lavg1,lavg5,lavg15;
	
	FILE   *fp;
	char linebuf[1024],nam[64],origdir[1024];
	struct cpustat *cpu=(struct cpustat *)malloc(sizeof(struct cpustat)); 
	memset(cpu,0,sizeof(struct  cpustat));

	getcwd(origdir,sizeof origdir);
	chdir("/proc");
	if((fp=fopen("stat","r"))!=NULL)
	{
		while(fgets(linebuf,sizeof(linebuf),fp)!=NULL)
		{
			nr=sscanf(linebuf,"%s   %lld %lld %lld %lld %lld %lld %lld "
					"%lld %lld %lld %lld %lld %lld %lld %lld ",
					nam,
					&cnts[0],  &cnts[1],  &cnts[2],  &cnts[3], 
					&cnts[4],  &cnts[5],  &cnts[6],  &cnts[7],
					&cnts[8],  &cnts[9],  &cnts[10], &cnts[11],
					&cnts[12], &cnts[13], &cnts[14]);
			if(nr<2)
				continue;

			if(strcmp("cpu",nam)==0)
			{
				cpu->all.utime = cnts[0];
				cpu->all.ntime = cnts[1];
				cpu->all.stime = cnts[2];
				cpu->all.itime = cnts[3];
				if(nr>5)
				{
					cpu->all.wtime= cnts[4];
					cpu->all.Itime= cnts[5];
					cpu->all.Stime= cnts[6];

					if(nr>8)
						cpu->all.steal= cnts[7];
					if(nr>9)
						cpu->all.guest= cnts[8];

				}
				continue;
                        
			}
			if(strncmp("cpu", nam,3)==0)
			{
				i=atoi(&nam[3]);
                                cpu->cpu[i].cpunr=i;
				cpu->cpu[i].utime= cnts[0];
				cpu->cpu[i].ntime= cnts[1];
				cpu->cpu[i].stime= cnts[2];
				cpu->cpu[i].itime= cnts[3];
				if(nr>5)
				{
					cpu->cpu[i].wtime=cnts[4];
					cpu->cpu[i].Itime=cnts[5];
					cpu->cpu[i].Stime=cnts[6];

					if(nr>8)
						cpu->cpu[i].steal=cnts[7];
					if(nr>9)
						cpu->cpu[i].guest= cnts[8];
				}
				cpu->nrcpu++;
				continue;

			}

			if(strcmp("ctxt",nam)==0)
			{
                     	cpu->csw=cnts[0];
				continue;
			}
			if(strcmp("intr",nam)==0)
			{
				cpu->devint=cnts[0];
				continue;
			}
			if(strcmp("processes",nam)==0)
			{
			cpu->nprocs=cnts[0];
				continue;
			}


		}

		fclose(fp);
		if(cpu->nrcpu==0)
			cpu->nrcpu=1;
	}  
        
       
	if((fp=fopen("loadavg","r"))!=NULL)
	{
		if(fgets(linebuf,sizeof(linebuf),fp)!=NULL)
		{
			if(sscanf(linebuf,"%f %f %f",&lavg1,&lavg5,&lavg15)==3)
			{
				cpu->lavg1=lavg1;
				cpu->lavg5=lavg5;
				cpu->lavg15=lavg15;
			}   

		} 

		fclose(fp);
	} 
     chdir(origdir);

    return cpu;
     
}

std::string getCpuInfo(long sleepTime=500)
{
	struct cpustat *cpu1=getCpuStat();

	string result("");
	
	if(cpu1==NULL)
	{ 
		return result; 
	}

	usleep(sleepTime*1000);
	struct cpustat *cpu2=getCpuStat();

	if(cpu2==NULL)
	{ 
		free(cpu1);
		return result; 
	}

	char   result_ss[1024]; 
	// double total1,total2;
	sprintf(result_ss,"load Avage: %f %f %f \n", cpu1->lavg1, cpu1->lavg5, cpu1->lavg15);
	result.append(std::string(result_ss));  

	result.append("CPU\tUser(0.01s) nice(0.01s) system(0.01s) idle(0.01s)  iowait(0.01s)  irq(0.01s) softirq(0.01s) steal(0.01s) guest(0.01s)\t");
	sprintf(result_ss,"\nCPU\t %lld\t\t %lld\t\t %lld\t %lld\t\t %lld\t\t %lld\t\t %lld\t %lld\t   %lld\n",
			cpu2->all.utime-cpu1->all.utime, 
			cpu2->all.ntime-cpu1->all.ntime,
			cpu2->all.stime-cpu1->all.stime,
			cpu2->all.itime-cpu1->all.itime,
			cpu2->all.wtime-cpu1->all.wtime,
			cpu2->all.Itime-cpu1->all.Itime,
			cpu2->all.Stime-cpu1->all.Stime,
			cpu2->all.steal-cpu1->all.steal,
			cpu2->all.guest-cpu1->all.guest);
	result.append(std::string(result_ss));
	for(int i=0;i<cpu1->nrcpu;i++){

		sprintf(result_ss,"CPU%d\t %lld\t\t %lld\t\t %lld\t %lld\t\t %lld\t\t %lld\t\t %lld\t %lld\t %lld\n",i,
				cpu2->cpu[i].utime-cpu1->cpu[i].utime, 
				cpu2->cpu[i].ntime-cpu1->cpu[i].ntime,
				cpu2->cpu[i].stime-cpu1->cpu[i].stime,
				cpu2->cpu[i].itime-cpu1->cpu[i].itime,
				cpu2->cpu[i].wtime-cpu1->cpu[i].wtime,
				cpu2->cpu[i].Itime-cpu1->cpu[i].Itime,
				cpu2->cpu[i].Stime-cpu1->cpu[i].Stime,
				cpu2->cpu[i].steal-cpu1->cpu[i].steal,
				cpu2->cpu[i].guest-cpu1->cpu[i].guest);
		result.append(std::string(result_ss));

	}
	free(cpu1);
	free(cpu2);
	return result;

}

std::map<std::string,std::string> cmd_list;

void init_cmd_list(){
	cmd_list["cpu"]="iostat -c -x -m 1 2";
	cmd_list["mem"]="vmstat";
	cmd_list["net"]="netstat ";
	cmd_list["disk"]="iostat -d -x  1 2";
	cmd_list["core"]="ls -l $( dirname `cat /proc/sys/kernel/core_pattern` )";
}

std::string getSysInfo(std::string& infoType){

	FILE *fp;
	std::string result;
	char buf[1024];   
       	init_cmd_list();
        if(strcmp(infoType.c_str(),"cpu")==0){

           return getCpuInfo(100);
        }
	fp=popen( cmd_list[infoType].c_str(),"r");
      
	while(fgets(buf,sizeof(buf),fp)){

		result.append(std::string(buf));

	}

	pclose(fp);
	return result;
}
