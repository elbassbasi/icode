/*
 * Manager.cpp
 *
 *  Created on: 2 oct. 2021
 *      Author: azeddine
 */

#include "../icode.h"

Manager::Manager() {
	this->id = 0;
	this->plugin = 0;
	this->flags = 0;
}
Manager::Manager(const char *id) {
	SetId(id);
}
Manager::~Manager() {
}

StringId* Manager::GetId() {
	return this->id;
}

void Manager::SetId(const char *id) {
	this->id = Managers::RegistreString(id);
}
bool Manager::OnInit(Plugin *plugin) {
	this->plugin = plugin;
	char txt[0x300];
	snprintf(txt, sizeof(txt), "plugins/%s/managers/%s", plugin->GetID()->name,
			this->id->name);
	ManagersGet()->GetConfigRoot().CreateDir(txt, this->config);
	return config.IsOk();
}

bool Manager::OnInstall(Plugin *info, XMLElement *element) {
	return false;
}

bool Manager::OnLoad() {
	return false;
}

void Manager::Init(Plugin *plugin) {
	if (this->IsInit == false) {
		this->OnInit(plugin);
		if (!ManagersGet()->IsInstallMode()) {
			this->OnLoad();
		}
		this->IsInit = true;
	}
}

bool Manager::OnSave() {
	return true;
}
