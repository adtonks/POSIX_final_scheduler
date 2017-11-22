/*
 * pipe_buf_print.c
 *
 *  Created on: 22 Nov 2017
 *      Author: adtonks
 */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

int main(int argc, char const *argv[]) {
	printf("The size of PIPE_BUF (in bytes) on this system is %d\n", PIPE_BUF);
	return(0);
}
