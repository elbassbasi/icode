/*
 * MWindow.h
 *
 *  Created on: 15 avr. 2020
 *      Author: azeddine
 */

#ifndef ICODE_MODEL_MWINDOW_H_
#define ICODE_MODEL_MWINDOW_H_
#include "MPerspective.h"
class IWindow: public IObject {
public:
	virtual void Close()=0;
};
class MWindow: public IWindow,public WShell {
public:
	CMenu menubar;
	MPerspective* perspective;
	MWindow();
	~MWindow();
	bool Create(int style);
	bool LoadPerspective();
	void UpdateLayout();
	void Close();
	void SwitchToPerspective(MPerspective* perspective);
protected:
	bool OnLayout(WEvent &e);
	bool OnNotify(WEvent &e);
public:
	/*
	 * test
	 */
	void test();
};

#endif /* ICODE_MODEL_MWINDOW_H_ */
