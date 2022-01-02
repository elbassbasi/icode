/*
 * ThreadGroup.cpp
 *
 *  Created on: 12 juil. 2021
 *      Author: azeddine
 */
#include "../icode.h"
ThreadGroup0::ThreadGroup0() {
	this->index = 0;
	this->flags = 0;
	this->group = 0;
}

ThreadGroup0::~ThreadGroup0() {
	this->Cancel();
}
ThreadGroup::ThreadGroup() {
	this->threads = 0;
	this->thread_count = 0;
	this->tasks = 0;
}
ThreadGroup::~ThreadGroup() {
	if (this->threads != 0) {
		delete[] this->threads;
	}
}
void ThreadGroup::Create(int thread_count) {
	if (thread_count <= 0) {
		thread_count = w_thread_get_cpu_count();
	}
	this->threads = new ThreadGroup0[thread_count + 1];
	threads[thread_count].index = thread_count;
	/*threads[thread_count].Create(ThreadGroup::thread_control_start, this,
	 &threads[thread_count], 0);*/
	for (int i = 0; i < thread_count; i++) {
		threads[i].index = i;
		threads[i].group = this;
		threads[i].Create(ThreadGroup::thread_start, &threads[i], 0);
	}
}
void ThreadGroup::AddTask(w_thread_start fun, void *args, int numberofThread,
		int flags) {
	ThreadTask *task = new ThreadTask();
	lock.Lock();
	task->next = this->tasks;
	this->tasks = task;
	lock.UnLock();
	task->fun = fun;
	task->numberofThread = numberofThread;
	task->args = args;
	condition.val = 1;
	condition.WakeSingle();
}

int ThreadGroup::thread_control_start(void *args) {
	return 1;
}

int ThreadGroup::thread_start(void *args) {
	ThreadGroup0 *group0 = (ThreadGroup0*) args;
	ThreadGroup *group = group0->group;
	while (1) {
		group->condition.val = 0;
		group->condition.Wait(0);
		ThreadTask *task;
		group->lock.Lock();
		task = group->tasks;
		if (task != 0) {
			group->tasks = task->next;
		}
		group->lock.UnLock();
		if (task != 0) {
			group0->IsRunning = true;
			task->fun(task->args);
			group0->IsRunning = false;
		}
	};
}
