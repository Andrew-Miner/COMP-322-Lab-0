// Author: Andrew Miner
// Date: 5/9/2020
// Class: Comp 322
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

#define SEM_PREFIX "chopstick_"
#define SYNC_SEM "sync_sem"

void eat(int phID);
void think(int phID);

int isNumber(const char* str);
void termHandler(int sigNum);
sem_t* openChopstick(int id);
void closeChopstick(int id, sem_t* sem);
sem_t* openSem(char* name);

// Global need for access
// from signal callback
int terminate = 0;

int main(int argc, char* argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Error: missing \"where\" and \"seats\" arguments\n");
        return EXIT_FAILURE;
    }
    
    if(!isNumber(argv[2])) {
        fprintf(stderr, "Error: %s is not a number\n", argv[2]);
        return EXIT_FAILURE;
    }
    
    if(!isNumber(argv[1])) {
        fprintf(stderr, "Error: %s is not a number\n", argv[1]);
        return EXIT_FAILURE;
    }
    
    srand(time(0));
    
    const int SEAT_COUNT = atoi(argv[2]);
    const int SEAT_LOC = atoi(argv[1]);
    
    signal(SIGTERM, termHandler);
    
    /*
     * Semaphore/Philosopher logic
     */
    
    sem_t* lChopstick = openChopstick(SEAT_LOC);
    sem_t* rChopstick = openChopstick((SEAT_LOC + 1) % SEAT_COUNT);
    sem_t* allocSem = openSem(SYNC_SEM);
    
    int cycles = 0;
    
    do {
        sem_wait(allocSem);
        sem_wait(lChopstick);
        sem_wait(rChopstick);
        sem_post(allocSem);
        
        eat(SEAT_LOC);
        
        sem_post(lChopstick);
        sem_post(rChopstick);
        
        think(SEAT_LOC);
        
        cycles++;
    } while (terminate == 0);
    
    closeChopstick(SEAT_LOC, lChopstick);
    closeChopstick((SEAT_LOC + 1) % SEAT_COUNT, rChopstick);
    
    fprintf(stderr,
            "Philosopher #%d completed %d cycles\n",
            SEAT_LOC,
            cycles);
    
	return EXIT_SUCCESS;
}

void eat(int phID)
{
    printf("Philosopher #%d is eating\n", phID);
    usleep(rand() % 10000000 + 1);
}

void think(int phID)
{
    printf("Philosopher #%d is thinking\n", phID);
    usleep(rand() % 10000000 + 1);
}

int isNumber(const char* str)
{
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

void termHandler(int sigNum) {
    signal(sigNum, termHandler);
    terminate = 1;
}

sem_t* openChopstick(int id)
{
    char intBuf[16];
    sprintf(intBuf, "%d", id);
    
    char strBuf[32] = SEM_PREFIX;
    strcat(strBuf, intBuf);
    
    return openSem(strBuf);
}

sem_t* openSem(char* name)
{
    sem_t* ret = sem_open(name, O_CREAT | O_EXCL, 0666, 1);
    if(ret == SEM_FAILED) {
        perror(NULL);
        ret = sem_open(name, 0);
    }
    
    return ret;
}

void closeChopstick(int id, sem_t* sem)
{
    char intBuf[16];
    sprintf(intBuf, "%d", id);
    
    char strBuf[32] = SEM_PREFIX;
    strcat(strBuf, intBuf);
    
    sem_close(sem);
    sem_unlink(strBuf);
}
