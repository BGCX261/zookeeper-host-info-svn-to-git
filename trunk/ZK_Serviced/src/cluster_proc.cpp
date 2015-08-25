#include "cluster_proc.h"
#include "pSysInfo.h"


#include <iostream>
#include <vector>
#include <sys/utsname.h>

#include <glog/logging.h>
#include <gflags/gflags.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>




DECLARE_string(zk_host);
DECLARE_int32(zk_timeout);
DECLARE_int32(query_interval);
DECLARE_string(node_list);
DECLARE_string(business_name);
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
 * 0, get hostname and create business and hostname nodes
 * 1, init all nodes in node_list
 * 2, set flag =  ZOO_EPHEMERAL only node_list
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


	bool ret_zkcreate = false;
	int znode = 0;

	znode = pzoo->zkNodeExists(FLAGS_business_name, 0);
	if (ZNONODE == znode){
		ret_zkcreate = pzoo->zkCreateNodePath(FLAGS_business_name, 0);
		if(!ret_zkcreate){
			LOG(INFO) << "create root(business)[" << FLAGS_business_name << "] fail!";
			return false;
		}
	}

	std::string host_node = FLAGS_business_name + "/" + m_hostname;
	znode = pzoo->zkNodeExists(host_node, 0);
	if (ZNONODE == znode){
		ret_zkcreate = pzoo->zkCreateNodePath(host_node, 0);
		if(!ret_zkcreate){
			LOG(INFO) << "create hostname[" << m_hostname<< "] fail!";
			return false;
		}
	}


	std::string data("null");
	std::string _nodes(FLAGS_node_list);
	boost::split( m_nodelist, _nodes, boost::is_any_of( "," ), boost::token_compress_on );

	for(vsit=m_nodelist.begin(); vsit!=m_nodelist.end(); ++vsit){
		std::string chr_node = host_node + "/" + *vsit;
		znode = pzoo->zkNodeExists(chr_node, 0);
		if (ZNONODE == znode){
			pzoo->zkCreateDataNode(chr_node, data, ZOO_EPHEMERAL);
		}
	}

	return true;
}

/**
 *suggest to call zkUpdate func in handler so as to update all nodes data
 */
void CLS_PROC::_handler(){
	for(vsit=m_nodelist.begin(); vsit!=m_nodelist.end(); ++vsit){
		std::string _node = FLAGS_business_name + "/" + m_hostname + "/" + *vsit;
		std::string _info = getSysInfo(*vsit);
		if(0 != _info.size()){
			pzoo->zkUpdateNodeData(_node, _info, ZOO_EPHEMERAL);
		}
	}
}


/**
 *
 */
void CLS_PROC::_start(){
	
	do{
		_handler();

		usleep(FLAGS_query_interval);
	}while(!m_exit);
}


/**
 *
 */
void CLS_PROC::_stop(){
	LOG(INFO) << "m_exit=true, and break loop";
	m_exit = true;
}
