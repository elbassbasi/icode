/*
 * ManagerUI.cpp
 *
 *  Created on: 25 oct. 2021
 *      Author: azeddine
 */

#include "ManagerUI.h"
const char *ManagerUI::btnTexts[BTN_LAST] = { //
		"Check All", "UnCheck All", "Ok", "Cancel" //
		};

ManagerUI::ManagerUI(w_thread_start fun, void *args) {
	this->fun = fun;
	this->args = args;
}

ManagerUI::~ManagerUI() {
}

bool ManagerUI::Create(WFrame *parent) {
	bool ret = frame.Create(parent, W_FRAME_TRIM | W_DISPOSE_ON_CLOSE);
	if (ret) {
		layout.numColumns = BTN_LAST;
		frame.SetLayout(layout);
		WTreeView::Create(&frame, W_CHECK | W_FULL_SELECTION | W_VIRTUAL);
		CreateColumns();
		description.Create(&frame, W_MULTI | W_READ_ONLY);
		for (int i = 0; i < BTN_LAST; i++) {
			btn[i].CreatePush(&frame, btnTexts[i]);
			btn[i].SetId(i + 1);
			btn[i].SetSelectionAction(this,
					W_ACTION(ManagerUI::OnButtonSelection));
		}
		this->SetLayoutData(WGridData(W_GRID_FILL_BOTH, layout.numColumns, 1));
		description.SetLayoutData(
				WGridData(W_GRID_FILL_BOTH, layout.numColumns, 1));
	}
	return ret;
}

void ManagerUI::CreateColumns() {
	this->GetColumn(0).SetText("Name");
	WColumnItem column;
	this->AppendColumn(column).SetText("Version");
	this->AppendColumn(column).SetText("Author");
	this->SetHeaderVisible(true);
}

void ManagerUI::LoadPlugins() {
	const char *currentPath = ManagersGet()->GetCurrentPath();
	int length = strlen(currentPath);
	char *tmp = (char*) malloc(length + 0x200);
	if (tmp == 0)
		return;
	sprintf(tmp, "%s/plugins", currentPath);
	WIterator<WFindDir> it;
	WFindDir find;
	WDirList(tmp, it);
	while (it.Next(find)) {
		if (strcmp(find.name, ".") && strcmp(find.name, "..")) {
			LoadPluginName(find.name, tmp, length, 0);
		}
	}
	free(tmp);
}

void ManagerUI::LoadPluginName(const char *name, char *tmp, int length,
		IProgressMonitor *monitor) {
	XMLDocument *document = 0;
	Zip *zip = 0;
	sprintf(&tmp[length], "/plugins/%s/plugin.xml", name);
	FILE *file = fopen(tmp, "r");
	if (file != 0) {
		document = new XMLDocument();
		XMLError err = document->LoadFile(tmp);
		if (err != XML_SUCCESS) {
			delete document;
			document = 0;
		}
		fclose(file);
	} else {
		// is zip file
		sprintf(&tmp[length], "plugins/%s", name);
	}
	if (document != 0) {
		PluginInfoUI *p = new PluginInfoUI();
		p->document = document;
		p->name = "icode";
		p->version = "0.1";
		p->author = "";
		p->description = "icode";
		p->enabled = true;
		p->location = strdup(name);
		WTreeItem item;
		this->AppendItem(item, 0);
		item.SetData(p);
		item.SetChecked(true);
		plugins.Add(p);
	}
}

bool ManagerUI::Execute(ExecutionEvent &event) {
	WFrame *parent = 0;
	if (event.source != 0) {

	}
	this->Create(parent);
	LoadPlugins();
	this->frame.Open();
	WApp::Get()->Run(&this->frame);
	return true;
}
int ManagerUI::Open() {
	this->Create(0);
	LoadPlugins();
	this->frame.Open();
	return true;
}
bool ManagerUI::OnItemGetText(WTreeEvent &e) {
	PluginInfoUI *p = (PluginInfoUI*) e.GetListItem()->GetData();
	if (p != 0) {
		switch (e.GetColumnIndex()) {
		case 0:
			e.SetAttrText(p->name);
			break;
		case 1:
			e.SetAttrText(p->version);
			break;
		case 2:
			e.SetAttrText(p->author);
			break;
		}
	}
	return false;
}

bool ManagerUI::OnItemGetAttr(WListEvent &e) {
	return false;
}

bool ManagerUI::OnItemSelection(WTreeEvent &e) {
	PluginInfoUI *p = (PluginInfoUI*) e.GetListItem()->GetData();
	if (p != 0) {
		p->enabled = e.GetListItem()->GetChecked();
		this->description.SetText(p->description);
	}
	return false;
}

bool ManagerUI::OnItemDispose(WTreeEvent &e) {
	PluginInfoUI *p = (PluginInfoUI*) e.GetListItem()->GetData();
	e.GetListItem()->SetData(0);
	if (p != 0) {
		delete p;
	}
	return true;
}

int ManagerUI::InstallPlugins(void *args) {
	ManagerUI *ui = reinterpret_cast<ManagerUI*>(args);
	return ui->InstallPlugins();
}

bool ManagerUI::OnButtonSelection(WEvent &e) {
	int id = e.widget->GetId() - 1;
	switch (id) {
	case BTN_OK: {
		ManagersGet()->AddTask(ManagerUI::InstallPlugins, this);
	}
		break;
	default:
		break;
	}
	return true;
}

int ManagerUI::NotifyClose(void *args) {
	ManagerUI *ui = (ManagerUI*) args;
	ui->frame.Close();
	return true;
}

bool ManagerUI::InstallPlugins() {
	PluginInfoUI **p;
	const char *currentPath = ManagersGet()->GetCurrentPath();
	char *pluginsPath = (char*) malloc(strlen(currentPath) + 0x100);
	if (pluginsPath == 0)
		return false;
	sprintf(pluginsPath, "%s/plugins", currentPath);
	int count = plugins.GetCount(p);
	for (int i = 0; i < count; i++) {
		ManagersGet()->InstallPlugin(pluginsPath, p[i]->location, 0);
	}
	free(pluginsPath);
	this->fun(this->args);
	ManagersGet()->AsyncExec(NotifyClose, this);
	return true;
}
