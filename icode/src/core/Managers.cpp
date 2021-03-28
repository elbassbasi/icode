/*
 * Managers.cpp
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#include "Managers.h"
#include "Splash.h"
#include <new>
extern "C" {
IManagers* IManagersGet() {
	return Managers::managers;
}
int IManagersStart(int argc, char **argv) {
	return Managers::ManagersStart(argc, argv);
}
}
Managers *Managers::managers = 0;
Managers::Managers(int argc, char **argv) :
		app(argc, argv) {
	Init();
	this->pluginLast = 0;
	this->plugins = 0;
	this->commands = 0;
}
Managers::~Managers() {
}

bool Managers::Init() {
	if (app.argc <= 0)
		return 0;
	if (!InitPath()) {
		return false;
	}
	InitImageList();
	return true;
}

int Managers::ManagersStart(int argc, char **argv) {
	if (argc <= 0)
		return 0;
	managers = new Managers(argc, argv);
	managers->Init();
	int ret = managers->Start();
	delete managers;
	return ret;
}

const char** Managers::GetCommandLine(int &argc) {
	argc = this->app.argc;
	return (const char**) this->app.argv;
}
/**
 * path
 */
bool Managers::InitPath() {
	char *s = app.argv[0];
	int i = 0, last0 = 0, last1 = 0;
	while (s[i] != 0) {
		if (s[i] == '/' || s[i] == '\\') {
			last1 = last0;
			last0 = i;
		}
		i++;
	}
	this->currentPath = (char*) malloc(last1 + 1);
	if (this->currentPath == 0)
		return false;
	memcpy(this->currentPath, app.argv[0], last1);
	this->currentPath[last1] = 0;
	for (int i = 0; i < last1; i++) {
		if (this->currentPath[i] == '\\')
			this->currentPath[i] = '/';
	}
	this->pluginsPath = (char*) malloc(last1 + 10);
	if (this->pluginsPath == 0)
		return false;
	sprintf(this->pluginsPath, "%s/plugins", this->currentPath);
	this->dataPath = (char*) malloc(last1 + 6);
	if (this->dataPath == 0)
		return false;
	sprintf(this->dataPath, "%s/data", this->currentPath);
	return true;
}
const char* Managers::GetCurrentPath() {
	return this->currentPath;
}

const char* Managers::GetPluginsPath() {
	return this->pluginsPath;
}

const char* Managers::GetDataPath() {
	return this->dataPath;
}
/**
 * plugins
 */
class PluginsIterator: public WBasicIterator<Plugin*> {
public:
	Plugin *first;
	Plugin *current;
	PluginsIterator(Plugin *first) {
		this->first = first;
		this->current = first;
	}
	WResult Reset() {
		this->current = first;
		return true;
	}
	WResult Remove() {
		return false;
	}
	size_t GetCount() {
		return 0;
	}
	bool Next(Plugin *&t) {
		if (current == 0)
			return false;
		t = current;
		current = Managers::NextPlugin(current);
		return true;
	}
};
void Managers::GetPlugins(WIterator<Plugin*> &plugins) {
	new (&plugins) PluginsIterator(this->plugins);
}

Plugin* Managers::FindPlugin0(const char *name, size_t length) {
	const char *s;
	bool is_find;
	Plugin *plugin = this->plugins;
	while (plugin != 0) {
		s = plugin->GetName();
		is_find = true;
		for (size_t j = 0; j < length; j++) {
			if (name[j] != s[j]) {
				is_find = false;
				break;
			}
			if (s[j] == 0 || name[j] == 0) {
				break;
			}
		}
		if (is_find)
			return plugin;
		plugin = plugin->data.next;
	}
	return 0;
}
Plugin* Managers::FindPlugin(const char *name) {
	return FindPlugin0(name, -1);
}

