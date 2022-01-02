/*
 * Plugin.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_RUNTIME_PLUGIN_H_
#define ICODE_INCLUDE_RUNTIME_PLUGIN_H_
#include "StringId.h"
#include "Manager.h"
class IStream;
class Zip;
class Managers;
class Plugin: protected AVLPtrKey<StringId> {
private:
	Plugin();
	~Plugin();
	enum {
		IS_ZIP = 0x01, IS_LOADED = 0x02, IS_STARTUP = 0x04,
	};
	int flags;
	IPlugin *plugin;
	char *binary;
	char *location;
	Zip *zip;
	w_module *module;
	Config config;
public:
	friend class Managers;
	friend class ManagerUI;
	virtual StringId* GetID();
	virtual Manager* FindManager(const char *id);
	virtual Handler FindHandler(const char *name);
	virtual Config& GetConfig();
	virtual IStream* OpenStream(const char *file);
	virtual size_t ReadFile(const char *file, w_alloc alloc, void *userdata);
	virtual IPlugin* GetPlugin();
	inline size_t ReadFile(const char *file, void **mem) {
		w_alloc_buffer buff;
		buff.buffer = 0;
		buff.total_size = 0;
		buff.size = 0;
		ReadFile(file, w_alloc_buffer_new, &buff);
		*mem = buff.buffer;
		return buff.total_size;
	}
};
extern "C" {
typedef IPlugin* (*_ICodeCreatePlugin)(Plugin *info);
}
struct HandlerName {
	const char *name;
	Handler handler;
};
class IPlugin {
public:
	friend class Managers;
	friend Plugin;
	virtual ~IPlugin();
protected:
	virtual bool OnInit(Plugin *info);
	virtual void OnAttach();
	virtual void OnRelease();
	virtual void OnStart();
	virtual void OnSave();
public:
	virtual StringId* GetID();
	virtual const char* GetName();
	virtual Config& GetConfig();
	virtual IStream* OpenStream(const char *file);
	virtual int GetHandlers(HandlerName *&handlers);
	virtual int GetManagers(Manager **&managers);
	virtual Manager* FindManager(const char *id);
	virtual Handler FindHandler(const char *name);
public:
	Plugin *info;
};

#endif /* ICODE_INCLUDE_RUNTIME_PLUGIN_H_ */
