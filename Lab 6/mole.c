// Author: Andrew Miner
// Date: 5/7/2020
// Class: Comp 322
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <pwd.h>

int main(int argc, char* argv[])
{
    // Gets rid of unused compiler error
    (void)argc;
    
    const char* LOG_NAME = "/lab6.log";
    char logDir[PATH_MAX];
    
    // Get Home Directory: "~/"
    const char* homeDir;
    if((homeDir = getenv("HOME")) == NULL) {
        homeDir = getpwuid(getuid())->pw_dir;
    }
    
    strcpy(logDir, homeDir);
    strcat(logDir, LOG_NAME);
    
    FILE* fp;
    fp = fopen(logDir, "a");
    
    if (fp == NULL) {
        fprintf(stderr,
                "Error: %s can't open %s",
                argv[0], LOG_NAME);
        return EXIT_FAILURE;
    }
    
    fprintf(fp, "Pop %s\n", argv[0]);
    fclose(fp);
    
    pause();
    
	return 0;
}
