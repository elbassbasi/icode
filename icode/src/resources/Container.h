/*
 * Container.h
 *
 *  Created on: 31 août 2021
 *      Author: azeddine
 */

#ifndef ICODE_SRC_RESOURCES_CONTAINER_H_
#define ICODE_SRC_RESOURCES_CONTAINER_H_
#include "Resource.h"
class Container: public Resource, public virtual IContainer {
public:
	Container();
	~Container();
public:
	bool Exists(const char *path);
	IResource* FindMember(const char *path, bool includePhantoms);
	IFile* GetFile(const char *path);
	IFolder* GetFolder(const char *path);
	int Members(int memberFlags, IResource **&resources);
	void ReleaseMembers();
};

#endif /* ICODE_SRC_RESOURCES_CONTAINER_H_ */
