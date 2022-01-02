/*
 * Managers.cpp
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */
#include "../icode.h"
#include "../ui/managers/ManagerUI.h"
#include "../db/DBConfig.h"
#include <new>
void database_test();
void lines_test();
extern "C" {
Managers* ManagersGet() {
	return Managers::managers;
}
int ManagersStart(int argc, char **argv) {
	//lines_test();
	//database_test();
	return Managers::ManagersStart(argc, argv);
}
}
struct StartUp {
	const char *handler;
	IStartup *startup;
};
class ManagersInternal {
public:
	DBConfigManager db;
	ManagersInternal() {

	}
	~ManagersInternal() {

	}
	int Open(Managers *managers) {
		const char *path = managers->GetCurrentPath();
		int sz = strlen(path);
		char *s = (char*) malloc(sz + 30);
		if (s == 0)
			return 0;
		sprintf(s, "%s/config.db", path);
		int ret = db.Open(s, 0);
		free(s);
		return ret;
	}

};
Managers *Managers::managers = 0;
Managers::Managers(int argc, char **argv) :
		app(argc, argv) {
	this->flags = 0;
	Init();
	ManagersInternal *internal = new (this->internal) ManagersInternal();
	int ret = internal->Open(this);
}
Managers::~Managers() {
	ManagersInternal *internal =
			reinterpret_cast<ManagersInternal*>(this->internal);
	internal->~ManagersInternal();
	if (this->currentPath != 0) {
		free(this->currentPath);
	}
	if (this->pluginsPath != 0) {
		free(this->pluginsPath);
	}
	if (this->binPath != 0) {
		free(this->binPath);
	}
	if (this->usLocal != 0) {
		freelocale(usLocal);
	}
}

bool Managers::Init() {
	if (app.app->argc <= 0)
		return 0;
	if (!InitPath()) {
		return false;
	}
	threadGroups.Create();
	return true;
}

int Managers::ManagersStart(int argc, char **argv) {
	if (argc <= 0)
		return 0;
	managers = new Managers(argc, argv);
	int ret = managers->Start();
	delete managers;
	return ret;
}

const char** Managers::GetCommandLine(int &argc) {
	argc = this->app.app->argc;
	return (const char**) this->app.app->argv;
}
/**
 * path
 */
bool Managers::InitPath() {
	char *s = app.app->argv[0];
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
	memcpy(this->currentPath, app.app->argv[0], last1);
	this->currentPath[last1] = 0;
	for (int i = 0; i < last1; i++) {
		if (this->currentPath[i] == '\\')
			this->currentPath[i] = '/';
	}
	this->pluginsPath = (char*) malloc(last1 + 10);
	if (this->pluginsPath == 0)
		return false;
	sprintf(this->pluginsPath, "%s/plugins", this->currentPath);
	this->binPath = (char*) malloc(last1 + 6);
	if (this->binPath == 0)
		return false;
	sprintf(this->binPath, "%s/bin", this->currentPath);
	/* */
	this->usLocal = newlocale(LC_ALL_MASK, "en_US.UTF-8", (locale_t) 0);
	this->plugins = 0;
	return true;
}
const char* Managers::GetCurrentPath() {
	return this->currentPath;
}

const char* Managers::GetPluginsPath() {
	return this->pluginsPath;
}

const char* Managers::GetBinPath() {
	return this->binPath;
}
locale_t Managers::GetLocal() {
}

locale_t Managers::GetUSLocal() {
	return this->usLocal;
}
ThreadGroup* Managers::GetThreadGroups() {
	return &this->threadGroups;
}
/*
 * plugins
 */
