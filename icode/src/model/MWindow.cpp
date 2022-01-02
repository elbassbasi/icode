/*
 * MWindow.cpp
 *
 *  Created on: 15 avr. 2020
 *      Author: azeddine
 */

#include "MWindow.h"
#include "../icode.h"

MWindow::MWindow() :
		area(this) {
	this->current = 0;
	ClearPreferedSize();
}
void MWindow::ClearPreferedSize() {
	for (int i = 0; i < 4; i++) {
		coolbarPreferedSize[i].width = -1;
		coolbarPreferedSize[i].height = -1;
	}
}
MWindow::~MWindow() {
}
ObjectRef* MWindow::GetRef(int *tmp) {
	return &this->ref;
}
bool MWindow::Create(int style) {
	bool ret = WFrame::Create(style);
	if (ret) {
		menubar.Create(this);
		MenuManager *mainmenu = ICodePlugin::Get()->GetMenuBarManager();
		if (mainmenu != 0) {
			IWindow *window = this;
			mainmenu->CreateMenu(&menubar, window);
		}
		this->SetMenuBar(&menubar);
		LoadPerspective();
	}
	return ret;
}

bool MWindow::OnLayout(WEvent &e) {
	UpdateAll();
	return true;
}

void MWindow::Close() {
	WShell::Close();
}

bool MWindow::LoadPerspective() {
	this->current =
			ICodePlugin::Get()->GetPerspectiveManager()->FindPerspective(
					"Resource");
	if (this->current != 0) {
		PerspectiveFolder *folders;
		int count = this->current->folders.GetCount(folders);
		area.LoadFolders(folders, count);
	}
	return true;
}
void MWindow::GetAreaRect(WRect &r) {
	WSize sz;
	GetClientArea(r);
	if (GetCoolBarSize(MCOOLBAR_TOP, sz)) {
		r.y += sz.height;
		r.height -= sz.height;
	}
	if (GetCoolBarSize(MCOOLBAR_BOTTOM, sz)) {
		r.height -= sz.height;
	}
	if (GetCoolBarSize(MCOOLBAR_LEFT, sz)) {
		r.x += sz.width;
		r.width -= sz.width;
	}
	if (GetCoolBarSize(MCOOLBAR_RIGHT, sz)) {
		r.width -= sz.width;
	}
}
bool MWindow::GetCoolBarSize(int index, WSize &size) {
	if (coolbar[index].IsVisible()) {
		if (coolbarPreferedSize[index].width < 0
				|| coolbarPreferedSize[index].height < 0) {
			coolbar[index].ComputeSize(coolbarPreferedSize[index], W_DEFAULT,
					W_DEFAULT);
		}
		size.width = coolbarPreferedSize[index].width;
		size.height = coolbarPreferedSize[index].height;
		return true;
	}
	return false;
}
void MWindow::UpdateCoolBar() {
	WRect rect;
	WSize sz, defaultSize;
	GetClientArea(rect);
	defaultSize.width = rect.width;
	defaultSize.height = rect.height;
	if (GetCoolBarSize(MCOOLBAR_TOP, sz)) {
		coolbar[MCOOLBAR_TOP].SetBounds(rect.x, rect.y, sz.width, sz.height);
		rect.y += sz.height;
		rect.height -= sz.height;
	}
	if (GetCoolBarSize(MCOOLBAR_BOTTOM, sz)) {
		coolbar[MCOOLBAR_BOTTOM].SetBounds(rect.x,
				defaultSize.height - sz.height, sz.width, sz.height);
		rect.height -= sz.height;
	}
	if (GetCoolBarSize(MCOOLBAR_LEFT, sz)) {
		coolbar[MCOOLBAR_LEFT].SetBounds(rect.x, rect.y, sz.width, sz.height);
		rect.x += sz.width;
		rect.width -= sz.width;
	}
	if (GetCoolBarSize(MCOOLBAR_RIGHT, sz)) {
		coolbar[MCOOLBAR_RIGHT].SetBounds(defaultSize.width - sz.width, rect.y,
				sz.width, sz.height);
		rect.width -= sz.width;
	}
}
void MWindow::UpdateAll() {
	UpdateCoolBar();
	area.UpdateBounds();
}

void MWindow::SwitchToPerspective(PerspectiveItem *perspective) {
	this->current = perspective;
}

IFolderLayout* MWindow::GetEditorArea() {
	return this->area.sharedPart;
}

bool MWindow::SetFullScreen(bool fullScren) {
	return WFrame::SetFullScreen(fullScren);
}
bool MWindow::GetFullScreen() {
	return WFrame::GetFullScreen();
}

IFolderLayout* MWindow::CreateFolder(int relationship, int ratio) {
}
void MWindow::OnDispose(WEvent &e) {
	WFrame::OnDispose(e);
	WApp::Get()->Exit();
}

bool MWindow::OnClose(WEvent &e) {
	Dispose();
	return true;
}
