/*
 * start_macos.c
 *
 *  Created on: 2 déc. 2020
 *      Author: azeddine
 */
#ifdef __APPLE__
#include <stdio.h>
#include <stdlib.h>

extern int IManagersStart(int argc, char **argv);
int main(int argc, char* argv[]) {
	return IManagersStart(argc,argv);
}
#endif