bool Managers::InstallPlugin(Plugin *plugin, XMLElement *element) {
	ManagersInternal *internal =
			reinterpret_cast<ManagersInternal*>(this->internal);
	XMLElement *child = element->FirstChildElement();
	while (child != 0) {
		if (!strcmp(child->Value(), "bin")) {

		}
		if (!strcmp(child->Value(), "manager")) {
			const XMLAttribute *_id = child->FindAttribute("id");
			if (_id != 0) {
				const char *id = _id->Value();
				Manager *manager = FindManager(id);
				if (manager != 0) {
					manager->OnInstall(plugin, child);
				}
			}
		}
		child = child->NextSiblingElement();
	}
	return true;
}
bool Managers::InstallPlugin(const char *dir, const char *name, int flags) {
	const char *currentPath = this->GetCurrentPath();
	XMLDocument *document = 0;
	Zip *zip = 0;
	char *plugin_xml = (char*) malloc(strlen(dir) + strlen(name) + 20);
	if (plugin_xml == 0)
		return false;
	sprintf(plugin_xml, "%s/%s/plugin.xml", dir, name);
	FILE *file = fopen(plugin_xml, "r");
	if (file != 0) {
		document = new XMLDocument();
		XMLError err = document->LoadFile(plugin_xml);
		if (err != XML_SUCCESS) {
			delete document;
			document = 0;
		}
		fclose(file);
	} else {
		// is zip file
	}
	free(plugin_xml);
	char config[0x200];
	if (document != 0) {
		this->isInstallMode = true;
		XMLElement *element = document->FirstChildElement();
		while (element != 0) {
			if (!strcmp(element->Value(), "plugin")) {
				const XMLAttribute *_id = element->FindAttribute("id");
				const char *id = _id->Value();
				if (_id != 0) {
					StringId *pluginID = Managers::RegistreString(id);
					if (pluginID == 0) {
						break;
					}
					Plugin *p;
					AVLNode *node = this->plugins->FindNode(pluginID,
							ComparePtrKey, 0);
					if (node == 0) {
						p = new Plugin();
						p->key = pluginID;
						p->location = strdup(dir);
						this->plugins =
								static_cast<Plugin*>(this->plugins->InsertNode(
										p, ComparePtrNode, 0));
					} else {
						p = static_cast<Plugin*>(node);
					}
					Config root, c;
					ManagersGet()->GetConfigRoot(root);
					sprintf(config, "/plugins/%s/enable", id);
					root.CreateBool(config, c);
					c.SetBool(true);
					sprintf(config, "/plugins/%s/location", id);
					root.CreateString(config, c);
					c.SetString(dir);
					_id = element->FindAttribute("startup");
					if (_id != 0) {
						bool v = _id->BoolValue();
						if (v) {
							sprintf(config, "/plugins/%s/startup", id);
							root.CreateBool(config, c);
							c.SetBool(true);
							p->flags |= Plugin::IS_STARTUP;
						}
					}
					InstallPlugin(p, element);
				}
			}
			element = element->NextSiblingElement();
		}
		this->isInstallMode = false;
		delete document;
	}
	return true;
}
void Managers::PluginsConfigNotify(Config &parent, void *userdata,
		const char *child) {
	Config c, r;
	parent.Open(child, r);
	r.Open("enable", c);
	bool enabled = false;
	c.GetBool(&enabled);
	if (enabled == false)
		return;
	const char *id = child;
	StringId *_id = RegistreString(id);
	if (_id == 0)
		return;
	Plugin *p = new Plugin();
	p->key = _id;
	p->location = strdup(id);
	ManagersGet()->plugins =
			static_cast<Plugin*>(ManagersGet()->plugins->InsertNode(p,
					ComparePtrNode, 0));
	r.Open("startup", c);
	bool _start = false;
	c.GetBool(&_start);
	if (_start) {
		p->flags |= Plugin::IS_STARTUP;
	}
	p->config = c;

}
int Managers::NotifyManagersUI(void *args) {
	ManagerUI *ui = new ManagerUI(Managers::SaveManagersAndStartUp, args);
	ui->Open();
	return true;
}
int Managers::LoadPluginsThread(void *args) {
	Config pluginsConfig;
	ManagersGet()->GetConfigRoot().Open("/plugins", pluginsConfig);
	if (pluginsConfig.IsDir()) {
		pluginsConfig.ForAll(PluginsConfigNotify, 0);
		RunStartUp(args);
	} else {
		AsyncExec(Managers::NotifyManagersUI, args);
	}
	return true;
}
int Managers::RunStartUp(void *args) {
	AVLNode *node = ManagersGet()->plugins->GetFirst();
	while (node != 0) {
		Plugin *p = static_cast<Plugin*>(node);
		if (p->flags & Plugin::IS_STARTUP) {
			IPlugin *_p = p->GetPlugin();
			if (_p != 0) {
				_p->OnStart();
			}
		}
		node = node->GetNext();
	}
	if (args != 0) {
		IProgressMonitor *monitor = (IProgressMonitor*) args;
		monitor->Release();
	}
	return 1;
}
int Managers::SaveManagers(void *args) {
	AVLNode *node = ManagersGet()->plugins->GetFirst();
	while (node != 0) {
		Plugin *p = static_cast<Plugin*>(node);
		IPlugin *_p = p->GetPlugin();
		if (_p != 0) {
			_p->OnSave();
		}
		node = node->GetNext();
	}
	return 1;
}
int Managers::SaveManagersAndStartUp(void *args) {
	SaveManagers(args);
	return RunStartUp(args);
}
IObject* SplashHandler();
IProgressMonitor* Managers::CreateSplash() {
	IProgressMonitor *monitor = 0;
	IObject *obj = 0;
	Handler handler = FindHandler("icode.Splash");
	if (handler != 0) {
		obj = handler();
	}
	if (obj == 0)
		obj = SplashHandler();
	monitor = obj->QueryInterfaceT<IProgressMonitor>();
	if (monitor == 0)
		obj->Release();
	return monitor;
}
int Managers::Start() {
	IProgressMonitor *progress = CreateSplash();
	AddTask(LoadPluginsThread, progress);
	return WApp::Get()->Run();
}
int Managers::Stop(IProgressMonitor *monitor) {
	WApp::Get()->Exit();
	return 1;
}
Plugin* Managers::FindPlugin(StringId *id) {
	if (id == 0)
		return 0;
	AVLNode *node = plugins->FindNode(id, ComparePtrKey, 0);
	if (node == 0) {
		return 0;
	} else {
		return static_cast<Plugin*>(node);
	}
}
Handler Managers::FindHandler(const char *id) {
	char pluginName[0x100];
	const char *s1 = strchr(id, '.');
	if (s1 == 0)
		return 0;
	intptr_t length = s1 - id;
	if (length > sizeof(pluginName))
		return 0;
	memcpy(pluginName, id, length);
	pluginName[length] = 0;
	Plugin *plugin = FindPlugin(pluginName);
	if (plugin == 0)
		return 0;
	return plugin->FindHandler(++s1);
}

