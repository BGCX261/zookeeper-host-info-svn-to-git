#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>

#include <sstream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <gflags/gflags.h>

#include "check_worker.h"

//DEFINE_string(pid_path, "/home/s/var/proc/", "The directory of pid file for chk_worker_alive.");
DECLARE_string(pid_path);

using namespace std;
using namespace boost;

	CheckWorkerFile::CheckWorkerFile(int argc, char **argv)
: pid_(0)
{
	std::copy(argv, argv + argc, std::back_inserter(argv_));
}

CheckWorkerFile::~CheckWorkerFile()
{
	Remove();
}

bool CheckWorkerFile::Init()
{
	char tmp[PATH_MAX] = {};
	argv_[0].copy(tmp, PATH_MAX);
	string prefix = basename(tmp);
	Init(FLAGS_pid_path, prefix);
	return Create();
}

bool CheckWorkerFile::Init(const std::string& directory, const std::string& prefix)
{
	if (directory.empty() || directory.empty()) {
		return false;
	}
	struct stat st = {};
	if (stat(directory.c_str(), &st) || !S_ISDIR(st.st_mode)) {
		return false;
	}
	directory_ = directory;
	prefix_ = prefix;
	return true;
}

string CheckWorkerFile::GetPath() const
{
	int pid = pid_;
	if (!pid) {
		pid = (int)getpid();
	}
	return directory_ + "/" + prefix_ + "." + lexical_cast<string>(pid) + ".pid";
}

string CheckWorkerFile::GetCmd() const
{
	stringstream ss;
	for (int i=0; i<(int)argv_.size(); ++i) {
		if (i != 0) {
			ss << " ";
		}
		ss << argv_[i];
	}
	return ss.str();
}

bool CheckWorkerFile::Create()
{
	int pid = getpid();
	if (pid_) {
		Remove();
	}
	pid_ = pid;
	string path = GetPath();
	string cmd = GetCmd();
	ofstream ofs(path.c_str());
	if (ofs) {
		ofs << cmd;
		ofs.close();
		return true;
	}
	return false;
}

bool CheckWorkerFile::Remove()
{
	if (pid_ == 0) {
		return true;
	}
	if (unlink(GetPath().c_str()) == 0) {
		pid_ = 0;
		return true;
	}
	return false;
}

