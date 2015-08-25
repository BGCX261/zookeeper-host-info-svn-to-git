#ifndef __cluster_proc_h__
#define __cluster_proc_h__

#include <iostream>
#include <tr1/memory>
#include <vector>
#include <zk_framework/zk_framework.h>

class CLS_PROC{

	public:

		CLS_PROC();
		~CLS_PROC();

	public:

		bool _init();
		bool _init_zk();
		bool _init_app();
		void _start();
		void _stop();

		void _handler();
		
	public:


	private:
		
		std::tr1::shared_ptr<ZKOP> pzoo;
		

	private:

		std::string m_hostname;
		std::vector<std::string>m_nodelist;
		std::vector<std::string>::iterator vsit;
		bool m_exit;
};





#endif//cluster_proc.h
