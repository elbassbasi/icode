/*
 * ICodePlugin.h
 *
 *  Created on: 16 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_ICODE_ICODEPLUGIN_H_
#define ICODE_INCLUDE_ICODE_ICODEPLUGIN_H_
#include "Plugin.h"
#include "MenuManager.h"
#include "ViewManager.h"
#include "CommandManager.h"
#include "ImageManager.h"
#include "PerspectiveManager.h"
class ICodePlugin;
extern "C" {
ICODE_PUBLIC ICodePlugin* ICodePluginGet();
ICODE_PUBLIC IPlugin* ICodeCreatePlugin(Plugin *info);
}
class ICodePlugin: public IPlugin {
protected:
	ICodePlugin();
	enum {
		MANAGER_COMMAND = 0,
		MANAGER_VIEW,
		MANAGER_MENUBAR,
		MANAGER_IMAGE,
		MANAGER_PERSPECTIVE,
		MANAGER_COUNT,
	};
	Manager *managers[MANAGER_COUNT];
	CommandManager commands;
	ViewManager views;
	MenuManager menuBar;
	ImageManager images;
	PerspectiveManager perspective;
	virtual Manager* GetManager(int index);
	void Init();
	static HandlerName handlers[];
	static const int handlersCount;
	static int StartUI(void *args);
	bool OnInit(Plugin *info);
	void OnStart();
public:
	static ICodePlugin *plugin;
	static ICodePlugin* _CreatePlugin(Plugin *info);
	static ICodePlugin* Get() {
		return ICodePluginGet();
	}
	int GetHandlers(HandlerName *&handlers);
	int GetManagers(Manager **&managers);
	static MenuManager* GetMenuBarManager() {
		return static_cast<MenuManager*>(ICodePluginGet()->GetManager(
				MANAGER_MENUBAR));
	}
	static ViewManager* GetViewManager() {
		return static_cast<ViewManager*>(ICodePluginGet()->GetManager(
				MANAGER_VIEW));
	}
	static CommandManager* GetCommandManager() {
		return static_cast<CommandManager*>(ICodePluginGet()->GetManager(
				MANAGER_COMMAND));
	}
	static ImageManager* GetImageManager() {
		return static_cast<ImageManager*>(ICodePluginGet()->GetManager(
				MANAGER_IMAGE));
	}
	static PerspectiveManager* GetPerspectiveManager() {
		return static_cast<PerspectiveManager*>(ICodePluginGet()->GetManager(
				MANAGER_PERSPECTIVE));
	}
	static WImageList* GetImageList16() {
		return GetImageManager()->GetImageList16();
	}
	static WImageList* GetImageList32() {
		return GetImageManager()->GetImageList32();
	}
};

#endif /* ICODE_INCLUDE_ICODE_ICODEPLUGIN_H_ */
