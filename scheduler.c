/*
 * scheduler.c
 *
 *  Created on: 21 Nov 2017
 *      Author: adtonks
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include "scheduler.h"

int check_dequeue(int fd, int stored) {
	int PID, read_suc;
	char buffer[256];
	char *token;
	lseek(fd, 0, SEEK_SET);
	read_suc = read(fd, buffer, 17);
	if(read_suc == 17) {
	token = strtok(buffer, " ");
	/* check that this isn't enroll signal */
	if(strcmp(token, "DEENROLL") == 0) {
		token = strtok(NULL, " ");
		PID = atoi(token);
		printf("Dequeue process %d\n", PID);
		return(PID);
	} else {
		return(0);
	}
	} else {
		return(0);
	}
}

int enqueue(int fd, int stored) {
	int PID, read_suc;
	char buffer[256];
	char *token;
	lseek(fd, 0, SEEK_SET);
	read_suc = read(fd, buffer, 17);
	if(read_suc == 17) {
	token = strtok(buffer, " ");
	/* check that this isn't deenroll signal */
	if(strcmp(token, "XENROLLX") == 0) {
		token = strtok(NULL, " ");
		PID = atoi(token);
		printf("Enqueue process %d\n", PID);
		return(PID);
	} else {
		return(0);
	}
	} else {
		return(0);
	}
}

int main(int argc, char const *argv[]) {
	int i, j, k, stored, curr, PID, PID_proc;
	int fd;
	int rrfifo = 0;
	int rrlifo = 0;
	int rrandom = 0;
	int slots = -1;
	char *fifo_path = "/tmp/FIFO_pipe";
	if(access(fifo_path, F_OK) != -1)
		if(remove(fifo_path) != 0) {
			printf("Please delete the pipe on filesystem\n");
			return(1);
		}
	if(argc != 4) {
		printf("Command line arguments incorrect\n");
		return(1);
	}
	for(i=1; i<4; i++) {
		if (strcmp(argv[i], "-queue") == 0) {
			if(i == 3) {
				printf("Command line arguments incorrect\n");
				return(1);
			} else {
				slots = atoi(argv[i+1]);
				i++;
			}
		} else if(strcmp(argv[i], "-rrfifo") == 0) {
			rrfifo = 1;
		} else if(strcmp(argv[i], "-rrlifo") == 0) {
			rrlifo = 1;
		} else if(strcmp(argv[i], "-rrandom") == 0) {
			rrandom = 1;
		} else {
			printf("Command line arguments incorrect\n");
			return(1);
		}
	}
	if(!(0<slots)) {
		printf("Command line arguments incorrect\n");
		return(1);
	}
	/* create process array */
	int PID_arr[slots];
	for(i=0; i<slots; i++)
		PID_arr[i] = -1;
	stored = 0;
	/* create pipe */
	assert(mkfifo(fifo_path, 0666) == 0);
	fd = open(fifo_path, O_RDONLY | O_NONBLOCK, 0666);
	/* wait for array to be filled */
	printf("Begin process enqueue\n");
	while(stored<slots) {
		if(PID_proc = enqueue(fd, stored)) {
			PID_arr[stored] = PID_proc;
			kill(PID_proc, SIGSTOP);
			stored++;
		}
	}
	printf("Process enqueue complete\n");
	srand(time(NULL));
	while(0<stored) {
		for(i=0; (i<slots) && (0<stored); i++) {
			if(rrfifo)
				curr = i;
			else if(rrlifo){
				curr = slots-1-i;
			}else if(rrandom)
				curr = rand() % slots;
			PID = PID_arr[curr];
			if(PID != -1) {
				/* run process */
				printf("Start process %d\n", PID);
				kill(PID, SIGCONT);
				for(j=0; j<10; j++) {
					usleep(100000);
					/* CHECK FOR DEQUEUE */
					if(PID_proc = check_dequeue(fd, stored)) {
						/* only this process should be asking for deenroll */
						printf("Process %d requests dequeue\n", PID_proc);
						for(k=0; k<slots; k++) {
							if(PID_arr[k] == PID_proc) {
								PID_arr[k] = -1;
								stored--;
								k = -1;
								break;
							}
						}
						if(k == -1) {
							continue;
						} else {
							printf("Deenroll requested by unenrolled process\n");
						}
					}
				}
				printf("Pause process %d\n", PID);
				kill(PID, SIGSTOP);
			}
		}
	}
	unlink(fifo_path);
	close(fd);
	return(0);
}
