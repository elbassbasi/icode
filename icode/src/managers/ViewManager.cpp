/*
 * ViewManager.cpp
 *
 *  Created on: 13 oct. 2021
 *      Author: azeddine
 */

#include "../icode.h"

ViewManager::ViewManager() {
	this->views = 0;
}

ViewManager::ViewManager(const char *id) :
		Manager(id) {
	this->views = 0;
}
ViewManager::~ViewManager() {
}

bool ViewManager::OnInstall(Plugin *info, XMLElement *element) {
	XMLElement *child = element->FirstChildElement();
	while (child != 0) {
		if (!strcmp(child->Value(), "view")) {
			const char *_id = 0, *_handler = 0;
			const XMLAttribute *attr = child->FindAttribute("id");
			if (attr != 0) {
				_id = attr->Value();
			}
			if (_id != 0) {
				Handler handler = 0;
				attr = child->FindAttribute("handler");
				if (attr != 0) {
					_handler = attr->Value();
					//handler = Managers::Get()->FindHandler(_handler);
				}
				if (_handler != 0) {
					StringId *id = Managers::RegistreString(_id);
					if (id != 0) {
						ViewManagerItem *item = NewItem(id);
						if (item != 0) {
							item->key = id;
							item->handlerName = strdup(_handler);
							item->handler = handler;
						}
					}
				}
			}
		}
		child = child->NextSiblingElement();
	}
	return true;
}

IObject* ViewManager::CreateView(StringId *id) {
	ViewManagerItem *item = FindItem(id);
	if (item != 0) {
		if (item->ishandlerLoaded == 0) {
			item->handler = Managers::Get()->FindHandler(item->handlerName);
			item->ishandlerLoaded = 1;
		}
		if (item->handler == 0)
			return 0;
		return item->handler();
	}
	return 0;
}
void ViewManager::LoadViews(Config &parent, void *userdata, const char *child) {
	StringId *id = Managers::RegistreString(child);
	if (id != 0) {
		ViewManager *manager = (ViewManager*) userdata;
		ViewManagerItem *item = manager->NewItem(id);
		if (item != 0) {
			item->key = id;
			item->handler = 0;
			item->handlerName = 0;
			Config c;
			parent.Open(child, c);
			if (c.IsOk()) {
				item->handlerName = c.Open("handler").GetString();
			}
		}
	}
}
bool ViewManager::OnLoad() {
	this->config.ForAll(LoadViews, this);
	return true;
}

bool ViewManager::OnSave() {
	Config c;
	AVLNode *view = this->views->GetFirst();
	while (view != 0) {
		ViewManagerItem *_view = static_cast<ViewManagerItem*>(view);
		this->config.CreateDir(_view->key->name, c);
		if (c.IsOk()) {
			c.CreateString("handler").SetString(_view->handlerName);
		}
		view = view->GetNext();
	}
	return true;
}

ViewManagerItem* ViewManager::NewItem(StringId *id) {
	AVLNode *node = this->views->Find(id);
	if (node != 0)
		return 0;
	ViewManagerItem *item = new ViewManagerItem();
	node = this->views->Insert(item);
	this->views = static_cast<ViewManagerItem*>(node);
	item->key = id;
	return item;
}

ViewManagerItem* ViewManager::FindItem(StringId *id) {
	AVLNode *node = this->views->Find(id);
	if (node != 0)
		return static_cast<ViewManagerItem*>(node);
	else
		return 0;
}

ViewManagerItem::ViewManagerItem() {
	this->handler = 0;
	this->handlerName = 0;
	this->ishandlerLoaded = false;
}

ViewManagerItem::~ViewManagerItem() {
}
