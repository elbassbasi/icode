/*
 * ProjectExplorer.h
 *
 *  Created on: 19 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_UI_VIEWS_PROJECTEXPLORER_H_
#define ICODE_UI_VIEWS_PROJECTEXPLORER_H_
#include "views.h"
class ProjectExplorer: public ViewPart, public WTreeView {
public:
	void CreateControl(WComposite *parent);
	WControl* GetPartControl(WComposite *parent);
	const char* GetTitle();
	void test();

};

#endif /* ICODE_UI_VIEWS_PROJECTEXPLORER_H_ */
