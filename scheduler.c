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

int check_dequeue(int PID_arr[], int fd, int stored) {
	printf("dequeue\n");
}

int enqueue(int PID_arr[], int fd, int stored) {
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
		return(1);
	} else {
		return(0);
	}
	} else {
		return(0);
	}
}

int main(int argc, char const *argv[]) {
	int i, j, stored, curr;
	int fd;
	int rrfifo = 0;
	int rrlifo = 0;
	int rrandom = 0;
	int slots = -1;
	char *fifo_path = "/tmp/FIFO_pipe";
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
			rrfifo = 1;
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
		PID_arr[i] = 541;
	stored = 0;
	/* create pipe */
	assert(mkfifo(fifo_path, 0666) == 0);
	fd = open(fifo_path, O_RDONLY, 0666);
	/* wait for array to be filled */
	while(stored<4) {
		if(enqueue(PID_arr, fd, stored))
			stored++;
	}
	/*
	while(enqueue(PID_arr, fd, stored)) {
		sleep(1);
	}
	*/
	/* run the scheduler */
	srand(time(NULL));
	while(0<stored) {
		for(i=0; (i<slots) && (0<stored); i++) {
			if(rrfifo)
				curr = i;
			else if(rrlifo)
				curr = slots-1-i;
			else if(rrandom)
				curr = rand() % slots;
			if(PID_arr[curr] != -1) {
				/* run process */
				for(j=0; j<10; j++) {
				usleep(800000);
				/* CHECK FOR DEQUEUE */
				if(check_dequeue(PID_arr, fd, stored))
					break;
				}
			}
		}
	}
	unlink(fifo_path);
	close(fd);
	return(0);
}
