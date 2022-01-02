/*
 * IViewPart.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_UI_IVIEWPART_H_
#define ICODE_INCLUDE_UI_IVIEWPART_H_
#include "IWorkbenchPart.h"

class IViewPart: public IWorkbenchPart {
public:
};
template<>
inline const IID __IID<IViewPart>() {
	return IID_IViewPart;
}
class ICODE_PUBLIC ViewPart: public IViewPart {
public:
	ObjectRef* GetRef(int *tmp);
	IObject* QueryInterface(const IID Id);
	WControl* GetPartControl(WComposite *parent);
	void Dispose();
	const char* GetTitle();
	int GetTitleImage();
	const char* GetTitleToolTip();
	void SetFocus();
protected:
	ObjectRef ref;
};

#endif /* ICODE_INCLUDE_UI_IVIEWPART_H_ */
