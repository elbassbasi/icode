/*
 * IContext.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_RUNTIME_ICONTEXT_H_
#define ICODE_INCLUDE_RUNTIME_ICONTEXT_H_
#include "Plugin.h"
class IManagers;
extern "C" {
ICODE_PUBLIC IManagers* IManagersGet();
ICODE_PUBLIC int IManagersStart(int argc, char **argv);
}
class MenuManager;
class ICommand;
class ICODE_PUBLIC IManagers: protected IDestruct {
public:
	static IManagers* Get() {
		return IManagersGet();
	}
	/**
	 * path
	 */
	virtual const char* GetCurrentPath()=0;
	virtual const char* GetPluginsPath()=0;
	virtual const char* GetDataPath()=0;
	/**
	 * command line
	 */
	virtual const char** GetCommandLine(int &argc)=0;
	/**
	 * plugins
	 */
	virtual void GetPlugins(WIterator<Plugin*> &plugins) = 0;
	virtual Plugin* FindPlugin(const char *name)=0;
	virtual Plugin* LoadPlugin(const char *name)=0;
	/**
	 * imagelist 16x16
	 */
	virtual int AddImage(const WImage &image, const char *name = 0)=0;
	virtual int FindImage(const char *name) = 0;
	virtual WImageList* GetImageList()=0;
	/**
	 * commands
	 */
	virtual bool RegistreCommand(const char *id, ICommand *command)=0;
	virtual ICommand* FindCommand(const char *id)=0;
};

#endif /* ICODE_INCLUDE_RUNTIME_ICONTEXT_H_ */
