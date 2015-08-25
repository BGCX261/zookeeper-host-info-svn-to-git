#include "zk_framework.h"
#include <sstream>


/**
 * we must wrap a class to call member function in ZKOP
 * because member function cannot be callback function
*/ 
/////////////////////////////////////////////////////////////////////////////////
namespace detail
{
	class ZooKeeperWatcher 
	{/*{{{*/
		public:
			typedef ZKOP::wrapFunc wrapFunc;

		public:
			explicit ZooKeeperWatcher(const wrapFunc& func)
				: func_(func)
			{}
			void RunInWatcher(int type, int state, const char* path) {
				func_(path, type, state);
			}

		private:
			wrapFunc func_;
	}/*}}}*/;

	void watcher(zhandle_t* zh, int type, int state, const char* path, void* ctx) 
	{/*{{{*/
		ZooKeeperWatcher* watcher = static_cast<ZooKeeperWatcher*>(ctx);
		watcher->RunInWatcher(type, state, path);
	}/*}}}*/
}//namespace detail
/////////////////////////////////////////////////////////////////////////////////









//ZKOP  CLASS Methords
//
//
//
//
/**
 * ZKOP constructor
*/ 
ZKOP::ZKOP() 
: zkhandle(NULL), context_(NULL), m_host(""), m_timeout(3000) 
{/*{{{*/
	memset(&clientid_, 0, sizeof(clientid_));
}/*}}}*/

/**
 * ZKOP destructor
 * if not NULL
 * 1,delete handler
 * 2,delete context 
*/ 
ZKOP::~ZKOP()
{/*{{{*/
	if (zkhandle != NULL) {
		zookeeper_close(zkhandle);
		zkhandle = NULL;
	}

	if (context_ != NULL) {
		delete context_;
		context_ = NULL;
	}
}/*}}}*/


/**
 * 1,set debug level of log
 * 2,init zookeeper(connect host and set timeout and callback func)
 * 3,if fail then false else true 
 * \param host indicate zookeeper server lists
 * \param fun indicate callback func
 * \param timeout indicate timeout of zookeeper
*/ 
bool ZKOP::zkInit(const std::string& host, const wrapFunc& func, const int timeout)
{/*{{{*/
	zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
	m_host = host;
	m_timeout = timeout;
	m_func = func;

	context_ = new detail::ZooKeeperWatcher(func);
	zkhandle = zookeeper_init(host.data(), 
			detail::watcher, 
			timeout,
			&clientid_,
			context_,
			0);

	//if (zoo_state(zkhandle) != ZOO_CONNECTED_STATE) ???
	if (NULL == zkhandle) {
		LOG(INFO) << "init zookeeper handle fail!\t"<< "host:" << host;
		return false;
	}else{
		LOG(INFO) << "init zookeeper successful!";
		return true;
	}
}/*}}}*/


/**
 \param path indicate zknode
 \param type ..
 \param state ..
*/ 
void ZKOP::zkWatcher(const char* path, int type, int state) 
{/*{{{*/
	if (type == ZOO_SESSION_EVENT) {
		if (state == ZOO_CONNECTED_STATE) {
			LOG(INFO) << "zookeeper service ok";
		} else if (state == ZOO_EXPIRED_SESSION_STATE) {
			LOG(INFO) << "Zookeeper session expired!\t" << path;
			delete context_;
			context_ = NULL;

			zkInit(m_host, m_func, m_timeout);
			//zkCreateDataNode(path, "expired", ZOO_EPHEMERAL);
		}
	}
}/*}}}*/


/**
* zk_acreate func will call this func 
*/ 
//void ZKOP::zkStrCompletion(int rc, const char *name, const void *data) {
//	//char* _data = (data==0?"null":data);
//	//LOG(INFO) << _data;
//	if (!rc) {
//		LOG(INFO) << "\tname =" << name;
//	}
//}



/**
 * create node and set value in zookeeper server
 * \param node indicate key
 * \param data indicate value
 * \param flag =ZOO_EPHEMERAL
*/ 
bool ZKOP::zkCreateDataNode(const std::string& node, const std::string& data, int flag) 
{/*{{{*/
	int rc = 0;
	bool __ret = true;
	if ((rc = zoo_create(zkhandle,
					node.data(), data.data(),
					data.size(),
					&ZOO_OPEN_ACL_UNSAFE,
					flag, NULL, 0)) != ZOK) {
		LOG(ERROR) << "create data node failed, node = [" << node << "], data = [" << data << "], error = [" << zerror(rc) << "]";
		__ret = false;
	}else{
		LOG(INFO) << "create data node succ, node = [" << node << "], data = [" << data << "]";
		__ret = true;
	}

	return __ret;
}/*}}}*/


