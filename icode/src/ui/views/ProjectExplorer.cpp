/*
 * ProjectExplorer.cpp
 *
 *  Created on: 19 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#include "ProjectExplorer.h"

void ProjectExplorer::CreateControl(WComposite *parent) {
	WTreeView::Create(parent, W_HSCROLL | W_VSCROLL | W_MULTI);
	this->SetImageList(IManagers::Get()->GetImageList());
}

WControl* ProjectExplorer::GetPartControl(WComposite *parent) {
	if (!WTreeView::IsOk()) {
		CreateControl(parent);
		test();
	}
	return this;
}
IViewPart* ProjectExplorerHandler() {
	return new ProjectExplorer();
}

const char* ProjectExplorer::GetTitle() {
	return "Project Explorer";
}

void ProjectExplorer::test() {
	WTreeItem root, item, item1;
	char txt[20];
	GetRootItem(root);
	for (int i = 0; i < 5; i++) {
		sprintf(txt, "test %d", i);
		root.AppendItem(item, txt);
		item.SetImage(0);
		for (int j = 0; j < 5; j++) {
			sprintf(txt, "test %d,%d", i, j);
			item.AppendItem(item1, txt).SetImage(1);
		}
	}
}
