/*
 * ThreadGroup.h
 *
 *  Created on: 12 juil. 2021
 *      Author: azeddine
 */

#ifndef ICODE_INCLUDE_RUNTIME_THREADGROUP_H_
#define ICODE_INCLUDE_RUNTIME_THREADGROUP_H_
#include "IProgressMonitor.h"
class ThreadGroup;
class ThreadTask {
public:
	ThreadTask *next;
	w_thread_start fun;
	void *args;
	int numberofThread;
};

class ThreadGroup0: public WThread {
public:
	ThreadGroup0();
	~ThreadGroup0();
	ThreadGroup *group;
	int index;
	union {
		struct {
			unsigned IsRunning :1;

		};
		int flags;
	};
};

class ThreadGroup {
protected:
	static int thread_control_start(void *args);
	static int thread_start(void *args);
public:
	int thread_count;
	ThreadGroup0 *threads;
	ThreadTask *tasks;
	WFutex lock;
	WFutex condition;
	ThreadGroup();
	~ThreadGroup();
	void Create(int thread_count);
	void Create() {
		Create(0);
	}
	void AddTask(w_thread_start fun, void *args, int numberofThread, int flags);
};

#endif /* ICODE_INCLUDE_RUNTIME_THREADGROUP_H_ */
