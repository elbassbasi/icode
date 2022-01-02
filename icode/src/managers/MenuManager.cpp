/*
 * MenuManager.cpp
 *
 *  Created on: 16 mai 2020
 *      Author: Azeddine El Bassbasi
 */
#include "../icode.h"

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
	MenuManagerItem *item = (MenuManagerItem*) WWidget::GetItemData(e.item);
	if (item != 0) {
		if ((item->style & MenuManager::M_MASK) != MenuManager::M_CASCADE) {
			if (item->command == 0) {
				item->command =
						ICodePluginGet()->GetCommandManager()->FindCommand(
								item->commandId);
			}
			if (item->command != 0) {
				ExecutionEvent e;
				e.source = this->source;
				return item->command->Execute(e);
			}
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

	this->id = 0;
}

MenuManager::~MenuManager() {
	w_array_free(root.children.array, sizeof(MenuManagerItem),
			MenuManager::FreeItem);
	if (this->id != 0) {
		free(this->id);
	}
}
MenuManagerItem* MenuManager::CreateItem(MenuManagerItem *root,
		const char *location, const char *group, int index) {
	int start = 0, i = 0;
	MenuManagerItem *p = root, *item;
	while (true) {
		if (location[i] == '/' || location[i] == '\\') {
			item = FindItem(p, &location[start], i - start);
			if (item == 0) {
				item = p->children.Add0();
				if (item == 0)
					return 0;
				item->name = strndup(&location[start], i - start);
				item->style = M_CASCADE;
			}
			p = item;
			start = i + 1;
		}
		if (location[i] == 0) {
			MenuManagerItem *items;
			int count = p->children.GetCount(items);
			const char *name = &location[start];
			const int namelength = i - start;
			item = 0;
			if (group != 0) {
				int grouplength = strlen(group);
				MenuManagerItem *_p = 0;
				for (int j = 0; j < count; j++) {
					if ((items[i].style & M_MASK) == M_GROUP) {
						if (!strncmp(items[i].name, group, grouplength)) {
							_p = &items[i];
							break;
						}
					}
				}
				if (_p == 0) {
					_p = p->children.Add0();
					if (_p == 0)
						return 0;
					_p->name = strdup(group);
					_p->style = M_GROUP;
				}
				p = _p;
			}
			if (item == 0)
				item = p->children.Add0(index);
			if (item == 0)
				return 0;
			item->name = strndup(name, namelength);
			return item;
			break;
		}
		i++;
	}
}
MenuManagerItem* MenuManager::FindItem(MenuManagerItem *parent,
		const char *text, ushort length) {
	MenuManagerItem *items;
	int count = parent->children.GetCount(items);
	for (int i = 0; i < count; i++) {
		if ((items[i].style & M_MASK) == M_GROUP) {
			MenuManagerItem *groups;
			int groups_count = items[i].children.GetCount(groups);
			for (int j = 0; j < groups_count; j++) {
				if (!strncmp(groups[j].name, text, length))
					return &groups[j];
			}
		}
		if (!strncmp(items[i].name, text, length))
			return &items[i];
	}
	return 0;
}
int MenuManager::GetItemStyle(int style) {
	switch (style & M_MASK) {
	case M_PUSH:
		return W_PUSH;
		break;
	case M_CHECK:
		return W_CHECK;
		break;
	case M_CASCADE:
		return W_CASCADE;
		break;
	default:
		return W_PUSH;
		break;
	}
}
void MenuManager::CreateMenu(CMenu *menu, IObject *source) {
	menu->source = source;
	WMenuItem root;
	menu->GetRoot(root);
	CreateSubMenu(&this->root, root);
}
void MenuManager::CreateSubMenu(MenuManagerItem *parent, WMenuItem &_p) {
	MenuManagerItem *items, *groups;
	WMenuItem item;
	int count = parent->children.GetCount(items);
	int prev_style = 0;
	for (int i = 0; i < count; i++) {
		if ((items[i].style & M_MASK) == M_GROUP) {
			if (prev_style != 0)
				_p.AppendSeparator();
			int groupcount = items[i].children.GetCount(groups);
			for (int j = 0; j < groupcount; j++) {
				_p.Append(item, groups[j].name, GetItemStyle(groups[j].style));
				WWidget::SetItemData(&item, &groups[j]);
				CreateSubMenu(&groups[j], item);
			}
		} else {
			if ((prev_style & M_MASK) == M_GROUP)
				_p.AppendSeparator();
			_p.Append(item, items[i].name, GetItemStyle(items[i].style));
			WWidget::SetItemData(&item, &items[i]);
			CreateSubMenu(&items[i], item);
		}
		prev_style = items[i].style;
	}
}
void MenuManager::FreeItem(w_array *array, void *element, int index) {
	MenuManagerItem *item = (MenuManagerItem*) element;
	w_array_free(item->children.array, sizeof(MenuManagerItem),
			MenuManager::FreeItem);
	if (item->command != 0) {
		item->command->Release();
	}
	if (item->name != 0) {
		free(item->name);
	}
	if (item->commandId != 0) {
		free(item->commandId);
	}
}
bool MenuManager::OnInstall(Plugin *info, XMLElement *element) {
	XMLElement *child = element->FirstChildElement();
	while (child != 0) {
		int style = 0;
		if (!strcmp(child->Value(), "item")) {
			style = M_PUSH;
		}
		if (!strcmp(child->Value(), "check")) {
			style = M_CHECK;
		}
		if (!strcmp(child->Value(), "menu")) {
			style = M_CASCADE;
		}
		if (style != 0) {
			const XMLAttribute *attr = child->FindAttribute("location");
			if (attr != 0) {
				const char *location = attr->Value();
				attr = child->FindAttribute("group");
				const char *group = 0;
				if (attr != 0) {
					group = attr->Value();
				}
				MenuManagerItem *item = CreateItem(&root, location, group, -1);
				if (item != 0) {
					item->style = style;
					const char *_command =
							(style & M_CASCADE) != 0 ? "menu" : "command";
					const char *commandId = 0;
					attr = child->FindAttribute(_command);
					if (attr != 0) {
						commandId = attr->Value();
					}
					if (commandId != 0) {
						item->commandId = strdup(commandId);
					}
				}
			}
		}
		child = child->NextSiblingElement();
	}
	return true;
}
struct MenuManagerLoad {
	MenuManager *manager;
	MenuManagerItem *parent;
};
void MenuManager::DoLoad(Config &parent, void *userdata, const char *child) {
	int i = 0;
	while (child[i] != 0) {
		if (child[i] < '0' || child[i] > '9')
			return;
		i++;
	}
	MenuManagerLoad *_load = (MenuManagerLoad*) userdata;
	MenuManagerLoad _loadChildren;
	Config _child, c;
	parent.Open(child, _child);
	if (_child.IsDir()) {
		int _index = atoi(child);
		_child.Open(STYLE, c);
		int style = 0;
		if (c.IsInt()) {
			style = c.GetInt(0);
		}
		if ((style == M_PUSH) != 0 || (style == M_CHECK) != 0
				|| (style == M_CASCADE) != 0 || (style == M_GROUP) != 0) {
			MenuManagerItem *item = _load->parent->children.SetAt(_index);
			if (item != 0) {
				item->style = style;
				/* load name */
				_child.Open("name", c);
				item->name = c.GetString();
				/* load command */
				const char *_command =
						(style & M_CASCADE) != 0 ? "menu" : "command";
				_child.Open(_command, c);
				item->commandId = c.GetString();
				if ((style == M_GROUP) != 0 || (style == M_CASCADE) != 0) {
					_loadChildren.manager = _load->manager;
					_loadChildren.parent = item;
					_loadChildren.parent->style = style;
					_child.ForAll(MenuManager::DoLoad, &_loadChildren);
				}
			}
		}
	}
}
bool MenuManager::OnLoad() {
	Config _current;
	this->config.Open("current", _current);
	if (!_current.IsOk()) {
		this->config.Open("default", _current);
	}
	if (_current.IsOk()) {
		MenuManagerLoad _load;
		_load.manager = this;
		_load.parent = &this->root;
		_current.ForAll(MenuManager::DoLoad, &_load);
		return true;
	} else
		return false;
}

bool MenuManager::OnSave() {
	Config c;
	this->config.CreateDir("default", c);
	return DoSave(c, &root);
	/*this->config.CreateDir("current", c);
	 return DoSave(c, &root);*/
}

bool MenuManager::DoSave(Config &parent, MenuManagerItem *root) {
	MenuManagerItem *items;
	Config c;
	char txt[10];
	int count = root->children.GetCount(items);
	for (int i = 0; i < count; i++) {
		snprintf(txt, sizeof(txt), "%03d", i);
		parent.CreateDir(txt, c);
		if (c.IsOk()) {
			c.CreateString("name").SetString(items[i].name);
			c.CreateInt(STYLE).SetInt(items[i].style);
			if (items[i].commandId != 0) {
				const char *_command =
						(items[i].style & M_CASCADE) != 0 ? "menu" : "command";
				c.CreateString(_command).SetString(items[i].commandId);
			}
			DoSave(c, &items[i]);
		}
	}
	return true;
}
