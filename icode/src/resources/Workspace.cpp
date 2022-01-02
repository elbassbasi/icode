/*
 * Workspace.cpp
 *
 *  Created on: 31 août 2021
 *      Author: azeddine
 */

#include "Workspace.h"
#include "File.h"
#include "Folder.h"
Workspace *Workspace::currentWorkspace = 0;
IWorkspace* IWorkspace::GetCurrentWorkspace() {
	return Workspace::GetCurrentWorkspace();
}
Workspace::Workspace(const char *location) {
	this->fullPath = strdup(location);
	this->IsResourcesLoaded = 0;
}

Workspace* Workspace::GetCurrentWorkspace() {
	if (Workspace::currentWorkspace == 0) {
		Workspace::currentWorkspace = new Workspace(
				"/home/azeddine/Bureau/IcodeWorkTest");
	}
	return Workspace::currentWorkspace;
}

Workspace::~Workspace() {
	// TODO Auto-generated destructor stub
}

ObjectRef* Workspace::GetRef() {
	return &this->ref;
}

int Workspace::Members(int memberFlags, IResource **&resources) {
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

IObject* Workspace::QueryInterface(const IID Id) {
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
