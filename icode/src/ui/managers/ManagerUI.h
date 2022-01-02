/*
 * ManagerUI.h
 *
 *  Created on: 25 oct. 2021
 *      Author: azeddine
 */

#ifndef ICODE_SRC_MANAGERS_MANAGERUI_H_
#define ICODE_SRC_MANAGERS_MANAGERUI_H_
#include "../../icode.h"
class PluginInfoUI {
public:
	unsigned enabled :1;
	const char *name;
	const char *location;
	const char *description;
	const char *author;
	const char *version;
	XMLDocument *document;
};
class ManagerUI: public Command, public WTreeView {
public:
	enum {
		BTN_CHECK_ALL, BTN_UNCHECK_ALL, BTN_OK, BTN_CANCEL, BTN_LAST
	};
	static const char *btnTexts[BTN_LAST];
	WFrame frame;
	WGridLayout layout;
	WTextEdit description;
	WButton btn[BTN_LAST];
	WArray<PluginInfoUI*> plugins;
	w_thread_start fun;
	void *args;
	ManagerUI(w_thread_start fun, void *args);
	~ManagerUI();
	static int InstallPlugins(void *args);
	static int NotifyClose(void *args);
	bool Create(WFrame *parent);
	void CreateColumns();
	void LoadPlugins();
	void LoadPluginName(const char *name, char *tmp, int length,
			IProgressMonitor *monitor);
	bool Execute(ExecutionEvent &event);
	int Open();
protected:
	bool OnItemGetText(WTreeEvent &e);
	bool OnItemGetAttr(WListEvent &e);
	bool OnItemSelection(WTreeEvent &e);
	bool OnItemDispose(WTreeEvent &e);
	bool OnButtonSelection(WEvent &e);
	bool InstallPlugins();
};

#endif /* ICODE_SRC_MANAGERS_MANAGERUI_H_ */
