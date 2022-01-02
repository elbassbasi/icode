/*
 * linux_launch2.c
 *
 *  Created on: 2 déc. 2020
 *      Author: azeddine
 */
#ifdef __linux
#include <stdio.h>
#include <stdlib.h>

extern int ManagersStart(int argc, char **argv);
int main(int argc, char* argv[]) {
	return ManagersStart(argc,argv);
}
#endif
