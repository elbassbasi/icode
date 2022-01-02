/*
 * Folder.h
 *
 *  Created on: 31 août 2021
 *      Author: azeddine
 */

#ifndef ICODE_SRC_RESOURCES_FOLDER_H_
#define ICODE_SRC_RESOURCES_FOLDER_H_
#include "Container.h"
class Folder: public Container, public virtual IFolder {
public:
	WArray<IResource*> resources;
	bool IsResourcesLoaded : 1;
	Folder(IContainer* parent,const char* name);
	~Folder();
	IObject* QueryInterface(const IID Id);
	int Members(int memberFlags, IResource **&resources);
};

#endif /* ICODE_SRC_RESOURCES_FOLDER_H_ */
