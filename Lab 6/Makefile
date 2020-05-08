# Name: Andrew Miner
# Project: Lab 6

# This is a Makefile for my lab assignment. It allows me to easily and accurately rebuild
# the lab assignment.

# EXECUTABLES is a Make Variable (similar to an environment variable) that lists all the
# "executables" associated with the lab.
EXECUTABLES=spiritd mole
OBJS=spiritd.o mole.o

# The CC and CFLAGS variables are defined to ensure I'm using the native compiler and
# that the correct command-line options are set
CC=gcc
CFLAGS=-g -Wall -Wextra

# Here is my default target for the Makefile.  When I type make, this first target will
# be created. I.e., all the executables
all: $(EXECUTABLES)


# On the following lines (which _must_ start with a tab), are the commands I need to type -- 
# each time my source file is modified.
spiritd: spiritd.c
	$(CC) $(CFLAGS) -c spiritd.c
	$(CC) $(CFLAGS) -o spiritd spiritd.o
	
mole: mole.c
	$(CC) $(CFLAGS) -c mole.c
	$(CC) $(CFLAGS) -o mole mole.o

# By convention, I always have a target called
# "clean" that I used to define all temporary
# or intermediates.
clean:
	rm -f $(EXECUTABLES) $(OBJS) *~
