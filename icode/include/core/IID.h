/*
 * classes.h
 *
 *  Created on: 9 août 2021
 *      Author: azeddine
 */

#ifndef ICODE_INCLUDE_CORE_IID_H_
#define ICODE_INCLUDE_CORE_IID_H_
#include "defs.h"
/**
 * id1 : plugin id : max is 4096
 * id2 : category id : max is 1024
 * id3 : interface id : max is 1024
 */
typedef wuint IID;
#define IID_GEN(id1,id2,id3) ((id1 >> 10) | (id2 >> 10) | (id3))

#define IID_PROPERTY_PLATFORM(id2,id3) IID_GEN(0,id2,id3)
enum {
	IID_IObject = IID_GEN(0, 0, 1), //
	IID_IProgressMonitor, //
	IID_IStartup,
	IID_ICommand,
	/* resource interface */
	IID_IResource = IID_GEN(0, 1, 0), //
	IID_IContainer, //
	IID_IStream, //
	/* Gui interface */
	IID_IWorkbenchPart = IID_GEN(0, 2, 0), //
	IID_IViewPart,
	IID_IDocument, //

};

template<class T>
inline const IID __IID() {
	return 0;
}

#endif /* ICODE_INCLUDE_CORE_IID_H_ */
