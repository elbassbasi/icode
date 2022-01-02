/*
 * ViewManager.h
 *
 *  Created on: 13 oct. 2021
 *      Author: azeddine
 */

#ifndef ICODE_INCLUDE_MANAGERS_VIEWMANAGER_H_
#define ICODE_INCLUDE_MANAGERS_VIEWMANAGER_H_
#include "Managers.h"
class ViewManagerItem: public AVLPtrKey<StringId> {
public:
	ViewManagerItem();
	~ViewManagerItem();
	unsigned ishandlerLoaded :1;
	char *handlerName;
	Handler handler;
};
class ViewManager: public Manager {
public:
	ViewManagerItem* views;
	ViewManager();
	ViewManager(const char *id);
	~ViewManager();
	bool OnInstall(Plugin *info, XMLElement *element);
	IObject* CreateView(StringId *id);
	IObject* CreateView(const char *id) {
		return CreateView(Managers::FindString(id));
	}
protected:
	ViewManagerItem* NewItem(StringId* id);
	ViewManagerItem* FindItem(StringId* id);
	static void LoadViews(Config &parent, void *userdata, const char *child);
	bool OnLoad();
	bool OnSave();
};

#endif /* ICODE_INCLUDE_MANAGERS_VIEWMANAGER_H_ */
