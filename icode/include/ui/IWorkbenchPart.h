/*
 * IWorkbenchPart.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_UI_IWORKBENCHPART_H_
#define ICODE_INCLUDE_UI_IWORKBENCHPART_H_
#include "../core/core.h"
class IWorkbenchPart: public IObject {
public:
	IID_CLASS_PLATFORM(IWorkbenchPart,2,0);
	virtual WControl* GetPartControl(WComposite *parent) = 0;
	virtual void Dispose()= 0;
	virtual const char* GetTitle()= 0;
	virtual int GetTitleImage()= 0;
	virtual const char* GetTitleToolTip()= 0;
	virtual void SetFocus()= 0;
};
#endif /* ICODE_INCLUDE_UI_IWORKBENCHPART_H_ */
