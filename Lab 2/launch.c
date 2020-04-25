// Author: Andrew Miner
// Date: 4/24/2020
// Class: Comp 322
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if(argc <= 1)
    {
        fprintf(stderr, "Error: program argument missing!\n");
        return 0;
    }
    
    int retVal = -1;
    int cPID = fork();
    
    if(cPID == 0) // Child Process Code
    {
        // Preparing new args
        char* cArgs[argc];
        cArgs[argc - 1] = NULL;
        for(int i = 1; i < argc; i++)
            cArgs[i - 1] = argv[i];
        
        char* envp[] = { NULL };
        execve(argv[1], cArgs, envp);
    }
    else        // Parent Process Code
    {
        if(cPID == -1)
        {
            fprintf(stderr, "Error: Failed to create child process!\n");
            return 0;
        }

        fprintf(stderr, "%s: $$ = %d\n", argv[1], cPID);
        waitpid(cPID, &retVal, 0);
        fprintf(stderr, "%s: $? = %d\n", argv[1], retVal);
    }
    
	return 0;
}
