/*
 * MPerspective.cpp
 *
 *  Created on: 12 avr. 2020
 *      Author: azeddine
 */

#include "MPerspective.h"
#include "MWindow.h"

MPerspective::MPerspective(MWindow *parent) :
		area(this) {
	this->parent = parent;
}

MPerspective::~MPerspective() {
}
bool MPerspective::Create(WComposite *parent) {
	WComposite::Create(parent, W_NONE);
	area.CreateSharedPart();
	//coolbar[MCOOLBAR_TOP].Create(this, W_NONE);
	test();
	return true;
}
WComposite* MPerspective::GetComposite() {
	return this;
}

void MPerspective::GetPartBounds(WRect &r) {
	this->GetClientArea(r);
}

void MPerspective::UpdateBounds(WRect &r) {
	SetBounds(r);
	WRect rect(r);
	WSize sz;
	if (coolbar[MCOOLBAR_TOP].IsVisible()) {
		coolbar[MCOOLBAR_TOP].ComputeSize(sz, W_DEFAULT, W_DEFAULT);
		coolbar[MCOOLBAR_TOP].SetBounds(rect.x, rect.y, sz.width, sz.height);
		rect.y += sz.height;
		rect.height -= sz.height;
	}
	if (coolbar[MCOOLBAR_BOTTOM].IsVisible()) {
		coolbar[MCOOLBAR_BOTTOM].ComputeSize(sz, W_DEFAULT, W_DEFAULT);
		coolbar[MCOOLBAR_BOTTOM].SetBounds(rect.x, r.height - sz.height,
				sz.width, sz.height);
		rect.height -= sz.height;
	}
	if (coolbar[MCOOLBAR_LEFT].IsVisible()) {
		coolbar[MCOOLBAR_LEFT].ComputeSize(sz, W_DEFAULT, W_DEFAULT);
		coolbar[MCOOLBAR_LEFT].SetBounds(rect.x, rect.y, sz.width, sz.height);
		rect.x += sz.width;
		rect.width -= sz.width;
	}
	if (coolbar[MCOOLBAR_RIGHT].IsVisible()) {
		coolbar[MCOOLBAR_RIGHT].ComputeSize(sz, W_DEFAULT, W_DEFAULT);
		coolbar[MCOOLBAR_RIGHT].SetBounds(r.width - sz.width, rect.y, sz.width,
				sz.height);
		rect.width -= sz.width;
	}
	area.UpdateBounds(rect);
}

void MPerspective::GetAreaBounds(WRect &r) {
	this->GetClientArea(r);
}

bool MPerspective::UpdateSash(WSashEvent *event) {
	return area.UpdateSash(event);
}

bool MPerspective::OnNotify(WEvent &e) {
	if (WSash::IsSash(e.widget)) {
		return UpdateSash(reinterpret_cast<WSashEvent*>(&e));
	}
	return false;
}

void MPerspective::UpdateLayout() {
	parent->UpdateLayout();
}
void ResourcePerspectiveInit(IPageLayout *layout) {
	IFolderLayout *folder = layout->CreateFolder(W_LEFT, 20);
	folder->AddView("ProjectExplorer");
}
void MPerspective::test() {
	coolbar[MCOOLBAR_TOP].Create(this->GetComposite(), W_HORIZONTAL);
	testcoolbar(&coolbar[MCOOLBAR_TOP], W_HORIZONTAL);
	coolbar[MCOOLBAR_BOTTOM].Create(this->GetComposite(), W_HORIZONTAL);
	testcoolbar(&coolbar[MCOOLBAR_BOTTOM], W_HORIZONTAL);
	coolbar[MCOOLBAR_LEFT].Create(this->GetComposite(), W_VERTICAL);
	testcoolbar(&coolbar[MCOOLBAR_LEFT], W_VERTICAL);
	coolbar[MCOOLBAR_RIGHT].Create(this->GetComposite(), W_VERTICAL);
	testcoolbar(&coolbar[MCOOLBAR_RIGHT], W_VERTICAL);
	area.test();
}

void MPerspective::testcoolbar(WCoolBar *coolbar, int style) {
	WCoolItem item;
	WToolItem ti;
	WSize sz;
	WToolBar *toolbar;
	for (int idxCoolItem = 0; idxCoolItem < 3; ++idxCoolItem) {
		coolbar->AppendItem(item);
		toolbar = new WToolBar();
		toolbar->Create(coolbar, style | W_FREE_MEMORY);
		toolbar->SetImageList(IManagers::Get()->GetImageList());
		toolbar->Append(ti, 0, W_PUSH).SetImage(0);
		toolbar->Append(ti, 0, W_PUSH).SetImage(1);
		toolbar->AppendSeparator(ti);
		toolbar->Append(ti, 0, W_CHECK).SetImage(3);
		toolbar->Append(ti, 0, W_CHECK).SetImage(4);
		/** resize to toolbar */
		toolbar->ComputeSize(sz, W_DEFAULT, W_DEFAULT);
		toolbar->SetSize(sz);
		item.ComputeSize(sz, sz.width, sz.height);
		item.SetControl(toolbar);
		item.SetSize(sz);
	}
}

IFolderLayout* MPerspective::GetEditorArea() {
	return this->area.sharedPart;
}

IFolderLayout* MPerspective::CreateFolder(int relationship, int ratio) {
	MPartStack *p, *shared = area.GetSharedPart();
	int flags = 0;
	if (relationship & W_LEFT) {
		flags = MPART_FIRST | MPART_VERTICAL;
	}
	if (relationship & W_RIGHT) {
		flags = MPART_LAST | MPART_VERTICAL;
	}
	if (relationship & W_TOP) {
		flags = MPART_FIRST | MPART_HORIZENTAL;
	}
	if (relationship & W_BOTTOM) {
		flags = MPART_LAST | MPART_HORIZENTAL;
	}
	p = area.Div(shared, flags);
	if (p != 0) {
		p->SetWeight(ratio);
	}
	return p;
}
