/*
 * Plugin.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_RUNTIME_PLUGIN_H_
#define ICODE_INCLUDE_RUNTIME_PLUGIN_H_
#include "../core/core.h"
class Plugin;
class IStream;
class IConfig;
typedef IObject* (*Handler)();
extern "C" {
typedef Plugin* (*_ICodeCreatePlugin)();
}
struct PluginInitData {
	Plugin *next;
	w_module *module;
	char *name;
	int index;
};
class Plugin {
public:
	friend class Managers;
	Plugin();
	virtual ~Plugin();
protected:
	virtual bool OnInit();
	virtual void OnAttach();
	virtual void OnRelease();
	virtual void OnStart();
public:
	virtual const char* GetName();
	virtual IConfig* OpenConfig(const char *config);
	virtual IStream* OpenStream(const char *file);
public:
	bool LoadImage(WImage &image, const char *name);
	bool CreateEmptyImage(WImage& image,int w,int h);
	size_t LoadImages(WImage *images, const char **names, size_t length);
	size_t LoadImagesAndRegistre(WImage *images, const char **names,
			size_t length, int *firstindex);
private:
	union {
		PluginInitData data;
		void *reserved[0x10];
	};
};

#endif /* ICODE_INCLUDE_RUNTIME_PLUGIN_H_ */
