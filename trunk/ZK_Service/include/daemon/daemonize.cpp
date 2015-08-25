#include "daemonize.h"

#include <errno.h> 
#include <string.h>
#include <sys/types.h>

#include <gflags/gflags.h>

//DEFINE_bool(daemonize, true, "Run as daemon");
DECLARE_bool(daemonize);

Daemonize::Daemonize()
{
}

Daemonize::~Daemonize()
{
	Finish();
}

void Daemonize::Init()
{
	/* Reset signal handlers */
	/*  if (daemon_reset_sigs(-1) < 0) {
	    daemon_log(LOG_ERR, "Failed to reset all signal handlers: %s", strerror(errno));
	    }*/

	/* Unblock signals */
	/*  if (daemon_unblock_sigs(-1) < 0) {
	    daemon_log(LOG_ERR, "Failed to unblock all signals: %s", strerror(errno));
	    }*/

	if (FLAGS_daemonize)
	{
		/* Prepare for return value passing from the initialization procedure of the daemon process */
		pid_t pid;
		if (daemon_retval_init() < 0) {
			daemon_log(LOG_ERR, "Failed to create pipe.");
		}

		/* Do the fork */
		if ((pid = daemon_fork()) < 0) {

			/* Exit on error */
			daemon_retval_done();

		} else if (pid) { /* The parent */
			int ret;

			/* Wait for 20 seconds for the return value passed from the daemon process */
			if ((ret = daemon_retval_wait(20)) < 0) {
				daemon_log(LOG_ERR, "Could not recieve return value from daemon process: %s", strerror(errno));
				exit(0);
			}

			switch (ret)
			{
				case 0:
					daemon_log(LOG_INFO, "Sucessfully started");
					break;
				case 1:
					daemon_log(LOG_ERR, "Failed to close all file descriptors: %s", strerror(errno));
					break;
				default:
					daemon_log(ret != 0 ? LOG_ERR : LOG_INFO, "Daemon returned %i as return value.", ret);
					break;
			}
			exit(0);

		} else { /* The daemon */
			/* Close FDs */
			//if (daemon_close_all(-1) < 0) {
			/* Send the error condition to the parent process */
			//	daemon_retval_send(1);
			//}

			/*... do some further init work here */
			setsid(); /* become session leader */ 
			chdir("/"); /* change working directory */ 
			umask(0); /* clear file mode creation mask */ 
			close(0); /* close stdin */ 
			close(1); /* close stdout */ 
			close(2); /* close stderr */ 

			/* Send OK to parent process */
			daemon_retval_send(0);

		}
	}
}

void Daemonize::Finish()
{
	daemon_log(LOG_INFO, "Exiting...");
	daemon_retval_send(255);
}