Manager* Managers::FindManager(const char *id) {
	char pluginName[0x100];
	const char *s1 = strchr(id, '.');
	if (s1 == 0)
		return 0;
	intptr_t length = s1 - id;
	if (length > sizeof(pluginName))
		return 0;
	memcpy(pluginName, id, length);
	pluginName[length] = 0;
	Plugin *plugin = FindPlugin(pluginName);
	if (plugin == 0)
		return 0;
	return plugin->FindManager(++s1);
}
/**
 *
 */

int Managers::OpenConfig(const char *name, int attr, Config &config) {
	DBConfigManager *db =
			&reinterpret_cast<ManagersInternal*>(this->internal)->db;
	return db->OpenConfig(name, attr, config);
}
int StringId::CompareKey(const AVLNode *node, const void *key, void *userData) {
	return AVLNode::CompareString(static_cast<const StringId*>(node)->name,
			(const char*) key);
}
int StringId::CompareNode(const AVLNode *node1, const AVLNode *node2,
		void *userData) {
	return AVLNode::CompareString(static_cast<const StringId*>(node1)->name,
			static_cast<const StringId*>(node2)->name);
}
StringId* StringId::CreateStringId(const char *name, int length) {
	void *ptr = malloc(sizeof(StringId) + length + 1);
	if (ptr == 0)
		return 0;
	StringId *id = new (ptr) StringId();
	id->ref = 0;
	id->length = length;
	memcpy(id->name, name, length);
	id->name[length] = 0;
	return id;
}
extern "C" {
StringId *strings = 0;
StringId* ManagersFindString(const char *str, bool _registre) {
	if (str == 0)
		return 0;
	StringId *id = (StringId*) strings->FindNode(str, StringId::CompareKey, 0);
	if (_registre) {
		if (id == 0) {
			id = StringId::CreateStringId(str, strlen(str));
			if (id == 0)
				return 0;
			strings = (StringId*) strings->InsertNode(id, StringId::CompareNode,
					0);
		}
		id->ref++;
	}
	return id;
}
}

