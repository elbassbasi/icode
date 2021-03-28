/*
 * MenuManager.h
 *
 *  Created on: 16 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_ICODE_MENUMANAGER_H_
#define ICODE_INCLUDE_ICODE_MENUMANAGER_H_
#include "../core/core.h"
class IMenu;
class ICommand;
class ICODE_PUBLIC CMenu: public WMenuBase {
public:
	IObject *source;
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
class MenuManager;
class MenuManagerItem: public BasicNode {
public:
	MenuManager *manager;
	MenuManagerItem *parent;
	union {
		IMenu *menu;
		ICommand *command;
	};
	int style;
	ushort name_length;
	char name[0];
};
class MenuManager {
public:
	friend class Managers;
	MenuManager();
	~MenuManager();
public:
	void AddItemCommand(const char *location, ICommand *command, int style);
	void AddItem(const char *location, const char *command, int style);
	void AddMenu(const char *location, IMenu *menu);
	void CreateMenu(CMenu *menu, IObject *source);
private:
	MenuManagerItem* CreateItem(const char *location);
	MenuManagerItem* CreateItem(MenuManagerItem *parent, const char *text,
			ushort length);
	MenuManagerItem* FindItem(MenuManagerItem *parent, const char *text,
			ushort length);
	void CreateSubMenu(MenuManagerItem *firstchild, WMenuItem &_p);
	void FreeChildren(MenuManagerItem *item);
	void FreeItem(MenuManagerItem *item);
private:
	MenuManagerItem root;
public:
	char *id;
};
#endif /* ICODE_INCLUDE_ICODE_MENUMANAGER_H_ */
