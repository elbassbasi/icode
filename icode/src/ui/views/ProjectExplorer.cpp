/*
 * ProjectExplorer.cpp
 *
 *  Created on: 19 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#include "ProjectExplorer.h"

void ProjectExplorer::CreateControl(WComposite *parent) {
	WTreeView::Create(parent, W_VIRTUAL | W_HSCROLL | W_VSCROLL | W_MULTI);
	this->SetImageList(ICodePlugin::Get()->GetImageList16());
}

WControl* ProjectExplorer::GetPartControl(WComposite *parent) {
	if (!WTreeView::IsOk()) {
		CreateControl(parent);
		SetWorkspace(IWorkspace::GetCurrentWorkspace());
	}
	return this;
}
IObject* ProjectExplorerHandler() {
	return new ProjectExplorer();
}

const char* ProjectExplorer::GetTitle() {
	return "Project Explorer";
}

void ProjectExplorer::SetFocus() {
	WTreeView::ForceFocus();
}
void ProjectExplorer::Expand(WTreeItem &item, IResource *resource) {
	IContainer *container = resource->QueryInterfaceT<IContainer>();
	if (container != 0) {
		WTreeItem _item;
		IResource **resources;
		int length = container->Members(0, resources);
		for (int i = 0; i < length; i++) {
			IResource *_resource = resources[i];
			IContainer *_container = _resource->QueryInterfaceT<IContainer>();
			item.AppendItem(_item);
			_item.SetData(_resource);
			int image;
			if (_container != 0) {
				_item.SetHasChildren();
				image = ICodePlugin::GetImageList16()->AddMimeDirectory();
				_item.SetImage(image);
			} else {
				image = ICodePlugin::GetImageList16()->AddMimeExtension(
						_resource->GetFullPath());
				_item.SetImage(image);
			}
		}
	}
}

bool ProjectExplorer::OnItemExpand(WListEvent &e) {
	IResource *resource = (IResource*) e.item->GetData();
	if (resource == 0)
		return false;
	Expand(*e.GetTreeItem(), resource);
	return true;
}

void ProjectExplorer::SetWorkspace(IWorkspace *workspace) {
	this->workspace = workspace;
	workspace->AddRef();
	WTreeItem root;
	GetRootItem(root);
	Expand(root, workspace);
}

void ProjectExplorer::Refresh(WTreeItem &item, IResource *resource) {
}

void ProjectExplorer::Refresh(WTreeItem &item) {
	IResource *resource = (IResource*) item.GetData();
	Refresh(item, resource);
}

bool ProjectExplorer::OnItemCollapse(WListEvent &e) {
	e.GetTreeItem()->RemoveAll();
	e.GetTreeItem()->SetHasChildren();
	return false;
}

bool ProjectExplorer::OnItemSelection(WListEvent &e) {
}

bool ProjectExplorer::OnItemDefaultSelection(WListEvent &e) {
}

bool ProjectExplorer::OnItemGetText(WListEvent &e) {
	IResource *resource = (IResource*) e.GetTreeItem()->GetData();
	if (resource != 0) {
		if (e.textattr->alloc != 0) {
			e.SetAttrText(resource->GetName());
		}
		e.SetAttrForeground(W_COLOR_BLUE);
	}
	return true;
}

bool ProjectExplorer::OnItemDispose(WListEvent &e) {
	IResource *resource = (IResource*) e.GetTreeItem()->GetData();
	if (resource != 0) {
		resource->Release();
	}
	return true;
}
