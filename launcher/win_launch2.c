/*
 * start_win.c
 *
 *  Created on: 2 déc. 2020
 *      Author: azeddine
 */
#if defined( __WIN32__) || defined(__WIN64__)
#include <stdio.h>
#include <stdlib.h>

extern int IManagersStart(int argc, char **argv);
int main(int argc, char* argv[]) {
	return IManagersStart(argc,argv);
}
#endif
