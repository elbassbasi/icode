/*
 * Folder.cpp
 *
 *  Created on: 31 août 2021
 *      Author: azeddine
 */

#include "Folder.h"
#include "File.h"

Folder::Folder(IContainer *parent, const char *name) {
	Init(parent, name);
	this->IsResourcesLoaded = false;
}

Folder::~Folder() {
	// TODO Auto-generated destructor stub
}
int Folder::Members(int memberFlags, IResource **&resources) {
	if (!this->IsResourcesLoaded) {
		WIterator<WFindDir> iter;
		WFindDir file;
		if (WDirList(fullPath, iter)) {
			while (iter.Next(file)) {
				if (file.IsDir()) {
					if (strcmp(file.name, ".") && strcmp(file.name, "..")) {
						IFolder *f = new Folder(this, file.name);
						*this->resources.Add0() = f;
					}
				} else {
					IFile *f = new File(this, file.name);

					*this->resources.Add0() = f;
				}
			}
		}
		this->IsResourcesLoaded = true;
	}
	return this->resources.GetCount(resources);
}

IObject* Folder::QueryInterface(const IID Id) {
	switch (Id) {
	case IID_IContainer:
		return reinterpret_cast<IContainer*>(this);
		break;
	case IID_IResource:
		return reinterpret_cast<IResource*>(this);
		break;
	default:
		return IObject::QueryInterface(Id);
		break;
	}
}
