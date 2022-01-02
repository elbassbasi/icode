/*
 * ViewPart.cpp
 *
 *  Created on: 19 mai 2020
 *      Author: Azeddine El Bassbasi
 */
#include "../../icode.h"
IObject* ViewPart::QueryInterface(const IID Id) {
	if(Id == IID_IViewPart){
		return static_cast<IViewPart*>(this);
	}
	if(Id == IID_IWorkbenchPart){
		return static_cast<IWorkbenchPart*>(this);
	}
	return IObject::QueryInterface(Id);
}
WControl* ViewPart::GetPartControl(WComposite *parent) {
	return 0;
}

void ViewPart::Dispose() {
}

const char* ViewPart::GetTitle() {
	return "View";
}

int ViewPart::GetTitleImage() {
	return 0;
}

const char* ViewPart::GetTitleToolTip() {
	return 0;
}

void ViewPart::SetFocus() {
	WControl* control =  GetPartControl(0);
	if(control != 0){
		control->SetFocus();
	}
}

ObjectRef* ViewPart::GetRef(int *tmp) {
	return &this->ref;
}
