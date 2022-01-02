/*
 * MenuManager.h
 *
 *  Created on: 16 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_ICODE_MENUMANAGER_H_
#define ICODE_INCLUDE_ICODE_MENUMANAGER_H_
#include "Manager.h"
#include <vector>
class IMenu;
class ICommand;
class MenuManager;
class ICODE_PUBLIC CMenu: public WMenuBase {
public:
	IObject *source;
	MenuManager *manager;
	void OnDispose(WEvent &e);
	bool OnHelp(WEvent &e);
	bool OnHide(WEvent &e);
	bool OnShow(WEvent &e);
	bool OnItemArm(WMenuEvent &e);
	bool OnItemHelp(WMenuEvent &e);
	bool OnItemSelection(WMenuEvent &e);
	bool OnItemDispose(WMenuEvent &e);
	bool OnItemAdded(WMenuEvent &e);
};
class MenuManagerItem {
public:
	//MenuManagerItem *parent;
	//MenuManagerItem();
	//~MenuManagerItem();
	int style;
	union {
		IMenu *menu;
		ICommand *command;
	};
	char *commandId;
	char *name;
	WArray<MenuManagerItem> children;
};
class MenuManager: public Manager {
public:
	friend class Managers;
	MenuManager();
	~MenuManager();
	static constexpr const char *STYLE = "style";
	enum {
		M_PUSH = 1, M_RADIO, M_CHECK, M_CASCADE, M_GROUP, M_MASK = 0x07
	};
public:
	static void DoLoad(Config &parent, void *userdata, const char *child);
	bool OnLoad();
	bool OnSave();
	bool OnInstall(Plugin *info, XMLElement *element);
	void CreateMenu(CMenu *menu, IObject *source);
private:
	bool DoSave(Config &parent, MenuManagerItem *root);
	static int GetItemStyle(int style);
	MenuManagerItem* CreateItem(MenuManagerItem *root, const char *location,
			const char *group, int index);
	MenuManagerItem* FindItem(MenuManagerItem *parent, const char *text,
			ushort length);
	void CreateSubMenu(MenuManagerItem *parent, WMenuItem &_p);
	static void FreeItem(w_array *array, void *element, int index);
private:
	MenuManagerItem root;
};
#endif /* ICODE_INCLUDE_ICODE_MENUMANAGER_H_ */