Plugin* Managers::LoadPlugin(const char *name) {
	char txt[0x100];
	snprintf(txt, sizeof(txt), "%s/%s", name, w_module_default_extension());
	w_module *module = w_module_load(txt);
	if (module == 0)
		return 0;
	_ICodeCreatePlugin createfunction =
			(_ICodeCreatePlugin) w_module_get_symbol(module,
					"ICodeCreatePlugin");
	if (createfunction == 0) {
		w_module_unload(module);
		return 0;
	}
	return LoadPlugin0(module, name, createfunction);
}
Plugin* Managers::LoadPlugin0(w_module *module, const char *name,
		_ICodeCreatePlugin createfunction) {
	Plugin *plugin = createfunction();
	if (plugin != 0) {
		plugin->data.module = module;
		plugin->data.name = strdup(name);
		plugin->data.next = 0;
		if (pluginLast != 0) {
			pluginLast->data.next = plugin;
		} else {
			this->plugins = plugin;
		}
		pluginLast = plugin;
		plugin->OnInit();
	}
	return plugin;
}
extern "C" {
ICODE_PUBLIC Plugin* ICodeCreatePlugin();
}
void Managers::LoadPlugins() {
	LoadPlugin0(0, "icode", ICodeCreatePlugin);
	char name[0x100];
	size_t length = strlen(this->GetCurrentPath());
	char *s = (char*) malloc(length + 0x200);
	if (s == 0)
		return;
	const char *ext = w_module_default_extension();
	size_t ext_length = strlen(ext);
	memcpy(s, this->GetCurrentPath(), length);
	memcpy(&s[length], "/plugins/", 9);
	length += 8;
	WIterator<WFindDir> it;
	WFindDir find;
	WDirList(s, it);
	while (it.Next(find)) {
		size_t l = strlen(find.name);
		if (l > ext_length && !strcmp(&find.name[l - ext_length], ext)) {
			memcpy(&s[length], find.name, l + 1);
			w_module *module = w_module_load(s);
			if (module != 0) {
				_ICodeCreatePlugin createfunction =
						(_ICodeCreatePlugin) w_module_get_symbol(module,
								"ICodeCreatePlugin");
				if (createfunction != 0) {
					if (find.name[0] == 'l' && find.name[1] == 'i'
							&& find.name[2] == 'b') {
						memcpy(name, &find.name[3], l - 3 - ext_length);
						name[l - 3 - ext_length] = 0;
					} else {
						memcpy(name, find.name, l - ext_length);
						name[l - ext_length] = 0;
					}
					LoadPlugin0(module, name, createfunction);
				}
			}
		}
	}
	free(s);
}
/**
 *
 */
int Managers::AddImage(const WImage &image, const char *name) {
	return imagelist.Add((WImage&) image);
}

int Managers::FindImage(const char *name) {
	return -1;
}

void Managers::StartPlugins() {
	Plugin *plugin = plugins;
	while (plugin != 0) {
		plugin->OnStart();
		plugin = plugin->data.next;
	}
}
void Managers::AttachPlugins() {
	Plugin *plugin = plugins;
	while (plugin != 0) {
		plugin->OnAttach();
		plugin = plugin->data.next;
	}
}
int Managers::Start() {
	Splash *splash = new Splash();
	splash->Create();
	return app.Run();
}
bool Managers::RegistreCommand(const char *id, ICommand *command) {
	if (id == 0 || command == 0)
		return false;
	CommandItem *commands = this->commands, *last = 0;
	ushort length = strlen(id);
	while (commands != 0) {
		if (length == commands->idlength) {
			if (!strcmp(commands->id, id)) {
				return false;
			}
		}
		last = commands;
		commands = commands->next;
	}
	commands = (CommandItem*) malloc(sizeof(CommandItem) + length + 1);
	if (commands == 0)
		return false;
	commands->next = 0;
	commands->command = command;
	commands->idlength = length;
	memcpy(commands->id, id, length);
	commands->idlength = length;
	if (last == 0) {
		this->commands = commands;
	} else {
		last->next = commands;
	}
	return true;
}

ICommand* Managers::FindCommand(const char *id) {
	if (id == 0)
		return 0;
	CommandItem *commands = this->commands, *last = 0;
	ushort length = strlen(id);
	while (commands != 0) {
		if (length == commands->idlength) {
			if (!strcmp(commands->id, id)) {
				return commands->command;
			}
		}
		commands = commands->next;
	}
	return 0;
}

int Managers::Exit() {
	WApp::Get()->Exit();
	return 1;
}
WImageList* Managers::GetImageList() {
	return &this->imagelist;
}

void Managers::InitImageList() {
	imagelist.Create(16, 16);
}
