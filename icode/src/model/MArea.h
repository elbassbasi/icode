/*
 * MArea.h
 *
 *  Created on: 15 avr. 2020
 *      Author: azeddine
 */

#ifndef ICODE_MODEL_MAREA_H_
#define ICODE_MODEL_MAREA_H_
#include "../icode.h"
class MPerspective;
class MPartSashContainer;

#define DRAG_MINIMUM 50
enum {
	MPART_TYPE_STACK = 0 << 2,
	MPART_TYPE_SASHCONTAINER = 1 << 2,
	MPART_TYPE_AREA = 2 << 2,
	MPART_TYPE_MASK = 3 << 2,
	MPART_TYPE_SHARED = 1 << 4,
	MPART_LEFT = PerspectiveManager::M_LEFT,
	MPART_RIGHT = PerspectiveManager::M_RIGHT,
	MPART_TOP = PerspectiveManager::M_TOP,
	MPART_BOTTOM = PerspectiveManager::M_BOTTOM,
	MPART_MASK = 0x3,
};
class MPart0 {
public:
	MPart0();
	virtual ~MPart0();
	MPart0 *parentPart;
	const char *name;
	wuint64 weight;
	int flags;
	int GetType() {
		return (this->flags & MPART_TYPE_MASK);
	}
	static inline bool IsVertical(int flags) {
		return (flags & MPART_MASK) == MPART_LEFT
				|| (flags & MPART_MASK) == MPART_RIGHT;
	}
	virtual void UpdateBounds(WRect &r)=0;
	virtual void GetPartBounds(WRect &r)=0;
};

class MPartStack: public MPart0, public WTabView, public IFolderLayout {
public:
	int id;
	MPartStack();
	~MPartStack();
	ObjectRef* GetRef(int *tmp);
	void GetPartBounds(WRect &r);
	void UpdateBounds(WRect &r);
	void SetWeight(int weight);
	void AddView(StringId *viewId);
protected:
	bool OnItemClose(WTabEvent &e);
};
class MPartSashContainer: public MPart0, public WSash {
public:
	MPart0 *first;
	MPart0 *last;
	MPartSashContainer();
	~MPartSashContainer();
	void GetPartBounds(WRect &r);
	void GetChildBounds(WRect &r, MPart0 *child);
	void UpdateBounds(WRect &r);
	void SetWeight(wuint64 first, wuint64 last);
protected:
	bool OnSelection(WSashEvent &e);
};

class MArea: public MPart0 {
public:
	MPart0 *root;
	MPartStack *sharedPart;
	MArea(MWindow *parent);
	~MArea();
	MPartStack* FindPartId(MPart0 *part, int refIndex);
	void LoadFolders(PerspectiveFolder *folders, int count);
	MPartStack* Div(MPartStack *parent, float ratio, int flags);
	void GetPartBounds(WRect &r);
	void UpdateBounds();
	void UpdateBounds(WRect &r);
	MWindow* GetWindow() {
		return (MWindow*) this->parentPart;
	}
	MPartStack* GetSharedPart() {
		return this->sharedPart;
	}

	MPartStack* CreateSharedPart();
};

#endif /* ICODE_MODEL_MAREA_H_ */
