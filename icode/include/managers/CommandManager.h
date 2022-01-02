/*
 * CommandManager.h
 *
 *  Created on: 11 oct. 2021
 *      Author: azeddine
 */

#ifndef ICODE_INCLUDE_RUNTIME_COMMANDMANAGER_H_
#define ICODE_INCLUDE_RUNTIME_COMMANDMANAGER_H_
#include "Manager.h"
class ICommand;
class CommandManagerItem: public AVLPtrKey<StringId> {
public:
	CommandManagerItem();
	~CommandManagerItem();
	unsigned isCommandLoaded :1;
	char *handlerName;
	ICommand *command;
};
class CommandManager: public Manager {
public:
	CommandManagerItem *commands;
	CommandManager();
	~CommandManager();
	bool OnInstall(Plugin *info, XMLElement *element);
	ICommand* FindCommand(StringId *id);
	ICommand* FindCommand(const char *id) {
		return FindCommand(ManagersGet()->FindString(id));
	}
protected:
	CommandManagerItem* NewItem(StringId *id);
	CommandManagerItem* FindItem(StringId *id);
	static void LoadCommands(Config &parent, void *userdata, const char *child);
	bool OnLoad();
	bool OnSave();
};

#endif /* ICODE_INCLUDE_RUNTIME_COMMANDMANAGER_H_ */
