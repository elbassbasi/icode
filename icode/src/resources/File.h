/*
 * File.h
 *
 *  Created on: 31 août 2021
 *      Author: azeddine
 */

#ifndef ICODE_SRC_RESOURCES_FILE_H_
#define ICODE_SRC_RESOURCES_FILE_H_
#include "Resource.h"
class File: public Resource, public virtual IFile {
public:
	File(IContainer* parent,const char* name);
	~File();
	IObject* QueryInterface(const IID Id);
};

#endif /* ICODE_SRC_RESOURCES_FILE_H_ */
