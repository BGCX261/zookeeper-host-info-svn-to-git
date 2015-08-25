#ifndef _CHECK_WORKER_H_
#define _CHECK_WORKER_H_

#include <string>
#include <vector>

class CheckWorkerFile {
public:
    CheckWorkerFile(int argc, char** argv);
    ~CheckWorkerFile();

    bool Init();
    bool Init(const std::string& directory, const std::string& prefix);

    bool Create();
    bool Remove();

    std::string GetPath() const;
    std::string GetCmd() const;

private:
    std::vector<std::string> argv_;
    std::string directory_;
    std::string prefix_;
    int pid_;
};

#endif
