/*
 * File.cpp
 *
 *  Created on: 31 août 2021
 *      Author: azeddine
 */

#include "File.h"

File::File(IContainer* parent,const char* name) {
	Init(parent, name);
}

File::~File() {
}

IObject* File::QueryInterface(const IID Id) {
	switch (Id) {
	case IID_IResource:
		return reinterpret_cast<IResource*>(this);
		break;
	default:
		return IObject::QueryInterface(Id);
		break;
	}
}
