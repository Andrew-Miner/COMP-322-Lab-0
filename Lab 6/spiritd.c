// Author: Andrew Miner
// Date: 5/7/2020
// Class: Comp 322
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <limits.h>

void usrHandler(int sigNum);
void termHandler(int sigNum);
pid_t spawnChild();
void daemonize(const char* cmd);

const char* MOLE_NAME = "mole";
static char moleDir[PATH_MAX];

static pid_t cPIDs[2] = {-1, -1};
static int terminate = 0;
static char workingDir[PATH_MAX];
static char* dir;

int main(int argc, char* argv[])
{
    // Gets rid of unused compiler error
    (void)argc;
    
    dir = getcwd(workingDir, sizeof(workingDir));
    
    daemonize(argv[0]);
    
    srand(time(0));
    
    // Get Mole Directory
    if(dir == NULL) {
        strcpy(moleDir, "./");
        strcat(moleDir, MOLE_NAME);
    }
    else {
        strcpy(moleDir, workingDir);
        strcat(moleDir, "/");
        strcat(moleDir, MOLE_NAME);
    }
    
    signal(SIGUSR1, usrHandler);
    signal(SIGUSR2, usrHandler);
    signal(SIGTERM, termHandler);
    
    while(terminate == 0)
        pause();
    
	return 0;
}

void termHandler(int sigNum)
{
    if(terminate == 0 && sigNum == SIGTERM) {
        kill(0, SIGTERM);
        terminate = 1;
    }
}

void usrHandler(int sigNum)
{
    signal(sigNum, usrHandler);
    
    int cIndex = -1;
    if(sigNum == SIGUSR1)
        cIndex = 0;
    else if(sigNum == SIGUSR2)
        cIndex = 1;
    
    if(cIndex == -1)
        return;
    
    pid_t pid = cPIDs[cIndex];
    if(pid != -1) {
        kill(pid, SIGTERM);
        cPIDs[cIndex] = -1;
    }
    
    spawnChild();
}

pid_t spawnChild()
{
    int childIndex = rand() % 2;
    if(cPIDs[childIndex] != -1)
        return -1;
    
    pid_t pid = fork();
    
    // Execute Mole Program
    if(pid == 0) {
        char name[6];
        strcpy(name, "mole");
        strcat(name, childIndex == 0 ? "1" : "2");
        
        char* envp[] = { NULL };
        char* argv[] = { name };
        if(execve(moleDir, argv, envp) == -1) {
            fprintf(stderr, "Error: failed to execute mole program\n");
            exit(EXIT_FAILURE);
        }
    }
    
    // Only called by parent
    cPIDs[childIndex] = pid;
    return pid;
}

void daemonize(const char* cmd)
{
    int                 fd0, fd1, fd2;
    pid_t               pid;
    struct rlimit       rl;
    struct sigaction    sa;
    
    umask(0);
    
    // Get maximum number of file descriptors
    if(getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        fprintf(stderr, "%s: can't get file limit\n", cmd);
        exit(EXIT_FAILURE);
    }
    
    // Fork and become session leader
    if((pid = fork()) < 0) {
        fprintf(stderr, "%s: can't fork\n", cmd);
        exit(EXIT_FAILURE);
    }
    else if(pid != 0) // Parent
        exit(EXIT_SUCCESS);
    setsid();
    
    // Ignore signal to ensure
    // future opens won't allocated
    // controlling TTYs
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask); // 0 out signals
    sa.sa_flags = 0;
    
    if(sigaction(SIGHUP, &sa, NULL) < 0) {
        fprintf(stderr, "%s: can't ignore SIGHUP\n", cmd);
        exit(EXIT_FAILURE);
    }
    if((pid = fork()) < 0) {
        fprintf(stderr, "%s: can't fork\n", cmd);
        exit(EXIT_FAILURE);
    }
    else if(pid != 0) // Parent
        exit(EXIT_SUCCESS);
    
    // Change current working directory to root so we
    // don't prevent file systems from being unmounted
    if(chdir("/") < 0) {
        fprintf(stderr, "%s: can't change directory to /\n", cmd);
        exit(EXIT_FAILURE);
    }
    
    // Close all open file descriptors
    if(rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for(rlim_t i = 0; i < rl.rlim_max; i++)
        close(i);
    
    // Attach file descriptors to 0, 1, and 2 to /dev/null
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    
    // Intitalize log file
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if(fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR,
               "unexpected filedescriptors %d %d %d\n",
               fd0, fd1, fd2);
        exit(EXIT_FAILURE);
    }
}