/**
 * create node in zookeeper server
 * \param node indicate key
 * param flag =ZOO_EPHEMERAL
*/ 
bool ZKOP::zkCreateNodePath(const std::string& node, int flag) 
{/*{{{*/
	int rc = 0;
/*
	if ((rc = zoo_exists(zkhandle, node.data(), 0, NULL)) == ZOK) {
		return true;
	}

	if (rc != ZNONODE) {
		//LOG(ERROR) << "create new node failed, node = [" << node << "], error = [" << zerror(rc) << "]";
		return false;
	}

	if (zoo_exists(zkhandle, node.data(), 0, NULL) == ZNONODE) {
		if ((rc = zoo_create(zkhandle, node.data(),
						NULL, -1, &ZOO_OPEN_ACL_UNSAFE,
						flag, NULL, 0)) != ZOK) {
			LOG(ERROR) << "create new node failed, node = [" << node << "], error = [" << zerror(rc) << "]";
			return false;
		}
	}

	LOG(INFO) << "create new node successful, node = [" << node << "], flag = [" << flag << "]";

	return true;
*/
	rc = zkNodeExists(node, flag);
	if( ZOK != rc ) {
		return false;
	}
	if(ZNONODE == rc){
		if ((rc = zoo_create(zkhandle, node.data(),
						NULL, -1, &ZOO_OPEN_ACL_UNSAFE,
						flag, NULL, 0)) != ZOK) {
			LOG(ERROR) << "create new node failed, node = [" << node << "], error = [" << zerror(rc) << "]";
			return false;
		}
	}else{
		std::stringstream ss;
		std::string str;
		ss << rc;
		ss >> str;
		LOG(ERROR) << "create new node failed, node = [" << node << "], error = [" << zerror(rc) << "]" << "\trc=" << str;
		return false;
	}

	return true;
}/*}}}*/

int ZKOP::zkNodeExists(const std::string& node, int watch)
{/*{{{*/
	int ret = zoo_exists(zkhandle, node.data(), watch, NULL);
	if(ZNONODE == ret){
		return ZNONODE;
	}
	if(ZOK == ret){
		return ZOK;
	}

	return ret;
}/*}}}*/

bool ZKOP::zkGetNodeData(const std::string& node, std::string& data, int size, int watch) 
{/*{{{*/

	data.resize(size);

	int rc = 0;
	if ((rc = zoo_get(zkhandle, node.data(), watch, &data[0], &size, NULL)) != ZOK) {
		LOG(ERROR) << "get node data failed, node = ["
			<< node << "], error = [" << zerror(rc) << "]" << std::endl;
		return false;
	}

	do {
		if (size < 0) {
			data.resize(0);
			break;
		}   
		data.resize(size);
	} while (0);

/*
#ifndef NDEBUG
	LOG(INFO) << "get node data successful, node = [" << node
		<< "], data = [" << data << "], size = [" << size << "]" << std::endl;
#endif
*/

	return true;
}/*}}}*/

bool ZKOP::zkFreezkVector(zkVector& zkvector)
{/*{{{*/
	int rc = 0;
	if ((rc = deallocate_String_vector(&zkvector)) != ZOK) {
		LOG(ERROR) << "rree node children failed, error = ["
			<< zerror(rc) << "]" << std::endl;
		return false;
	}

/*
#ifndef NDEBUG
	LOG(INFO) << "free node children successful, count = ["
		<< zkvector.count << "]" << std::endl;
#endif
*/

	return true;
}/*}}}*/

bool ZKOP::zkGetChildren(const std::string& node, zkVector& zkvector)
{/*{{{*/
	int rc = 0;
	if ((rc = zoo_get_children(zkhandle, node.data(), 0, &zkvector)) != ZOK) {
		LOG(ERROR) << "get node children failed, node = ["
			<< node << "], error = [" << zerror(rc) << "]" << std::endl;
		return false;
	}

	
/*
#ifndef NDEBUG
	LOG(INFO) << "get node children successful, node = [" << node
		<< "], count = [" << zkvector.count << "]" << std::endl;
#endif
*/

	return true;
}/*}}}*/

/**
 * update node and data
 * \param node indicate key
 * \param data indicate value
 * \param flag =ZOO_EPHEMERAL
*/ 
bool ZKOP::zkUpdateNodeData(const std::string& node, const std::string& data, int flag) 
{/*{{{*/
	if (!zkCreateNodePath(node, flag)) {
		return false;
	}

	int rc = 0;
	if ((rc = zoo_set(zkhandle, node.data(), data.data(), data.size(), -1)) != ZOK) {
		LOG(ERROR) << "update node data failed, node = [" << node << "], data = [" << data << "], error = [" << zerror(rc) << "]";
		return false;
	}

/*
#ifndef NDEBUG
	LOG(INFO) << "update node data successful, node = [" << node << "], data = [" << data << "]";
#endif
*/

	return true;
}/*}}}*/
