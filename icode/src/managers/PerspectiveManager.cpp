/*
 * PerspectiveManager.cpp
 *
 *  Created on: 13 oct. 2021
 *      Author: azeddine
 */

#include "../icode.h"
#include "../model/MWindow.h"
PerspectiveFolder::PerspectiveFolder() {
	this->refIndex = -1;
	this->ratio = 0.2;
	this->relationship = PerspectiveManager::M_LEFT;
}

PerspectiveFolder::~PerspectiveFolder() {
}

PerspectiveManager::PerspectiveManager() {
}

PerspectiveManager::~PerspectiveManager() {
}
bool PerspectiveManager::AddViews(XMLElement *element,
		PerspectiveFolder *folder) {
	XMLElement *child = element->FirstChildElement();
	while (child != 0) {
		if (!strcmp(child->Value(), "view")) {
			const XMLAttribute *attr = child->FindAttribute("id");
			if (attr != 0) {
				PerspectivePart *part = folder->parts.Add0();
				if (part != 0) {
					StringId *id = Managers::RegistreString(attr->Value());
					part->id = id;
					part->type = 0;
				}
			}
		}
		child = child->NextSiblingElement();
	}
	return true;
}
bool PerspectiveManager::AddPerspective(Plugin *info, XMLElement *element) {
	const char *_id = 0;
	const XMLAttribute *attr = element->FindAttribute("id");
	if (attr != 0) {
		_id = attr->Value();
	}
	if (_id != 0) {
		PerspectiveItem *item = perspectives.Add0();
		if (item != 0) {
			item->init = 0;
			item->image = 0;
			item->id = Managers::RegistreString(_id);
		}
		XMLElement *child = element->FirstChildElement();
		while (child != 0) {
			if (!strcmp(child->Value(), "folder")) {
				void *_folder = item->folders.Add0();
				PerspectiveFolder *folder = new (_folder) PerspectiveFolder();
				attr = child->FindAttribute("relationship");
				if (attr != 0) {
					const char *relation = attr->Value();
					if (!strcmp(relation, "left")) {
						folder->relationship = M_LEFT;
					}
					if (!strcmp(relation, "right")) {
						folder->relationship = M_RIGHT;
					}
					if (!strcmp(relation, "top")) {
						folder->relationship = M_TOP;
					}
					if (!strcmp(relation, "bottom")) {
						folder->relationship = M_BOTTOM;
					}
				}
				attr = child->FindAttribute("ratio");
				if (attr != 0) {
					const char *_ratio = attr->Value();
					float ratio = strtof_l(_ratio, 0,
							ManagersGet()->GetUSLocal());
					if (ratio != 0) {
						folder->ratio = ratio;
					}
				}
				attr = child->FindAttribute("refId");
				if (attr != 0) {
					const char *refId = attr->Value();
					if (strcmp(refId, "editorArea")) {
						XMLElement *_child = element->FirstChildElement();
						int j = 0;
						while (_child != 0) {
							if (!strcmp(_child->Value(), "folder")) {
								attr = child->FindAttribute("id");
								if (attr != 0) {
									if (!strcmp(refId, attr->Value())) {
										folder->refIndex = j;
										break;
									}
								}
								j++;
							}
							_child = _child->NextSiblingElement();
						}
					}
				}
				AddViews(child, folder);
			}
			child = child->NextSiblingElement();
		}
	}
	return true;
}
bool PerspectiveManager::OnInstall(Plugin *info, XMLElement *element) {
	XMLElement *child = element->FirstChildElement();
	while (child != 0) {
		if (!strcmp(child->Value(), "perspective")) {
			AddPerspective(info, child);
		}
		child = child->NextSiblingElement();
	}
	return true;
}

