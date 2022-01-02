/*
 * Workspace.h
 *
 *  Created on: 31 août 2021
 *      Author: azeddine
 */

#ifndef ICODE_SRC_RESOURCES_WORKSPACE_H_
#define ICODE_SRC_RESOURCES_WORKSPACE_H_
#include "Container.h"
class Workspace: public Container, public virtual IWorkspace {
public:
	static Workspace *currentWorkspace;
	ObjectRef ref;
	WArray<IResource*> resources;
	bool IsResourcesLoaded : 1;
	Workspace(const char *location);
	~Workspace();
	IObject* QueryInterface(const IID Id);
	ObjectRef* GetRef();
	static Workspace* GetCurrentWorkspace();
	int Members(int memberFlags, IResource **&resources);
};

#endif /* ICODE_SRC_RESOURCES_WORKSPACE_H_ */
