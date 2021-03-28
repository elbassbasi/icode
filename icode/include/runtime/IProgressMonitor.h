/*
 * IProgressMonitor.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_RUNTIME_IPROGRESSMONITOR_H_
#define ICODE_INCLUDE_RUNTIME_IPROGRESSMONITOR_H_
#include "../core/core.h"
class ITask;
class IProgressMonitor: public IObject {
public:
	IID_CLASS(IProgressMonitor,0,1,1);
};

#endif /* ICODE_INCLUDE_RUNTIME_IPROGRESSMONITOR_H_ */
