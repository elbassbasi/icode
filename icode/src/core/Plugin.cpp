/*
 * Plugin.cpp
 *
 *  Created on: 2 sept. 2019
 *      Author: azeddine
 */
#include "../icode.h"

Plugin::Plugin() {
	memset(this->reserved, 0, sizeof(this->reserved));
}

Plugin::~Plugin() {
	if (this->data.name != 0) {
		free(this->data.name);
	}
}

bool Plugin::OnInit() {
	return true;
}

const char* Plugin::GetName() {
	return this->data.name;
}
IStream* Plugin::OpenStream(const char *file) {
	return 0;
}

IConfig* Plugin::OpenConfig(const char *config) {
	return 0;
}

void Plugin::OnAttach() {
}

void Plugin::OnRelease() {
}

void Plugin::OnStart() {
}
bool Plugin::LoadImage(WImage &image, const char *name) {
	const char *path = IManagers::Get()->GetDataPath();
	const char *plugin_name = this->GetName();
	char *file = (char*) malloc(
			strlen(path) + strlen(plugin_name) + strlen(name) + 10);
	if (file == 0)
		return false;
	sprintf(file, "%s/%s/%s", path, plugin_name, name);
	bool ret = image.Create(file);
	free(file);
	return ret;
}

size_t Plugin::LoadImages(WImage *images, const char **names, size_t length) {
	const char *path = IManagers::Get()->GetDataPath();
	const char *plugin_name = this->GetName();
	char *file = (char*) malloc(strlen(path) + strlen(plugin_name) + 0x500);
	if (file == 0)
		return false;
	size_t ret = 0;
	for (size_t i = 0; i < length; i++) {
		sprintf(file, "%s/%s/%s", path, plugin_name, names[i]);
		if (images[i].Create(file)) {
			ret++;
		}
	}
	free(file);
	return ret;
}
bool Plugin::CreateEmptyImage(WImage &image, int w, int h) {
	WSurface surface;
	WGraphics gc;
	surface.Create(w, w);
	surface.GetGraphics(gc);
	gc.SetAdvanced(true);
	if (gc.GetAdvanced()) {
		gc.SetAlpha(0);
		gc.FillRectangle(0, 0, w, h);
	} else {
		gc.FillRectangle(0, 0, w, h);
	}
	surface.GetImage(image);
	return true;
}
size_t Plugin::LoadImagesAndRegistre(WImage *images, const char **names,
		size_t length, int *firstindex) {
	*firstindex = -1;
	const char *path = IManagers::Get()->GetDataPath();
	const char *plugin_name = this->GetName();
	char *file = (char*) malloc(strlen(path) + strlen(plugin_name) + 0x500);
	if (file == 0)
		return false;
	WImageList *imagelist = IManagers::Get()->GetImageList();
	size_t ret = 0;
	bool registred = false;
	for (size_t i = 0; i < length; i++) {
		int index;
		sprintf(file, "%s/%s/%s", path, plugin_name, names[i]);
		if (images[i].Create(file)) {
			ret++;
		} else {
			CreateEmptyImage(images[i], 16, 16);
		}
		index = imagelist->Add(images[i]);
		if (!registred) {
			*firstindex = index;
			registred = true;
		}
	}
	free(file);
	return ret;
}
