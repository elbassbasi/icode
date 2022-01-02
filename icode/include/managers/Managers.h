/*
 * IContext.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_RUNTIME_ICONTEXT_H_
#define ICODE_INCLUDE_RUNTIME_ICONTEXT_H_
#include "Plugin.h"
extern "C" {
ICODE_PUBLIC Managers* ManagersGet();
ICODE_PUBLIC int ManagersStart(int argc, char **argv);
ICODE_PUBLIC extern StringId *strings;
ICODE_PUBLIC StringId* ManagersFindString(const char *str, bool _registre);
}
class Managers: protected IDestruct {
protected:
	Managers(int argc, char **argv);
	~Managers();
	bool Init();
	bool InitPath();
	WApp app;
	char *currentPath;
	char *pluginsPath;
	char *binPath;
	Plugin *plugins;
	ThreadGroup threadGroups;
	locale_t usLocal;
	union {
		int flags;
		struct {
			bool isInstallMode :1;
		};
	};
public:
	void *internal[0x50];
	static int LoadPluginsThread(void *args);
	static int SaveManagers(void *args);
	static int SaveManagersAndStartUp(void *args);
	static int RunStartUp(void *args);
	static int NotifyManagersUI(void *args);
	static void PluginsConfigNotify(Config &parent, void *userdata,
			const char *child);
public:
	static Managers *managers;
	static int ManagersStart(int argc, char **argv);
	static Managers* Get() {
		return ManagersGet();
	}
	inline bool IsInstallMode() {
		return isInstallMode;
	}
	/**
	 * path
	 */
	virtual const char* GetCurrentPath();
	virtual const char* GetPluginsPath();
	virtual const char* GetBinPath();
	/**
	 * command line
	 */
	virtual const char** GetCommandLine(int &argc);
	virtual locale_t GetLocal();
	virtual locale_t GetUSLocal();
	virtual ThreadGroup* GetThreadGroups();
	inline void AddTask(w_thread_start fun, void *args, int numberofThread,
			int flags) {
		GetThreadGroups()->AddTask(fun, args, numberofThread, flags);
	}
	inline void AddTask(w_thread_start fun, void *args) {
		GetThreadGroups()->AddTask(fun, args, 1, 0);
	}
	static bool SyncExec(w_thread_start function, void *args) {
		return WApp::Get()->GetDefaultToolkit()->SyncExec(function, args);
	}
	static bool AsyncExec(w_thread_start function, void *args) {
		return WApp::Get()->GetDefaultToolkit()->AsyncExec(function, args);
	}
	/**
	 * plugins
	 */
protected:
	IProgressMonitor* CreateSplash();
	bool InstallPlugin(Plugin *plugin, XMLElement *element);
public:
	virtual bool InstallPlugin(const char *dir, const char *name, int flags);
	/*
	 * Handler
	 */
	virtual Plugin* FindPlugin(StringId *id);
	inline Plugin* FindPlugin(const char *id) {
		return FindPlugin(FindString(id));
	}
	virtual Handler FindHandler(const char *id);
	virtual Manager* FindManager(const char *id);
	int Start();
	int Stop(IProgressMonitor *monitor);
	/*
	 * configuration
	 */
	virtual int OpenConfig(const char *name, int attr, Config &config);
	int OpenConfig(const char *name, Config &config) {
		return OpenConfig(name, 0, config);
	}
	Config GetConfigRoot() {
		Config config;
		OpenConfig(0, 0, config);
		return config;
	}
	Config& GetConfigRoot(Config &config) {
		OpenConfig(0, 0, config);
		return config;
	}
	/*
	 * strings
	 */
	static StringId* RegistreString(const char *str) {
		return ManagersFindString(str, true);
	}
	static StringId* FindString(const char *str) {
		return ManagersFindString(str, false);
	}
	static const char* GetString(StringId *id) {
		if (id == 0)
			return 0;
		return id->name;
	}
};

#endif /* ICODE_INCLUDE_RUNTIME_ICONTEXT_H_ */
