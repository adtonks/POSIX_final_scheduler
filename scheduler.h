/*
 * scheduler.h
 *
 *  Created on: 22 Nov 2017
 *      Author: adtonks
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int check_dequeue(int fd, int stored);
int enqueue(int fd, int stored);
int check_no(const char *input);

int enroll(const char *pipe) {
	int fd = open(pipe, O_WRONLY, 0666);
	int PID = getpid();
	char buffer[256];
	if(fd == -1) {
		printf("Cannot open pipe\n");
		return(-1);
	}
	sprintf(buffer, "XENROLLX %06d", PID);
	printf("Enrolling process %d\n", PID);
	write(fd, buffer, 16);
	/* sleep until SIGSTOP is received */
	usleep(10000);
	return(fd);
}

void deenroll(int fd) {
	int PID = getpid();
	char buffer[256];
	sprintf(buffer, "DEENROLL %06d", PID);
	printf("Deenrolling process %d\n", PID);
	write(fd, buffer, 16);
	close(fd);
	sleep(1);
	return;
}

#endif /* SCHEDULER_H_ */
