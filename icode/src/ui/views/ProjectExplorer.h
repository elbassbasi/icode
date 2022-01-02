/*
 * ProjectExplorer.h
 *
 *  Created on: 19 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_UI_VIEWS_PROJECTEXPLORER_H_
#define ICODE_UI_VIEWS_PROJECTEXPLORER_H_
#include "../../icode.h"
class ProjectExplorer: public ViewPart, public WTreeView {
private:
	IWorkspace *workspace;
public:
	void CreateControl(WComposite *parent);
	WControl* GetPartControl(WComposite *parent);
	const char* GetTitle();
	void SetFocus();
	void SetWorkspace(IWorkspace *workspace);
	void Refresh(WTreeItem &item, IResource *resource);
	void Refresh(WTreeItem &item);
protected:
	void Expand(WTreeItem &item, IResource *resource);
	bool OnItemExpand(WListEvent &e);
	bool OnItemCollapse(WListEvent &e);
	bool OnItemSelection(WListEvent &e);
	bool OnItemDefaultSelection(WListEvent &e);
	bool OnItemGetText(WListEvent &e);
	bool OnItemDispose(WListEvent &e);
};

#endif /* ICODE_UI_VIEWS_PROJECTEXPLORER_H_ */
