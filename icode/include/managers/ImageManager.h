/*
 * ImageManager.h
 *
 *  Created on: 11 oct. 2021
 *      Author: azeddine
 */

#ifndef ICODE_INCLUDE_MANAGERS_IMAGEMANAGER_H_
#define ICODE_INCLUDE_MANAGERS_IMAGEMANAGER_H_
#include "Manager.h"
class ImageManagerItem: public AVLPtrKey<StringId> {
public:
	ImageManagerItem();
	~ImageManagerItem();
	bool IsOk();
	Plugin *plugin;
	char *location16;
	char *location32;
	union {
		int flags;
		struct {
			bool isImage16Loaded :1;
			bool isImage32Loaded :1;
			bool isImage16Index :1;
		};
	};
	int index16;
	int index32;
	WImage image;
	WImage image32;
};
class ImageManager: public Manager {
public:
	enum {
		LOAD_IMAGE16 = 1 << 0, LOAD_IMAGE32 = 1 << 1,
	};
	WImageList imagelist16;
	WImageList imagelist32;
	ImageManagerItem *images;
	ImageManager();
	~ImageManager();
	void InitImageList();
	virtual ImageManagerItem* FindImageItem(StringId *id, int flags);
	bool OnInstall(Plugin *info, XMLElement *element);
	bool LoadImage(Plugin *info, const char *location, WImage &image);
public:
	inline WImageList* GetImageList16() {
		return &this->imagelist16;
	}
	inline WImageList* GetImageList32() {
		return &this->imagelist32;
	}
	inline WImage* FindImage(StringId *id) {
		ImageManagerItem *_id = FindImageItem(id, LOAD_IMAGE16);
		if (_id != 0 && _id->image.IsOk())
			return &_id->image;
		else
			return 0;
	}
	inline WImage* FindImage32(StringId *id) {
		ImageManagerItem *_id = FindImageItem(id, LOAD_IMAGE32);
		if (_id != 0 && _id->image32.IsOk())
			return &_id->image32;
		else
			return 0;
	}
	inline int FindImageIndex(StringId *id) {
		ImageManagerItem *_id = FindImageItem(id, LOAD_IMAGE16);
		if (_id != 0)
			return _id->index16;
		else
			return -1;
	}
	inline int FindImage32Index(StringId *id) {
		ImageManagerItem *_id = FindImageItem(id, LOAD_IMAGE32);
		if (_id != 0)
			return _id->index32;
		else
			return -1;
	}
protected:
	static void LoadImages(Config &parent, void *userdata, const char *child);
	ImageManagerItem* NewImageId(StringId *id);
	bool OnInit(Plugin *plugin);
	bool OnLoad();
	bool OnSave();
};

#endif /* ICODE_INCLUDE_MANAGERS_IMAGEMANAGER_H_ */
