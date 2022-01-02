/*
 * Resource.cpp
 *
 *  Created on: 31 août 2021
 *      Author: azeddine
 */

#include "Resource.h"

Resource::Resource() {
	this->parent = 0;
	this->fullPath = 0;
}

Resource::~Resource() {
	// TODO Auto-generated destructor stub
}

void Resource::Copy(const char *destination, int flags,
		IProgressMonitor *monitor) {
}

void Resource::Delete(int updateFlags, IProgressMonitor *monitor) {
}

bool Resource::Exists() {
}

const char* Resource::GetFullPath() {
	return this->fullPath;
}

const char* Resource::GetLocation() {
	return this->fullPath;
}

const char* Resource::GetLocationURI() {
	return this->fullPath;
}

IContainer* Resource::GetParent() {
	return this->parent;
}

IProject* Resource::GetProject() {
}

const char* Resource::GetProjectRelativePath() {
}

int Resource::GetResourceAttributes(int mask) {
}

int Resource::GetType() {
}

void Resource::Move(const char *destination, int updateFlags,
		IProgressMonitor *monitor) {
}

void Resource::RefreshLocal(int depth, IProgressMonitor *monitor) {
}

int Resource::SetResourceAttributes(int attributes, int mask,
		IProgressMonitor *monitor) {
}

void Resource::Init(IContainer *parent, const char *name) {
	this->parent = parent;
	const char *pFullPath = parent->GetFullPath();
	int l1 = strlen(pFullPath);
	int l2 = strlen(name);
	this->fullPath =new char[l1 + l2 + 2];
	memcpy(this->fullPath, pFullPath, l1);
	this->fullPath[l1] = '/';
	memcpy(&this->fullPath[l1+1], name, l2);
	this->fullPath[l1 + l2 + 1] = 0;
}

ObjectRef* Resource::GetRef(int* tmp) {
	return &this->ref;
}
