/*
 * Container.cpp
 *
 *  Created on: 31 août 2021
 *      Author: azeddine
 */

#include "Container.h"

Container::Container() {
	// TODO Auto-generated constructor stub

}

Container::~Container() {
	// TODO Auto-generated destructor stub
}

bool Container::Exists(const char *path) {
}

IResource* Container::FindMember(const char *path, bool includePhantoms) {
}

IFile* Container::GetFile(const char *path) {
}

IFolder* Container::GetFolder(const char *path) {
}

int Container::Members(int memberFlags, IResource **&resources) {
}

void Container::ReleaseMembers() {
}
