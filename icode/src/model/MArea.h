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
#define MPART_TYPE (0xFF)
#define MPART_VERTICAL (1 << 8)
#define MPART_HORIZENTAL (0)
#define MPART_FIRST (0)
#define MPART_LAST (1 << 9)
#define MPART_CONTAINER_SHARED (1 << 10)

#define DRAG_MINIMUM 50
enum {
	MPART_TYPE_UNKNOWN = 0,
	MPART_TYPE_STACK,
	MPART_TYPE_SASHCONTAINER,
	MPART_TYPE_AREA,
};
class MPart0 {
public:
	MPart0();
	virtual ~MPart0();
	MPart0 *parent;
	const char *name;
	int flags;
	int GetType() {
		return (this->flags & MPART_TYPE);
	}
};

class MPartStack: public MPart0, public IFolderLayout, public WTabView {
public:
	int weight;
	MPartStack();
	~MPartStack();
	void GetPartBounds(WRect &r);
	void UpdateBounds(WRect &r);
	void SetWeight(int weight);
	void AddView(const char *viewId);
};
class MPartSashContainer: public MPart0 {
public:
	int weight;
	MPart0 *first;
	MPart0 *last;
	WSash sash;
	MPartSashContainer();
	~MPartSashContainer();
	int GetFirstWeight();
	int GetLastWeight();
	void SetFirstWeight(int weight);
	void SetLastWeight(int weight);
	void GetPartBounds(WRect &r);
	void GetChildBounds(WRect &r, MPart0 *child);
	void UpdateBounds(WRect &r);
	void SetWeight(int first, int last);
};

class MArea: public MPart0 {
public:
	MPart0 *root;
	MPartStack *sharedPart;
	MArea(MPerspective *parent);
	~MArea();
	MPartStack* Div(MPartStack *parent, int flags);
	void GetPartBounds(WRect &r);
	void UpdateBounds(WRect &r);
	bool UpdateSash(WSashEvent *event);
	MPerspective* GetPerspective() {
		return (MPerspective*) this->parent;
	}
	MPartStack* GetSharedPart() {
		return this->sharedPart;
	}

	MPartStack* CreateSharedPart();
	WComposite* GetComposite();
public:
	/*
	 * test
	 */
	void test();
	void testControl(MPartStack *container, const char *name);
	MPartStack* testDiv(MPartStack *parent, const char *name, int flags);
};

#endif /* ICODE_MODEL_MAREA_H_ */
