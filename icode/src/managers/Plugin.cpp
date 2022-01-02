/*
 * Plugin.cpp
 *
 *  Created on: 2 sept. 2019
 *      Author: azeddine
 */
#include "../icode.h"

IPlugin::~IPlugin() {
}

bool IPlugin::OnInit(Plugin *info) {
	this->info = info;
	return true;
}
StringId* IPlugin::GetID() {
	return this->info->GetID();
}
const char* IPlugin::GetName() {
	StringId *id = GetID();
	if (id == 0)
		return 0;
	return id->name;
}
IStream* IPlugin::OpenStream(const char *file) {
	return info->OpenStream(file);
}
void IPlugin::OnAttach() {
	/*XMLElement *child = info->element->FirstChildElement();
	 while (child != 0) {
	 if (!strcmp(child->Value(), "manager")) {
	 const XMLAttribute *_id = child->FindAttribute("id");
	 if (_id != 0) {
	 const char *id = _id->Value();
	 Manager *_manager = ManagersGet()->FindManager(id);
	 if (_manager != 0) {
	 _manager->AddItems(this->info, child);
	 }
	 }
	 }
	 child = child->NextSiblingElement();
	 }*/
}

void IPlugin::OnRelease() {
}

void IPlugin::OnStart() {
}
int IPlugin::GetHandlers(HandlerName *&handlers) {
	handlers = 0;
	return 0;
}
int IPlugin::GetManagers(Manager **&managers) {
	managers = 0;
	return 0;
}
Manager* IPlugin::FindManager(const char *id) {
	StringId *_id = Managers::FindString(id);
	if (_id == 0)
		return 0;
	Manager **managers = 0;
	int count = GetManagers(managers);
	if (managers == 0)
		return 0;
	for (int i = 0; i < count; i++) {
		if (managers[i]->GetId() == _id) {
			return managers[i];
		}
	}
	return 0;
}

Handler IPlugin::FindHandler(const char *name) {
	if (name == 0)
		return 0;
	HandlerName *names = 0;
	int count = GetHandlers(names);
	if (names == 0)
		return 0;
	for (int i = 0; i < count; i++) {
		HandlerName *s = &names[i];
		if (!strcmp(s->name, name)) {
			return s->handler;
		}
	}
	return 0;
}

Plugin::Plugin() {
	this->flags = 0;
	this->location = 0;
	this->module = 0;
	this->plugin = 0;
	this->zip = 0;
	this->binary = 0;
}

Plugin::~Plugin() {
	if (this->plugin != 0) {
		delete this->plugin;
	}
	if (this->flags & IS_ZIP) {

	} else {
		if (this->location != 0)
			free(this->location);
	}
}

StringId* Plugin::GetID() {
	return this->key;
}

Manager* Plugin::FindManager(const char *id) {
	Manager *manager = 0;
	IPlugin *plugin = GetPlugin();
	if (plugin != 0) {
		manager = plugin->FindManager(id);
		if (manager != 0) {
			manager->Init(this);
		}
	}
	return manager;
}

Handler Plugin::FindHandler(const char *name) {
	IPlugin *plugin = GetPlugin();
	if (plugin != 0)
		return plugin->FindHandler(name);
	else
		return 0;
}

IStream* Plugin::OpenStream(const char *file) {
	bool ret = false;
	if ((this->flags & IS_ZIP) != 0) {
		return 0;
	} else {
		const char *path = Managers::Get()->GetCurrentPath();
		const char *plugin_name = this->location;
		char *_file = (char*) malloc(
				strlen(path) + strlen(plugin_name) + strlen(file) + 10);
		if (_file == 0)
			return 0;
		sprintf(_file, "%s/%s/%s", path, plugin_name, file);
		FileStream *stream = new FileStream();
		ret = stream->Open(_file, "r");
		free(_file);
		return stream;
	}
}

IPlugin* Plugin::GetPlugin() {
	if ((this->flags & IS_LOADED) == 0) {
		this->flags |= IS_LOADED;
		char txt[0x300];
		snprintf(txt, sizeof(txt), "/plugins/%s/module", this->key->name);
		Config c;
		this->config.Open(txt, c);
		if (c.IsString()) {
			c.GetString(txt, sizeof(txt));
			txt[sizeof(txt) - 1] = 0;
		} else {
			strcpy(txt, this->key->name);
		}
		const char *currentPath = ManagersGet()->GetCurrentPath();
		int length = strlen(currentPath);
		int size = strlen(txt);
		const char *ext = w_module_default_extension();
		int ext_length = strlen(ext);
		int s_length = length + size + ext_length + 0x10;
		char *s = (char*) malloc(s_length);
		snprintf(s, s_length, "%s/bin/lib%s%s", currentPath, txt, ext);
		w_module *module = w_module_load(s);
		free(s);
		if (module != 0) {
			this->module = module;
			_ICodeCreatePlugin createfunction =
					(_ICodeCreatePlugin) w_module_get_symbol(module,
							"ICodeCreatePlugin");
			if (createfunction != 0) {
				this->plugin = createfunction(this);
				this->plugin->OnInit(this);
			}
		}
	}
	return this->plugin;
}

size_t Plugin::ReadFile(const char *file, w_alloc alloc, void *userdata) {
}
Config& Plugin::GetConfig() {
	return this->config;
}

Config& IPlugin::GetConfig() {
	return info->GetConfig();
}

void IPlugin::OnSave() {
	Manager **managers = 0;
	int count = GetManagers(managers);
	if (managers == 0)
		return;
	for (int i = 0; i < count; i++) {
		Manager *manager = managers[i];
		manager->OnSave();
	}
}
