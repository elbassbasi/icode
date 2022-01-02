/*
 * IProgressMonitor.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_RUNTIME_IPROGRESSMONITOR_H_
#define ICODE_INCLUDE_RUNTIME_IPROGRESSMONITOR_H_
#include "../core/core.h"
class IProgressMonitor: public IObject {
public:
	class Task;
	/**
	 * Notifies that the main task is beginning.  This must only be called once
	 * on a given progress monitor instance.
	 *
	 * @param name the name (or description) of the main task
	 * @param totalWork the total number of work units into which
	 *  the main task is been subdivided. If the value is <code>UNKNOWN</code>
	 *  the implementation is free to indicate progress in a way which
	 *  doesn't require the total number of work units in advance.
	 */
	virtual Task* CreateTask(Task *parent, const char *name, int totalWork)=0;
	virtual void CloseTask(Task *task)=0;
	/**
	 * Notifies that the work is done; that is, either the main task is completed
	 * or the user canceled it. This method may be called more than once
	 * (implementations should be prepared to handle this case).
	 */
	virtual void Done(Task *task)=0;
	/**
	 * Returns whether cancelation of current operation has been requested.
	 * Long-running operations should poll to see if cancelation
	 * has been requested.
	 *
	 * @return <code>true</code> if cancellation has been requested,
	 *    and <code>false</code> otherwise
	 * @see #setCanceled(boolean)
	 */
	virtual bool IsCanceled(Task *task)=0;

	/**
	 * Sets the cancel state to the given value.
	 *
	 * @param value <code>true</code> indicates that cancelation has
	 *     been requested (but not necessarily acknowledged);
	 *     <code>false</code> clears this flag
	 * @see #isCanceled()
	 */
	virtual void SetCanceled(Task *task, bool value)=0;
	virtual void SetText(Task *task, const char *text)=0;
	/**
	 * Notifies that a given number of work unit of the main task
	 * has been completed. Note that this amount represents an
	 * installment, as opposed to a cumulative amount of work done
	 * to date.
	 *
	 * @param work a non-negative number of work units just completed
	 */
	virtual void SetWorked(Task *task, int work)=0;
};
template<>
inline const IID __IID<IProgressMonitor>() {
	return IID_IProgressMonitor;
}
#endif /* ICODE_INCLUDE_RUNTIME_IPROGRESSMONITOR_H_ */
