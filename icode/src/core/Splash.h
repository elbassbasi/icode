/*
 * Splash.h
 *
 *  Created on: 22 déc. 2019
 *      Author: azeddine
 */

#ifndef SRC_ICODE_UI_SPLASH_H_
#define SRC_ICODE_UI_SPLASH_H_
#include "../icode.h"
class Splash: public WShell, public IWRunnable {
public:
	WThread backgroundThread;
	//WImage image;
	WFont font;
	WProgressBar progress;
	//WLabel label;
	bool update_finish :1;
	bool update_text :1;
	bool update_progress :1;
	bool update_progress_max :1;
	int progress_i;
	int progress_max;
	char text[0x100];
public:
	Splash();
	~Splash();
	void Create();
	void LoadPlugins();
	void UpdateLabelText(const char *format, ...);
protected:
	bool OnPaint(WPaintEvent &e);
	void OnUpdate();
	void _Update();
	bool OnTimer(WTimerEvent& e);
	void Run();
};

#endif /* SRC_ICODE_UI_SPLASH_H_ */
