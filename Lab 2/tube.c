// Author: Andrew Miner
// Date: 4/24/2020
// Class: Comp 322
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getDelimiter(char* delimiter, int argc, char* argv[])
{
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(delimiter, argv[i]) == 0)
            return i;
    }
    return -1;
}

int main(int argc, char* argv[])
{
    if(argc <= 1)
    {
        fprintf(stderr, "Error: program arguments missing!\n");
        return 0;
    }
    
    // Find comma position
    int delimiterPos = getDelimiter(",", argc, argv);
    if(delimiterPos == -1)
    {
        fprintf(stderr, "Error: missing delimiter (\",\")!");
        return 0;
    }
    
    // Create Pipe
    int pipefd[2];
    if(pipe(pipefd) == -1)
    {
        fprintf(stderr, "Error: failed to create pipe!\n");
        return 0;
    }
    
    // Spawn first child
    pid_t c1PID = fork();
    if(c1PID == -1)
    {
        fprintf(stderr, "Error: failed to create child process!");
        return 0;
    }
    
    if(c1PID == 0) // Child1 Process Code
    {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        
        // Preparing new args
        char* cArgs[delimiterPos];
        cArgs[delimiterPos - 1] = NULL;
        for(int i = 1; i < delimiterPos; i++)
            cArgs[i - 1] = argv[i];
        
        char* envp[] = { NULL };
        execve(argv[1], cArgs, envp);
    }
    else // Parent Process Code
    {
        // Spawn second child
        pid_t c2PID = fork();
        if(c2PID == -1)
        {
            fprintf(stderr, "Error: failed to create child process!");
            return 0;
        }
        
        if(c2PID == 0) // Child2 Process Code
        {
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[1]);
            
            int c2_argc = argc - delimiterPos;
            char* cArgs[c2_argc];
            cArgs[c2_argc - 1] = NULL;
            for(int i = 0; i < c2_argc - 1; i++)
                cArgs[i] = argv[i + delimiterPos + 1];
            
            char* envp[] = { NULL };
            execve(argv[delimiterPos + 1], cArgs, envp);
        }
        else // Parent Process Code
        {
            fprintf(stderr, "%s: $$ = %d\n", argv[1], c1PID);
            fprintf(stderr, "%s: $$ = %d\n", argv[delimiterPos + 1], c2PID);
            
            close(pipefd[0]);
            close(pipefd[1]);
            
            int ret1Val = -1;
            waitpid(c1PID, &ret1Val, 0);
            fprintf(stderr, "%s: $? = %d\n", argv[1], ret1Val);
            
            int ret2Val = -1;
            waitpid(c2PID, &ret2Val, 0);
            fprintf(stderr, "%s: $? = %d\n", argv[delimiterPos + 1], ret2Val);
        }
    }
    
	return 0;
}
