/*
 * ICodePlugin.cpp
 *
 *  Created on: 16 mai 2020
 *      Author: Azeddine El Bassbasi
 */
#include "../icode.h"
#include "../model/MWindow.h"
ICodePlugin *ICodePlugin::plugin = 0;
ICodePlugin* ICodePlugin::Get() {
	return plugin;
}
struct PerspectiveItem: public BasicLink {
public:
	PerspectiveInit init;
	int image;
	int id;
	ushort name_length;
	char name[0];
};
struct ViewItem: public BasicLink {
	ViewHandler handler;
	ushort name_length;
	char name[0];
};
const char *ICodePluginImpl_icons[] = { "fldr.png", "file.png" };
void ResourcePerspectiveInit(IPageLayout *layout);
void RegistreViews();
class ICodePluginImpl: public ICodePlugin {
private:
	MenuManager mainMenu;
	BasicList perspectives;
	BasicList views;
	int imageindex;
	WImage images[sizeof(ICodePluginImpl_icons)
			/ sizeof(ICodePluginImpl_icons[0])];
public:
	bool OnInit() {
		return true;
	}
	void OnAttach() {
		RegistrePerspective("Resource", 0, ResourcePerspectiveInit, 0);
		RegistreImages();
		RegistreCommands();
		RegistreMainMenus();
		RegistreViews();
	}
	void OnRelease() {
	}

	void OnStart() {
		MWindow *window = new MWindow();
		window->Create(W_SHELL_TRIM | W_DISPOSE_ON_CLOSE |W_FREE_MEMORY);
		window->LoadPerspective();
		window->SetMaximized(true);
		window->Open();
	}
	WImage* GetImage(int image) {
		if ((size_t) image < sizeof(images) / sizeof(images[0])) {
			return &this->images[image];
		} else
			return 0;
	}
	int GetImageIndex(int image) {
		if ((size_t) image < sizeof(images) / sizeof(images[0])) {
			return this->imageindex;
		} else
			return -1;
	}
	MenuManager* GetMainMenuManager() {
		return &this->mainMenu;
	}
	void RegistreMainMenus() {
		MenuManager *mainmenu = GetMainMenuManager();
		if (mainmenu == 0)
			return;
		mainmenu->AddItem("File/New/File", 0, W_PUSH);
		mainmenu->AddItem("File/", 0, W_SEPARATOR);
		mainmenu->AddItem("File/Restart", "Restart", W_PUSH);
		mainmenu->AddItem("File/Exit", "Quit", W_PUSH);
		mainmenu->AddItem("Window/New Window", 0, W_PUSH);
		mainmenu->AddItem("Window/FullScreen", "FullScreen", W_PUSH);
		mainmenu->AddItem("Window/ ", 0, W_SEPARATOR);
		mainmenu->AddItem("Window/Preferences", 0, W_PUSH);
	}
	void RegistreImages() {
		LoadImagesAndRegistre(images, ICodePluginImpl_icons,
				sizeof(images) / sizeof(images[0]), &imageindex);
	}

	void RegistreCommands() {
		CommandsRegistre();
	}
	int RegistrePerspective(const char *name, int image, PerspectiveInit init,
			int flags) {
		ushort length = strlen(name);
		PerspectiveItem *item = (PerspectiveItem*) malloc(
				sizeof(PerspectiveItem) + length + 1);
		if (item == 0)
			return 0;
		item->init = init;
		item->image = image;
		item->name_length = length;
		memcpy(item->name, name, length);
		item->name[length] = 0;
		item->id = perspectives.size + 1;
		perspectives.LinkLast(item);
		return item->id;
	}

	void OpenPerspective(MWindow *parent, const char *name) {
		ushort length = strlen(name);
		PerspectiveItem *item = (PerspectiveItem*) perspectives.first, *i = 0;
		while (item != 0) {
			if (length == item->name_length) {
				if (!strcmp(name, item->name)) {
					i = item;
					break;
				}
			}
			item = (PerspectiveItem*) item->next;
		}
		if (i == 0)
			return;
		MPerspective *perspective = new MPerspective(parent);
		perspective->Create(parent);
		i->init(perspective);
		parent->SwitchToPerspective(perspective);
	}
	int RegistreView(const char *name, ViewHandler handler) {
		if (handler == 0)
			return 0;
		ushort length = strlen(name);
		ViewItem *item = (ViewItem*) malloc(sizeof(ViewItem) + length + 1);
		if (item == 0)
			return 0;
		item->handler = handler;
		item->name_length = length;
		memcpy(item->name, name, length);
		item->name[length] = 0;
		views.LinkLast(item);
		return true;
	}

	IViewPart* CreateView(const char *id) {
		ushort length = strlen(id);
		ViewItem *item = (ViewItem*) views.first;
		while (item != 0) {
			if (length == item->name_length) {
				if (!strcmp(id, item->name)) {
					return item->handler();
				}
			}
			item = (ViewItem*) item->next;
		}
		return 0;
	}

};
extern "C" {
ICODE_PUBLIC Plugin* ICodeCreatePlugin() {
	if (ICodePlugin::plugin == 0) {
		ICodePlugin::plugin = new ICodePluginImpl();
	}
	return ICodePlugin::plugin;
}
}
