#include <iostream>
#include <tr1/memory>
#include <stdlib.h>

#include "cluster_proc.h"
#include <glog/logging.h>
#include <gflags/gflags.h>


DEFINE_string(log_path, "../logs/","log path");
DEFINE_string(host, "localhost", "The host what you wanna show information. default localhost");
DEFINE_string(item, "mem", "The specific information of host. default mem");
DEFINE_string(business_name, "/qss", "The business name.");
DEFINE_string(node_list, "cpu,mem,disk,net,core", "host informaton seperated by comma.");
DEFINE_string(zk_host, "10.119.121.16:2181,10.119.121.37:2182,10.119.112.246:2183,10.119.112.247:2184,10.119.112.248:2185","zookeeper server host");
DEFINE_int32(zk_timeout, 5000,"zookeeper timeout");


int main(int argc, char **argv) {
	::google::ParseCommandLineFlags(&argc, &argv, true);
	std::string log_info = FLAGS_log_path + "/info.";
	std::string log_error = FLAGS_log_path + "/error.";
	::google::SetLogDestination(::google::ERROR, log_error.c_str());
	::google::SetLogDestination(::google::INFO, log_info.c_str());
	::google::InitGoogleLogging(argv[0]);


	std::tr1::shared_ptr<CLS_PROC> pCLS_PROC(new CLS_PROC());
	bool ret_init = pCLS_PROC->_init();
	if(!ret_init){
		printf("init zookeeper fail! and try again in 5 seconds\n");
		return EXIT_FAILURE;
	}

	pCLS_PROC->_start();

	return EXIT_SUCCESS;
}
