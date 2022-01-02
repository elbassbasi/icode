/*
 * ICodePlugin.cpp
 *
 *  Created on: 13 oct. 2021
 *      Author: azeddine
 */
#include "../icode.h"
#include "../model/MWindow.h"
ICodePlugin::ICodePlugin() {
	Init();
}

void ICodePlugin::Init() {

}
bool ICodePlugin::OnInit(Plugin *info) {
	IPlugin::OnInit(info);
	managers[MANAGER_COMMAND] = &commands;
	managers[MANAGER_VIEW] = &views;
	managers[MANAGER_MENUBAR] = &menuBar;
	managers[MANAGER_IMAGE] = &images;
	managers[MANAGER_PERSPECTIVE] = &perspective;
	managers[MANAGER_COMMAND]->SetId("Command");
	managers[MANAGER_VIEW]->SetId("View");
	managers[MANAGER_MENUBAR]->SetId("MenuBar");
	managers[MANAGER_IMAGE]->SetId("Image");
	managers[MANAGER_PERSPECTIVE]->SetId("Perspective");
	return true;
}
int ICodePlugin::StartUI(void *args) {
	MWindow *window = new MWindow();
	window->Create(W_FRAME_TRIM | W_DISPOSE_ON_CLOSE | W_FREE_MEMORY);
	window->SetMaximized(true);
	window->Open();
	return W_TRUE;
}
void ICodePlugin::OnStart() {
	ManagersGet()->AsyncExec(ICodePlugin::StartUI, this);
}

Manager* ICodePlugin::GetManager(int index) {
	if (index < MANAGER_COUNT) {
		managers[index]->Init(this->info);
		return managers[index];
	} else
		return 0;
}
int ICodePlugin::GetHandlers(HandlerName *&_handlers) {
	_handlers = handlers;
	return handlersCount;
}

int ICodePlugin::GetManagers(Manager **&_managers) {
	_managers = managers;
	return sizeof(managers) / sizeof(managers[0]);
}
ICodePlugin *ICodePlugin::plugin = 0;
ICodePlugin* ICodePlugin::_CreatePlugin(Plugin *info) {
	if (plugin == 0) {
		plugin = new ICodePlugin();
	}
	return plugin;
}
extern "C" {
ICodePlugin* ICodePluginGet() {
	return ICodePlugin::plugin;
}
IPlugin* ICodeCreatePlugin(Plugin *info) {
	return ICodePlugin::_CreatePlugin(info);
}
}
