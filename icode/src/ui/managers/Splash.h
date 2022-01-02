/*
 * Splash.h
 *
 *  Created on: 22 déc. 2019
 *      Author: azeddine
 */

#ifndef SRC_ICODE_UI_SPLASH_H_
#define SRC_ICODE_UI_SPLASH_H_
#include "../../icode.h"
class Splash: public WFrame, public IProgressMonitor {
public:
	ObjectRef ref;
	//WImage image;
	WFontAuto font;
	WProgressBar progress;
	//WLabel label;
	bool update_finish :1;
	bool update_text :1;
	bool update_progress :1;
	bool update_progress_max :1;
	bool is_opened :1;
	int progress_i;
	int progress_max;
	char text[0x100];
public:
	Splash();
	~Splash();
	void Create();
	void LoadPlugins();
	void UpdateLabelText(const char *format, ...);
public:
	ObjectRef* GetRef(int *tmp);
	Task* CreateTask(Task *parent, const char *name, int totalWork);
	void CloseTask(Task *task);
	void Done(Task *task);
	bool IsCanceled(Task *task);
	void SetCanceled(Task *task, bool value);
	void SetText(Task *task, const char *text);
	void SetWorked(Task *task, int work);
protected:
	bool OnPaint(WPaintEvent &e);
	void OnUpdate();
	void _Update();
	bool OnTimer(WTimerEvent &e);
	void Run(void *args);
};

#endif /* SRC_ICODE_UI_SPLASH_H_ */
