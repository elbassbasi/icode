/*
 * PerspectiveManager.h
 *
 *  Created on: 13 oct. 2021
 *      Author: azeddine
 */

#ifndef ICODE_INCLUDE_MANAGERS_PERSPECTIVEMANAGER_H_
#define ICODE_INCLUDE_MANAGERS_PERSPECTIVEMANAGER_H_
#include "Managers.h"
class IPageLayout;
class MWindow;
typedef void (*PerspectiveInit)(IPageLayout *layout);
struct PerspectivePart {
	int type;
	union {
		StringId *id;
		char *name;
	};
};
struct PerspectiveFolder {
	PerspectiveFolder();
	~PerspectiveFolder();
	int relationship;
	int refIndex;
	float ratio;
	WArray<PerspectivePart> parts;
};
struct PerspectiveItem {
public:
	StringId *id;
	StringId *imageid;
	PerspectiveInit init;
	int image;
	WArray<PerspectiveFolder> folders;
};
class PerspectiveManager: public Manager {
public:
	enum {
		M_LEFT, M_RIGHT, M_TOP, M_BOTTOM
	};
	WArray<PerspectiveItem> perspectives;
	PerspectiveManager();
	~PerspectiveManager();
	bool AddViews(XMLElement *element, PerspectiveFolder *folder);
	bool AddPerspective(Plugin *info, XMLElement *element);
	PerspectiveItem* FindPerspective(StringId *id);
	PerspectiveItem* FindPerspective(const char *id) {
		return FindPerspective(Managers::FindString(id));
	}
protected:
	static void LoadItems(Config &parent, void *userdata, const char *child);
	static void LoadFolders(Config &parent, void *userdata, const char *child);
	static void LoadViews(Config &parent, void *userdata, const char *child);
	bool OnInstall(Plugin *info, XMLElement *element);
	bool OnLoad();
	bool OnSave();
};

#endif /* ICODE_INCLUDE_MANAGERS_PERSPECTIVEMANAGER_H_ */
