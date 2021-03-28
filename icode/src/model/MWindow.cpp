/*
 * MWindow.cpp
 *
 *  Created on: 15 avr. 2020
 *      Author: azeddine
 */

#include "MWindow.h"
#include "../icode.h"

MWindow::MWindow() {
	this->perspective = 0;
}

MWindow::~MWindow() {
}

bool MWindow::Create(int style) {
	bool ret = WShell::Create(style);
	if (ret) {
		//LoadPerspective();
		menubar.Create(this);
		MenuManager *mainmenu = ICodePlugin::Get()->GetMainMenuManager();
		if (mainmenu != 0) {
			mainmenu->CreateMenu(&menubar, this);
		}
		this->SetMenuBar(&menubar);
	}
	return ret;
}

bool MWindow::OnLayout(WEvent &e) {
	UpdateLayout();
	return true;
}

void MWindow::Close() {
	WShell::Close();
}

bool MWindow::LoadPerspective() {
	ICodePlugin::Get()->OpenPerspective(this, "Resource");
	return true;
}

bool MWindow::OnNotify(WEvent &e) {
	return perspective->OnNotify(e);
}

void MWindow::UpdateLayout() {
	if (perspective != 0) {
		WRect r;
		GetClientArea(r);
		perspective->UpdateBounds(r);
	}
}

void MWindow::test() {
	perspective->test();
}

void MWindow::SwitchToPerspective(MPerspective *perspective) {
	this->perspective = perspective;
}
