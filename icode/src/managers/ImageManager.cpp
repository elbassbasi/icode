/*
 * ImageManager.cpp
 *
 *  Created on: 11 oct. 2021
 *      Author: azeddine
 */

#include "../icode.h"

ImageManager::ImageManager() {
	this->images = 0;
}

ImageManager::~ImageManager() {
}

void ImageManager::InitImageList() {
	imagelist16.Create(16, 16, 16);
	imagelist32.Create(32, 32, 16);
}
ImageManagerItem* ImageManager::FindImageItem(StringId *id, int flags) {
	AVLNode *node = this->images->Find(id);
	if (node == 0)
		return 0;
	ImageManagerItem *item = static_cast<ImageManagerItem*>(node);
	if (flags & LOAD_IMAGE16) {
		if (item->isImage16Loaded == false) {
			LoadImage(item->plugin, item->location16, item->image);
			item->isImage16Loaded = true;
			if (item->isImage16Index) {
				item->index16 = this->imagelist16.Add(item->image);
			}
		}
	}
	if (flags & LOAD_IMAGE32) {
		if (item->isImage32Loaded == false) {
			LoadImage(item->plugin, item->location32, item->image32);
			item->isImage32Loaded = true;
			item->index32 = this->imagelist32.Add(item->image32);
		}
	}
	return item;
}
ImageManagerItem* ImageManager::NewImageId(StringId *id) {
	AVLNode *node = this->images->Find(id);
	if (node != 0)
		return 0;
	ImageManagerItem *item = new ImageManagerItem();
	item->key = id;
	node = this->images->Insert(item);
	return item;
}
bool ImageManager::OnInstall(Plugin *info, XMLElement *element) {
	XMLElement *child = element->FirstChildElement();
	while (child != 0) {
		if (!strcmp(child->Value(), "image")) {
			const char *_id = 0;
			const XMLAttribute *attr = child->FindAttribute("id");
			if (attr != 0) {
				_id = attr->Value();
			}
			if (_id != 0) {
				StringId *id = Managers::RegistreString(_id);
				if (id != 0) {
					ImageManagerItem *imageId = NewImageId(id);
					if (imageId != 0) {
						imageId->plugin = info;
						attr = child->FindAttribute("location16");
						if (attr == 0) {
							attr = child->FindAttribute("location");
						} else
							imageId->isImage16Index = true;
						if (attr != 0) {
							const char *_loc = attr->Value();
							if (_loc != 0) {
								imageId->location16 = strdup(_loc);
							}
						}
						attr = child->FindAttribute("location32");
						if (attr != 0) {
							const char *_loc = attr->Value();
							if (_loc != 0) {
								imageId->location32 = strdup(_loc);
							}
						}
					}
				}
			}
		}
		child = child->NextSiblingElement();
	}
	return true;
}
bool ImageManager::LoadImage(Plugin *info, const char *location,
		WImage &image) {
	IStream *stream = info->OpenStream(location);
	if (stream != 0) {
		return image.Create((IWStream*) stream);
	}
	return false;
}
ImageManagerItem::ImageManagerItem() {
	this->key = 0;
	this->index16 = -1;
	this->index32 = -1;
	this->location16 = 0;
	this->location32 = 0;
	this->plugin = 0;
	this->flags = 0;
}

ImageManagerItem::~ImageManagerItem() {
	if (this->location16 != 0) {
		free(this->location16);
	}
	if (this->location32 != 0) {
		free(this->location32);
	}
}

bool ImageManagerItem::IsOk() {
	if (image.IsOk() || image32.IsOk())
		return true;
	else
		return false;
}
bool ImageManager::OnInit(Plugin *plugin) {
	bool ret = Manager::OnInit(plugin);
	InitImageList();
	return ret;
}
void ImageManager::LoadImages(Config &parent, void *userdata,
		const char *child) {
	StringId *id = Managers::RegistreString(child);
	if (id != 0) {
		ImageManager *manager = (ImageManager*) userdata;
		ImageManagerItem *item = manager->NewImageId(id);
		if (item != 0) {
			Config c;
			parent.Open(child, c);
			if (c.IsOk()) {
				item->location16 = c.Open("location16").GetString();
				if (item->location16 != 0) {
					item->isImage16Index = true;
				} else {
					item->location16 = c.Open("location").GetString();
				}
				item->location32 = c.Open("location32").GetString();
				char *plugin = c.Open("plugin").GetString();
				if (plugin != 0) {
					item->plugin = ManagersGet()->FindPlugin(plugin);
					free(plugin);
				}
			}
		}
	}
}
bool ImageManager::OnLoad() {
	this->config.ForAll(LoadImages, this);
	return true;
}

bool ImageManager::OnSave() {
	Config c;
	AVLNode *images = this->images->GetFirst();
	while (images != 0) {
		ImageManagerItem *_image = static_cast<ImageManagerItem*>(images);
		this->config.CreateDir(_image->key->name, c);
		if (c.IsOk()) {
			if (_image->location16 != 0) {
				const char *_loc16 =
						_image->isImage16Index ? "location16" : "location";
				c.CreateString(_loc16).SetString(_image->location16);
			}
			if (_image->location32 != 0) {
				c.CreateString("location32").SetString(_image->location32);
			}
			c.CreateString("plugin").SetString(_image->plugin->GetID()->name);
		}
		images = images->GetNext();
	}
	return true;
}
