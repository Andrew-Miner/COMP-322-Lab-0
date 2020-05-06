// Author: Andrew Miner
// Date: 2/29/2020
// Class: Comp 322
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void printPIDs(int child, int retVal)
{
    printf("PPID: %d, PID: %d", getppid(), getpid());
    
    if(child != 0)
        printf(", CPID: %d, RETVAL: %d", child, retVal);
    
    printf("\n");
}

int main()
{
    printf("START: %ld\n", time(NULL));
    
    struct tms buf;
    
    int retVal = -1;
    int child = fork();
    
    waitpid(child, &retVal, 0);
    printPIDs(child, retVal);
    
    if(child == 0)
        exit(0);
    
    times(&buf);
    printf("USER: %ld, SYS: %ld\n", buf.tms_utime, buf.tms_stime);
    printf("CUSER: %ld, CSYS: %ld\n", buf.tms_utime, buf.tms_stime);
    printf("STOP: %ld\n", time(NULL));
    
	return 0;
}
