// Author: Andrew Miner
// Date: 4/25/2020
// Class: Comp 322
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>

const int TERM_EXIT_COUNT = 3;
static int termCount = 0;
static int sigCount = 0;

int getSignal(char* name);
char* toUpper(char* str);
char* toLower(char* str);
void sigHandler(int);
void termHandler(int sigNum);

int main(int argc, char* argv[])
{
    if(argc <= 1) {
        fprintf(stderr, "Error: missing signals to catch!\n");
        return EXIT_FAILURE;
    }
    
    fprintf(stderr, "catcher: $$ = %d\n", getpid());
    
    int termFound = 0;
    for(int i = 1; i < argc; i++) {
        int sig = getSignal(toLower(argv[i]));
        
        if(sig == -1) {
            fprintf(stderr, "Error: %s is not a valid signal!\n", toUpper(argv[i]));
            continue;
        }
        
        if(sig == SIGTERM)
            termFound = 1;
        
        signal(sig, sigHandler);
    }
    
    while(termFound && termCount < TERM_EXIT_COUNT)
        pause();
    
    if(!termFound)
        fprintf(stderr, "Error: terminated because SIGTERM is not being tracked!\n");
    
    fprintf(stderr, "catcher: Total signals count = %d\n", sigCount);
    
    return EXIT_SUCCESS;
}

void sigHandler(int sigNum)
{
    signal(sigNum, sigHandler);
    
    char* sigName = toUpper(strdup(sys_signame[sigNum]));
    printf("SIG%s caught at %ld\n", sigName, time(NULL));
    free(sigName);
    
    if(sigNum == SIGTERM)
        termCount++;
    else
        termCount = 0;
    sigCount++;
}

char* toUpper(char* str)
{
    char* tmp = str;
    while(*str) {
        *str = toupper(*str);
        str++;
    }
    return tmp;
}

char* toLower(char* str)
{
    char* tmp = str;
    while(*str) {
        *str = tolower(*str);
        str++;
    }
    return tmp;
}

int getSignal(char* name)
{
    for(int sig = 1; sig < NSIG; sig++) {
        char* sigName = strdup(sys_signame[sig]);
        if(sigName && strcmp(name, sigName) == 0)
            return sig;
        free(sigName);
    }
    return -1;
}
