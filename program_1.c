/*
 * program_1.c
 *
 *  Created on: 22 Nov 2017
 *      Author: adtonks
 */

#include "scheduler.h"

/* ends via a deenroll */
int main(int argc, char const *argv[]) {
	int i = 0;
	int fd = enroll("/tmp/FIFO_pipe");
	for(i=0; ; i += 100) {
		usleep(100000);
		printf("Process %d ran for %d milliseconds\n", getpid(), i);
		if(i == 3000) {
			deenroll(fd);
			sleep(1);
			break;
		}
	}
	return(0);
}
