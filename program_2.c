/*
 * program_2.c
 *
 *  Created on: 22 Nov 2017
 *      Author: adtonks
 */

#include "scheduler.h"

/* ends via a main returning */
int main(int argc, char const *argv[]) {
	int i = 0;
	enroll("/tmp/FIFO_pipe");
	for(i=0; ; i += 100) {
		usleep(100000);
		printf("Process %d ran for %d milliseconds\n", getpid(), i);
		if(i == 2500) {
			break;
		}
	}
	return(0);
}
