/*
 * MWindow.h
 *
 *  Created on: 15 avr. 2020
 *      Author: azeddine
 */

#ifndef ICODE_MODEL_MWINDOW_H_
#define ICODE_MODEL_MWINDOW_H_
#include "MArea.h"
class IWindow: public IObject {
public:
	virtual void Close()=0;
	virtual bool SetFullScreen(bool fullScren)=0;
	virtual bool GetFullScreen()=0;
};
class MWindow: public WFrame, public IWindow, public IPageLayout {
public:
	enum {
		MCOOLBAR_TOP = 0, MCOOLBAR_BOTTOM, MCOOLBAR_LEFT, MCOOLBAR_RIGHT
	};
	ObjectRef ref;
	CMenu menubar;
	WCoolBar coolbar[4];
	WSize coolbarPreferedSize[4];
	PerspectiveItem *current;
	MArea area;
	WArray<IWorkbenchPart*> parts;
	MWindow();
	~MWindow();
	bool Create(int style);
	ObjectRef* GetRef(int *tmp);
	bool LoadPerspective();
	void UpdateAll();
	void UpdateCoolBar();
	void Close();
	void SwitchToPerspective(PerspectiveItem *perspective);
	void GetAreaRect(WRect &r);
	void ClearPreferedSize();
	bool GetCoolBarSize(int index,WSize& size);
protected:
	bool OnLayout(WEvent &e);
	bool OnClose(WEvent &e);
	void OnDispose(WEvent &e);
public:
	IFolderLayout* CreateFolder(int relationship, int ratio);
	IFolderLayout* GetEditorArea();
	bool SetFullScreen(bool fullScren);
	bool GetFullScreen();
};

#endif /* ICODE_MODEL_MWINDOW_H_ */
