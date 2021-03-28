/*
 * Perspective.h
 *
 *  Created on: 19 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_UI_PERSPECTIVE_H_
#define ICODE_INCLUDE_UI_PERSPECTIVE_H_
#include "IViewPart.h"
class IFolderLayout: public IObject {
public:
	virtual void AddView(const char* viewId)=0;
};

class IPageLayout: public IObject {
public:
	virtual IFolderLayout* CreateFolder(int relationship, int ratio)=0;
	virtual IFolderLayout* GetEditorArea()=0;
};

#endif /* ICODE_INCLUDE_UI_PERSPECTIVE_H_ */
