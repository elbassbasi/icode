/*
 * ViewPart.cpp
 *
 *  Created on: 19 mai 2020
 *      Author: Azeddine El Bassbasi
 */
#include "../icode.h"

int ViewPart::IncRef() {
	return IObject::IncRef(this->ref);
}

int ViewPart::DecRef() {
	return IObject::DecRef(this->ref);
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
