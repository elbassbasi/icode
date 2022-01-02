/*
 * CommandManager.cpp
 *
 *  Created on: 11 oct. 2021
 *      Author: azeddine
 */

#include "../icode.h"

CommandManager::CommandManager() {
	this->commands = 0;
}

CommandManager::~CommandManager() {
}
bool CommandManager::OnInstall(Plugin *info, XMLElement *element) {
	XMLElement *child = element->FirstChildElement();
	while (child != 0) {
		if (!strcmp(child->Value(), "command")) {
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
						CommandManagerItem *item = NewItem(id);
						if (item != 0) {
							item->handlerName = strdup(_handler);
						}
					}
				}
			}
		}
		child = child->NextSiblingElement();
	}
	return true;
}

ICommand* CommandManager::FindCommand(StringId *id) {
	if(id == 0) return 0;
	CommandManagerItem *item = FindItem(id);
	if (item != 0) {
		if (item->isCommandLoaded == 0) {
			Handler handler = Managers::Get()->FindHandler(item->handlerName);
			if (handler != 0) {
				IObject *obj = handler();
				if (obj != 0) {
					item->command = obj->QueryInterfaceT<ICommand>();
				} else
					obj->Release();
			}
			item->isCommandLoaded = 1;
		}
		return item->command;
	}
	return 0;
}

void CommandManager::LoadCommands(Config &parent, void *userdata,
		const char *child) {
	StringId *id = Managers::RegistreString(child);
	if (id != 0) {
		CommandManager *manager = (CommandManager*) userdata;
		CommandManagerItem *item = manager->NewItem(id);
		if (item != 0) {
			Config c;
			parent.Open(child, c);
			if (c.IsOk()) {
				item->handlerName = c.Open("handler").GetString();
			}
		}
	}
}

bool CommandManager::OnLoad() {
	this->config.ForAll(LoadCommands, this);
	return true;
}

bool CommandManager::OnSave() {
	Config c;
	AVLNode *command = this->commands->GetFirst();
	while (command != 0) {
		CommandManagerItem *_command = static_cast<CommandManagerItem*>(command);
		this->config.CreateDir(_command->key->name, c);
		if (c.IsOk()) {
			c.CreateString("handler").SetString(_command->handlerName);
		}
		command = command->GetNext();
	}
	return true;
}

CommandManagerItem* CommandManager::NewItem(StringId *id) {
	AVLNode *node = this->commands->Find(id);
	if (node != 0)
		return 0;
	CommandManagerItem *item = new CommandManagerItem();
	node = this->commands->Insert(item);
	this->commands = static_cast<CommandManagerItem*>(node);
	item->key = id;
	return item;
}

CommandManagerItem* CommandManager::FindItem(StringId *id) {
	AVLNode *node = this->commands->Find(id);
	if (node != 0)
		return static_cast<CommandManagerItem*>(node);
	else
		return 0;
}

CommandManagerItem::CommandManagerItem() {
	this->isCommandLoaded = false;
	this->handlerName = 0;
	this->command = 0;
}

CommandManagerItem::~CommandManagerItem() {
	if (this->handlerName != 0) {
		free(this->handlerName);
	}
	if (this->command != 0) {
		this->command->Release();
	}
}
