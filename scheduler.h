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

int enroll(const char *pipe) {
	int fd = open(pipe, O_WRONLY, 0666);
	int PID = getpid();
	int buff_len;
	char buffer[256];
	sprintf(buffer, "XENROLLX %06d", PID);
	printf("Enrolling process %d\n", PID);
	write(fd, buffer, 17);
}

void deenroll(int fd) {
	int PID = getpid();
	int buff_len;
	char buffer[256];
	sprintf(buffer, "DEENROLL %06d\n", PID);
	buff_len = strlen(buffer)+1;
	write(PID, buffer, buff_len);
	close(fd);
}

#endif /* SCHEDULER_H_ */