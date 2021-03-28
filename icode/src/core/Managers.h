/*
 * Managers.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_CORE_MANAGERS_H_
#define ICODE_CORE_MANAGERS_H_
#include "../icode.h"
struct CommandItem {
	CommandItem* next;
	ICommand *command;
	ushort idlength;
	char id[0];
};
class Managers: public IManagers {
public:
	static Managers *managers;
	WApp app;
	char* currentPath;
	char *pluginsPath;
	char *dataPath;
	Plugin *plugins;
	Plugin *pluginLast;
	WImageList imagelist;
	CommandItem* commands;
public:
	static Plugin* NextPlugin(Plugin* plugin){
		return plugin->data.next;
	}
	Managers(int argc, char **argv);
	~Managers();
	bool Init();
	bool InitPath();
	static int ManagersStart(int argc, char **argv);
	/**
	 * path
	 */
	const char** GetCommandLine(int &argc);
	const char* GetCurrentPath();
	const char* GetPluginsPath();
	const char* GetDataPath();
	/**
	 * plugins
	 */
	void GetPlugins(WIterator<Plugin*> &plugins);
	Plugin* FindPlugin0(const char *name, size_t length);
	Plugin* FindPlugin(const char *name);
	Plugin* LoadPlugin(const char *name);
	Plugin* LoadPlugin0(w_module *module, const char *name,
			_ICodeCreatePlugin createfunction);
	/**
	 * imagelist 16x16
	 */
	void InitImageList();
	int AddImage(const WImage &image, const char* name = 0);
	int FindImage(const char* name);
	WImageList* GetImageList();
	/**
	 * commands
	 */
	bool RegistreCommand(const char *id, ICommand *command);
	ICommand* FindCommand(const char *id);
public:
	int Start();
	void StartPlugins();
	void AttachPlugins();
	void LoadPlugins();
	int Exit();
};

#endif /* ICODE_CORE_MANAGERS_H_ */
