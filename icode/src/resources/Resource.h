/*
 * Resource.h
 *
 *  Created on: 31 août 2021
 *      Author: azeddine
 */

#ifndef ICODE_SRC_RESOURCES_RESOURCE_H_
#define ICODE_SRC_RESOURCES_RESOURCE_H_
#include "../icode.h"
class Resource: public virtual IResource {
public:
	ObjectRef ref;
	IContainer *parent;
	char *fullPath;
	Resource();
	~Resource();
	void Init(IContainer *parent,const char* name);
public:
	ObjectRef* GetRef(int* tmp);
	void Copy(const char *destination, int flags, IProgressMonitor *monitor);
	void Delete(int updateFlags, IProgressMonitor *monitor);
	bool Exists();
	const char* GetFullPath();
	const char* GetLocation();
	const char* GetLocationURI();
	IContainer* GetParent();
	IProject* GetProject();
	const char* GetProjectRelativePath();
	int GetResourceAttributes(int mask);
	int GetType();
	void Move(const char *destination, int updateFlags,
			IProgressMonitor *monitor);
	void RefreshLocal(int depth, IProgressMonitor *monitor);
	int SetResourceAttributes(int attributes, int mask,
			IProgressMonitor *monitor);
};

#endif /* ICODE_SRC_RESOURCES_RESOURCE_H_ */
