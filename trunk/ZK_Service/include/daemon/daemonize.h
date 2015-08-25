#ifndef _DAEMONIZE_H_
#define _DAEMONIZE_H_

#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <libdaemon/daemon.h>

class Daemonize
{
  public:
    Daemonize();
    ~Daemonize();
    void Init();

  private:
    void Finish();
};

#endif
