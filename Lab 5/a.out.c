// Author: Andrew Miner
// Date: 5/6/2020
// Class: Comp 322
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

typedef unsigned int uint;
int isNumber(char* str);

int main(int argc, char* argv[])
{
    if(argc < 2 || isNumber(argv[1])) {
        fprintf(stderr, "Error: missing address argument!\n");
        return EXIT_FAILURE;
    }
    
    const uint PAGE_SIZE = 4096; // getpagesize()
    
    uint address = strtoul(argv[1], NULL, 10); // strtoul converts a string to an int
    uint pageNumb = address / PAGE_SIZE;
    uint pageOffset = address % PAGE_SIZE;
    
    printf("The address %u contains:\n", address);
    printf("page number = %u\n", pageNumb);
    printf("offset = %u\n", pageOffset);
    
	return EXIT_SUCCESS;
}

int isNumber(char* str)
{
    char* i = str;
    while(i != '\0') {
        if(isdigit(*i))
            i++;
        else
            return 0;
    }
    return 1;
}
