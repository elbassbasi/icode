/*
 * IWorkspace.h
 *
 *  Created on: 31 août 2021
 *      Author: azeddine
 */

#ifndef ICODE_INCLUDE_RESOURCES_IWORKSPACE_H_
#define ICODE_INCLUDE_RESOURCES_IWORKSPACE_H_
#include "IContainer.h"

class IWorkspace: public virtual IContainer {
public:
	static IWorkspace* GetCurrentWorkspace();
};

#endif /* ICODE_INCLUDE_RESOURCES_IWORKSPACE_H_ */
