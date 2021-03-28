/*
 * MenuManager.cpp
 *
 *  Created on: 16 mai 2020
 *      Author: Azeddine El Bassbasi
 */
#include "../icode.h"
#include "Managers.h"

void CMenu::OnDispose(WEvent &e) {
	WMenuBase::OnDispose(e);
}

bool CMenu::OnHelp(WEvent &e) {
	return false;
}

bool CMenu::OnHide(WEvent &e) {
	return false;
}

bool CMenu::OnShow(WEvent &e) {
	return false;
}

bool CMenu::OnItemArm(WMenuEvent &e) {
	return false;
}

bool CMenu::OnItemHelp(WMenuEvent &e) {
	return false;
}

bool CMenu::OnItemSelection(WMenuEvent &e) {
	MenuManagerItem *item = (MenuManagerItem*) e.item->GetData();
	if (item != 0) {
		if ((item->style & W_CASCADE) == 0 && item->command != 0) {
			ExecutionEvent e;
			e.source = this->source;
			return item->command->Execute(e);
		}
	}
	return false;
}

bool CMenu::OnItemDispose(WMenuEvent &e) {
	return false;
}

bool CMenu::OnItemAdded(WMenuEvent &e) {
	return false;
}

MenuManager::MenuManager() {
	memset(&this->root, 0, sizeof(MenuManagerItem));
	this->root.manager = this;
	this->id = 0;
}

MenuManager::~MenuManager() {
	FreeChildren(&this->root);
	if (this->id != 0) {
		free(this->id);
	}
}

void MenuManager::AddItemCommand(const char *location, ICommand *command, int style) {
	MenuManagerItem *item = CreateItem(location);
	if (item != 0) {
		style &= ~W_CASCADE;
		int mask = W_PUSH | W_RADIO | W_SEPARATOR | W_CHECK;
		if ((style & mask) == 0)
			style = W_PUSH;
		item->style = style;
		item->command = command;
	}
}

void MenuManager::AddItem(const char *location, const char *command,
		int style) {
	ICommand *_command = IManagers::Get()->FindCommand(command);
	AddItemCommand(location, _command, style);
}
void MenuManager::AddMenu(const char *location, IMenu *menu) {
	MenuManagerItem *item = CreateItem(location);
	if (item != 0) {
		item->style = W_CASCADE;
		item->menu = menu;
	}
}
MenuManagerItem* MenuManager::CreateItem(const char *location) {
	int start = 0, i = 0;
	MenuManagerItem *dir = &this->root;
	while (true) {
		if (location[i] == '/' || location[i] == '\\') {
			dir = FindItem(dir, &location[start], i - start);
			if (dir == 0)
				return 0;
			start = i + 1;
		}
		if (location[i] == 0) {
			dir = FindItem(dir, &location[start], i - start);
			if (dir == 0)
				return 0;
			dir->style = W_PUSH;
			return dir;
			break;
		}
		i++;
	}
}
MenuManagerItem* MenuManager::CreateItem(MenuManagerItem *parent,
		const char *text, ushort length) {
	MenuManagerItem *item = (MenuManagerItem*) malloc(
			sizeof(MenuManagerItem) + length + 1);
	if (item == 0)
		return 0;
	memset(item, 0, sizeof(MenuManagerItem));
	item->parent = parent;
	item->manager = parent->manager;
	item->name_length = length;
	memcpy(item->name, text, length);
	item->name[length] = 0;
	parent->Add(item);
	return item;
}

MenuManagerItem* MenuManager::FindItem(MenuManagerItem *parent,
		const char *text, ushort length) {
	MenuManagerItem *dir, *last = 0;
	dir = (MenuManagerItem*) parent->first;
	int i;
	while (dir) {
		if (dir->name_length == length) {
			for (i = 0; i < length; i++) {
				if (dir->name[i] != text[i])
					break;
			}
			if (i >= length) {
				return dir;
			}
		}
		last = dir;
		dir = (MenuManagerItem*) dir->next;
	}
	dir = CreateItem(parent, text, length);
	if (dir == 0)
		return 0;
	dir->style = W_CASCADE;
	return dir;
}
void MenuManager::CreateMenu(CMenu *menu, IObject *source) {
	menu->source = source;
	WMenuItem root;
	menu->GetRoot(root);
	if (this->root.first != 0)
		CreateSubMenu((MenuManagerItem*) this->root.first, root);
}
void MenuManager::CreateSubMenu(MenuManagerItem *firstchild, WMenuItem &_p) {
	MenuManagerItem *dir = firstchild;
	WMenuItem item;
	while (dir != 0) {
		_p.Append(item, dir->name, dir->style);
		item.SetData(dir);
		if (dir->first != 0) {
			CreateSubMenu((MenuManagerItem*) dir->first, item);
		}
		dir = (MenuManagerItem*) dir->next;
	}
}

void MenuManager::FreeChildren(MenuManagerItem *item) {
	MenuManagerItem *dir = (MenuManagerItem*) item->first, *prev;
	while (dir != 0) {
		prev = dir;
		dir = (MenuManagerItem*) dir->next;
		FreeItem(prev);
	}
}

void MenuManager::FreeItem(MenuManagerItem *item) {
	FreeChildren(item);
	if (item->command != 0) {
		item->command->DecRef();
	}
	free(item);
}

