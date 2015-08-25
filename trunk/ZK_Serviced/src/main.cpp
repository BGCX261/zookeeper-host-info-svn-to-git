#include <iostream>
#include <tr1/memory>
#include <stdlib.h>
#include <signal.h>


#include "cluster_proc.h"
#include <daemon/daemonize.h>
#include <daemon/check_worker.h>
#include <glog/logging.h>
#include <gflags/gflags.h>


DEFINE_bool(daemonize, false, "Run as daemon");
//DEFINE_string(pid_path, "/home/s/var/proc/", "The directory of pid file for chk_worker_alive.");
DEFINE_string(pid_path, "../logs/", "The directory of pid file for chk_worker_alive.");
DEFINE_string(log_path, "../logs/","log path");
DEFINE_string(business_name, "/qss", "The business name.");
DEFINE_string(node_list, "cpu,mem,disk,net,core", "host informaton seperated by comma.");
DEFINE_string(zk_host, "10.119.121.16:2181,10.119.121.37:2182,10.119.112.246:2183,10.119.112.247:2184,10.119.112.248:2185","zookeeper server host");
DEFINE_int32(zk_timeout, 5000,"zookeeper timeout");
DEFINE_int32(query_interval, 5000,"query interval");


void initsignal() {
//return;
	signal(SIGTERM, SIG_IGN);
	signal(SIGINT,  SIG_IGN);
	signal(SIGALRM, SIG_IGN);
	signal(SIGKILL, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGABRT, SIG_IGN);
	signal(SIGFPE,  SIG_IGN);
}

int main(int argc, char **argv) {
	initsignal();

	CheckWorkerFile pid_file(argc, argv);
	Daemonize daemonize;
	::google::ParseCommandLineFlags(&argc, &argv, true);
	daemonize.Init();
	pid_file.Init();

	std::string log_info = FLAGS_log_path + "/info.";
	std::string log_error = FLAGS_log_path + "/error.";
	::google::SetLogDestination(::google::FATAL, (FLAGS_log_path + "/fatal.").c_str());
	::google::SetLogDestination(::google::ERROR, (FLAGS_log_path + "/error.").c_str());
	::google::SetLogDestination(::google::WARNING, (FLAGS_log_path + "/warning.").c_str());
	::google::SetLogDestination(::google::INFO, (FLAGS_log_path + "/info.").c_str());
	::google::InitGoogleLogging(argv[0]);


	LOG(INFO) << "start process " << argv[0];
	std::tr1::shared_ptr<CLS_PROC> pCLS_PROC(new CLS_PROC());
	bool ret_init = pCLS_PROC->_init();
	if(!ret_init){
		LOG(INFO) << "service break down!" << argv[0];
		return EXIT_FAILURE;
	}
	usleep(100);

	LOG(INFO) << "service running...";

	pCLS_PROC->_start();

	LOG(INFO) << "exit process" << argv[0];
	return EXIT_SUCCESS;
}
