/*
 * Manager.h
 *
 *  Created on: 2 oct. 2021
 *      Author: azeddine
 */

#ifndef ICODE_INCLUDE_RUNTIME_MANAGER_H_
#define ICODE_INCLUDE_RUNTIME_MANAGER_H_
#include "Xml.h"
#include "IConfig.h"
class Plugin;
class IPlugin;
class Managers;
typedef IObject* (*Handler)();
class Manager {
public:
	friend Managers;
	friend Plugin;
	friend IPlugin;
	union {
		struct {
			unsigned IsInit : 1;
		};
		int flags;
	};
	Plugin *plugin;
	StringId *id;
	Config config;
	void SetId(const char *id);
	void SetPlugin(Plugin *plugin) {
		this->plugin = plugin;
	}
	Plugin* GetPlugin() {
		return plugin;
	}
public:
	Manager();
	Manager(const char *id);
	virtual ~Manager();
	virtual StringId* GetId();
	void Init(Plugin *plugin);
protected:
	virtual bool OnInit(Plugin *plugin);
	virtual bool OnInstall(Plugin *info, XMLElement *element);
	virtual bool OnLoad();
	virtual bool OnSave();
};

#endif /* ICODE_INCLUDE_RUNTIME_MANAGER_H_ */
