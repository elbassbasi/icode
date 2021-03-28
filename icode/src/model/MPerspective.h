/*
 * MPerspective.h
 *
 *  Created on: 12 avr. 2020
 *      Author: azeddine
 */

#ifndef ICODE_MODEL_MPERSPECTIVE_H_
#define ICODE_MODEL_MPERSPECTIVE_H_
#include "MArea.h"
class MWindow;
enum {
	MCOOLBAR_TOP = 0, MCOOLBAR_BOTTOM, MCOOLBAR_LEFT, MCOOLBAR_RIGHT
};
class MPerspective: public WComposite, public IPageLayout {
public:
	friend MWindow;
	MWindow *parent;
	WMenu menubar;
	WCoolBar coolbar[4];
	MArea area;
	MPerspective(MWindow *parent);
	~MPerspective();
	bool Create(WComposite *parent);
	WComposite* GetComposite();
	void GetPartBounds(WRect &r);
	void GetAreaBounds(WRect &r);
	void UpdateBounds(WRect &r);
	bool UpdateSash(WSashEvent *event);
	void UpdateLayout();
public:
	IFolderLayout* CreateFolder(int relationship, int ratio);
	IFolderLayout* GetEditorArea();
protected:
	bool OnNotify(WEvent &e);
public:
	/*
	 * test
	 */
	void test();
	void testcoolbar(WCoolBar *coolbar, int style);
};

#endif /* ICODE_MODEL_MPERSPECTIVE_H_ */
