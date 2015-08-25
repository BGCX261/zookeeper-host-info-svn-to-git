#include "cluster_proc.h"

#include <iostream>
#include <sys/utsname.h>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>


DECLARE_string(zk_host);
DECLARE_int32(zk_timeout);
DECLARE_string(node_list);
DECLARE_string(business_name);

DECLARE_string(host);
DECLARE_string(item);
/**
 *
 */
CLS_PROC::CLS_PROC() {
	m_exit = false;
	pzoo = std::tr1::shared_ptr<ZKOP>(new ZKOP());
}

/**
 *
 */
CLS_PROC::~CLS_PROC(){
}

/**
 * 1, init zookeeper
 * 2, init application (all node what you need)
 */
bool CLS_PROC::_init(){
	bool ret_zkini = _init_zk();
	if(!ret_zkini){
		LOG(INFO) << "_init_zk() fail!";
		return false;
	}

	bool ret_apini =  _init_app();
	if(!ret_apini){
		LOG(INFO) << "_init_app() fail!";
		return false;
	}

	return true;
}

/**
 * 1,init zookeeper zkhandle
 */
bool CLS_PROC::_init_zk(){
	bool ret_init = pzoo->zkInit(FLAGS_zk_host, 
			std::tr1::bind(&ZKOP::zkWatcher,
				*pzoo,
				std::tr1::placeholders::_1,
				std::tr1::placeholders::_2,
				std::tr1::placeholders::_3
				),
			FLAGS_zk_timeout);
	if(false == ret_init){
		return false;
	}
	return true;
}


/**
 */
bool CLS_PROC::_init_app(){
	do {
		struct utsname utsn;
		if (uname(&utsn) == -1) {
			m_hostname.assign("unknown");
			break;
		}
		m_hostname.assign(utsn.nodename);
	} while (0);

	std::string _nodes(FLAGS_node_list);
	boost::split( m_nodelist, _nodes, boost::is_any_of( "," ), boost::token_compress_on );

	return true;
}

/**
 *suggest to call zkUpdate func in handler so as to update all nodes data
 */
void CLS_PROC::_handler(){
	std::string _node;
	if(FLAGS_host == "localhost"){
		 _node = FLAGS_business_name + "/" + m_hostname;
	}else{
		 _node = FLAGS_business_name + "/" + FLAGS_host;
	}
	bool exist_node = pzoo->zkNodeExists( _node, 0);
	if(! exist_node){
		printf("host [%s] not found, please to start ZK_Serviced or change another hostname", FLAGS_host.c_str());
		return;
	}

	String_vector children;
	bool ret_child = pzoo->zkGetChildren(_node, children);
	if(!ret_child){
		printf("please start ZK_Serviced on host:[%s]\n", FLAGS_host.c_str());
		return;
	}

	if (children.count <= 0) {
		printf("please restart ZK_Serviced on host:[%s]\n", FLAGS_host.c_str());
		return;
	}

	int i = 0;
	std::string data;
	for (i = 0; i < children.count; i++) {
//std::cout << children.data[i] << std::endl;
		if(FLAGS_item != children.data[i]){
			continue;
		}

		std::string item_node = _node + "/" + children.data[i];
		if (!pzoo->zkGetNodeData(item_node, data, 8192, 0)) {
			printf("get infomation of item:[%s] failed\n", FLAGS_item.c_str());
			continue;
		}
		break;
    	}

	if(i == children.count){
		printf("please check input item:[%s]\n", FLAGS_item.c_str());
		return;
	}

	printf("%s\n", data.c_str());
}


/**
 *
 */
void CLS_PROC::_start(){
	
	do{
		_handler();

	}while(0);
}


/**
 *
 */
void CLS_PROC::_stop(){
	LOG(INFO) << "m_exit=true, and break loop";
	m_exit = true;
}
