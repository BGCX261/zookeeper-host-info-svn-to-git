/*function: supply a framework of zookeeper to process some task
 *          and the task only  need to modify zkHandler 
 *
 *created:Thu, 26 Sep 2013 16:25:41 +0800
 *author:muyanxue@360.cn
 *
 */

#ifndef _ZK_FRAMEWORK_H
#define _ZK_FRAMEWORK_H

#include <iostream>
#include <stdlib.h>
#include <tr1/functional>

#include "glog/logging.h"
#include "gflags/gflags.h"
#include "zookeeper/zookeeper.h"
//#include "zookeeper/zookeeper_log.h"

namespace detail { class ZooKeeperWatcher; }

class ZKOP{
	public:
		ZKOP();
		~ZKOP();

	public:
		typedef std::tr1::function<void(const char* path, int type, int state)> wrapFunc;
		typedef String_vector zkVector;

	public:

		bool zkInit(const std::string& host, const wrapFunc& func, const int timeout);
		bool zkCreateNodePath(const std::string& node, int flag);
		bool zkCreateDataNode(const std::string& node, const std::string& data, int flag);
		int  zkNodeExists(const std::string& node, int watch);
		bool zkUpdateNodeData(const std::string& node, const std::string& data, int flag);
		bool zkGetNodeData(const std::string& node, std::string& data, int size, int watch);
		bool zkGetChildren(const std::string& node, zkVector& zkvector);
		bool zkFreezkVector(zkVector& zkvector);
		void zkStrCompletion(int rc, const char *name, const void *data);
		void zkWatcher(const char* path, int type, int state);

	private:
		zhandle_t* zkhandle;
		detail::ZooKeeperWatcher* context_;
		clientid_t clientid_;

		int m_timeout;
		std::string m_host;
		wrapFunc m_func;

};

#endif//endif zk_frame.h