PerspectiveItem* PerspectiveManager::FindPerspective(StringId *id) {
	PerspectiveItem *items, *item = 0;
	int count = perspectives.GetCount(&items);
	int i;
	for (i = 0; i < count; i++) {
		if (items[i].id == id) {
			return &items[i];
		}
	}
	return 0;
}
struct PerspectiveLoadItems {
	PerspectiveManager *managers;
	PerspectiveItem *item;
	PerspectiveFolder *folder;

};
void PerspectiveManager::LoadItems(Config &parent, void *userdata,
		const char *child) {
	int i = 0;
	while (child[i] != 0) {
		if (child[i] < '0' || child[i] > '9')
			return;
		i++;
	}
	PerspectiveLoadItems *items = (PerspectiveLoadItems*) userdata;
	Config c, c1;
	parent.Open(child, c);
	if (c.IsDir()) {
		int _index = atoi(child);
		items->item = items->managers->perspectives.SetAt(_index);
		if (items->item != 0) {
			c.Open("id", c1);
			if (c1.IsString()) {
				char *_id = c1.GetString();
				if (_id != 0) {
					items->item->id = ManagersGet()->RegistreString(_id);
					free(_id);
				}
			}
			c.Open("image", c1);
			if (c1.IsString()) {
				char *_id = c1.GetString();
				if (_id != 0) {
					items->item->imageid = ManagersGet()->RegistreString(_id);
					free(_id);
				}
			}
			c.ForAll(PerspectiveManager::LoadFolders, userdata);
		}
	}
}
void PerspectiveManager::LoadFolders(Config &parent, void *userdata,
		const char *child) {
	int i = 0;
	while (child[i] != 0) {
		if (child[i] < '0' || child[i] > '9')
			return;
		i++;
	}
	PerspectiveLoadItems *items = (PerspectiveLoadItems*) userdata;
	Config c, c1;
	parent.Open(child, c);
	if (c.IsDir()) {
		int _index = atoi(child);
		items->folder = items->item->folders.SetAt(_index);
		if (items->folder != 0) {
			c.Open("relationship", c1);
			items->folder->relationship = c1.GetInt(W_LEFT);
			c.Open("refIndex", c1);
			items->folder->refIndex = c1.GetInt(0);
			c.Open("ratio", c1);
			items->folder->ratio = c1.GetFloat(0.2);
			c.ForAll(PerspectiveManager::LoadViews, userdata);
		}
	}
}

void PerspectiveManager::LoadViews(Config &parent, void *userdata,
		const char *child) {
	int i = 0;
	while (child[i] != 0) {
		if (child[i] < '0' || child[i] > '9')
			return;
		i++;
	}
	PerspectiveLoadItems *items = (PerspectiveLoadItems*) userdata;
	Config c, c1;
	parent.Open(child, c);
	if (c.IsDir()) {
		c.Open("type", c1);
		if (c1.IsInt()) {
			int type = c1.GetInt(0);
			StringId *id = 0;
			const char *_iid = "id";
			c.Open(_iid, c1);
			if (c1.IsString()) {
				char *part = c1.GetString();
				if (part != 0) {
					id = ManagersGet()->RegistreString(part);
					free(part);
				}
			}
			if (id != 0) {
				int _index = atoi(child);
				PerspectivePart *part = items->folder->parts.SetAt(_index);
				if (part != 0) {
					part->type = type;
					part->id = id;
				}
			}
		}
	}
}
bool PerspectiveManager::OnLoad() {
	Config c;
	this->config.Open("default", c);
	if (!c.IsDir())
		return false;
	PerspectiveLoadItems items;
	memset(&items, 0, sizeof(items));
	items.managers = this;
	c.ForAll(PerspectiveManager::LoadItems, &items);
	return true;
}

bool PerspectiveManager::OnSave() {
	Config c, c1, c2, c3;
	char txt[10];
	this->config.CreateDir("default", c);
	if (!c.IsOk())
		return false;
	PerspectiveItem *items;
	int count = this->perspectives.GetCount(items);
	for (int i = 0; i < count; i++) {
		snprintf(txt, sizeof(txt), "%03d", i);
		c.CreateDir(txt, c1);
		if (c1.IsOk()) {
			c1.CreateString("id").SetString(items[i].id->name);
			if (items[i].imageid != 0) {
				c1.CreateString("image").SetString(items[i].imageid->name);
			}
			PerspectiveFolder *folders;
			int folders_count = items[i].folders.GetCount(folders);
			for (int j = 0; j < folders_count; j++) {
				snprintf(txt, sizeof(txt), "%03d", j);
				c1.CreateDir(txt, c2);
				if (c2.IsOk()) {
					c2.CreateInt("relationship").SetInt(
							folders[j].relationship);
					c2.CreateInt("refIndex").SetInt(folders[j].refIndex);
					c2.CreateFloat("ratio").SetFloat(folders[j].ratio);
					PerspectivePart *parts;
					int views_count = folders[j].parts.GetCount(parts);
					for (int k = 0; k < views_count; k++) {
						snprintf(txt, sizeof(txt), "%03d", k);
						c2.CreateDir(txt, c3);
						if (c3.IsOk()) {
							c3.CreateInt("type").SetInt(parts[k].type);
							c3.CreateString("id").SetString(parts[k].id->name);
						}
					}
				}
			}
		}
	}
	return true;
}
