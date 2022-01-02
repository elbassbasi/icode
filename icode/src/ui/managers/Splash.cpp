/*
 * Splash.cpp
 *
 *  Created on: 22 déc. 2019
 *      Author: azeddine
 */

#include "Splash.h"
Splash::Splash() {
	this->progress_max = 0;
	this->progress_i = 0;
	this->update_finish = false;
	this->update_text = false;
	this->update_progress = false;
	this->update_progress_max = false;
	this->is_opened = false;
}

Splash::~Splash() {
}

void Splash::Create() {
	WShell::Create(W_NO_TRIM | W_DISPOSE_ON_CLOSE | W_DELETE_ON_DISPOSE);
	WSize size(450, 250);
	WRect rect, r;
	WFont *font = this->GetFont();
	WFontData data;
	font->GetFontData(data);
	data.SetHeight(30);
	this->font.Create(data);
	progress.Create(this, 0);
	//label.Create(this, W_CENTER);
	WApp::Get()->GetDefaultToolkit()->GetBounds(r);
	size.height += 50;
	this->SetBounds((r.width - size.width) / 2, (r.height - size.height) / 2,
			size.width, size.height);
	//this->ComputeTrim(rect, 0, 0, size.width, size.height);
	//this->SetBounds(100, 100, rect.width, rect.height);
	this->GetClientArea(rect);
	progress.SetBounds(0, rect.height - 25, rect.width, 20);
	//label.SetBounds(0, rect.height - 50, rect.width, 20);
	SetTimer(50);
}

void Splash::UpdateLabelText(const char *format, ...) {
	va_list args;
	va_start(args, format);
	vsnprintf(this->text, sizeof(this->text), format, args);
	va_end(args);
	this->update_text = true;
	this->_Update();
}

bool Splash::OnPaint(WPaintEvent &e) {
	WRect r;
	GetClientArea(r);
	e->SetBackground(W_COLOR_BLACK);
	e->FillRectangle(r);
	e->SetForeground(W_COLOR_WHITE);
	//e->SetFont(&this->font1);
	e->SetFont(WApp::Get()->GetDefaultToolkit()->GetSystemFont());
	e->DrawTextTransparent(this->text, r.x, r.height - 50);
	e->SetFont(this->font);
	e->SetForeground(W_COLOR_BLUE);
	e->DrawTextTransparent("ICode Editor", (r.x + r.width) / 2,
			(r.y + r.height) / 2);
	e->SetForeground(W_COLOR_WHITE);
	e->SetFont(WApp::Get()->GetDefaultToolkit()->GetSystemFont());
	e->DrawTextTransparent("Intelligent code editor", (r.x + r.width) / 2,
			(r.y + r.height) / 2 + 50);
	return true;
}

void Splash::LoadPlugins() {
	Managers *managers = Managers::Get();
	this->progress_max = 20;
	this->update_progress_max = true;
	this->_Update();
	for (int i = 0; i < this->progress_max; i++) {
		WThread::Sleep(1);
		this->progress_i = i;
		this->update_progress = true;
		UpdateLabelText("Loading Plugin %d", i);
	}
	this->update_finish = true;
	this->_Update();
}

void Splash::OnUpdate() {
	bool finish = true;
	if (this->update_text) {
		//label.SetText(this->text);
		this->update_text = false;
		finish = false;
		this->Redraw();
	}
	if (this->update_progress_max) {
		progress.SetMaximum(this->progress_max);
		this->update_progress_max = false;
		finish = false;
	}
	if (this->update_progress) {
		progress.SetSelection(this->progress_i);
		this->update_progress = false;
		finish = false;
	}
	if (this->update_finish && finish == true) {
		Managers *managers = Managers::Get();
		this->KillTimer();
		this->Close();
	}
}

bool Splash::OnTimer(WTimerEvent &e) {
	if (this->is_opened == false) {
		if (!this->update_finish)
			this->Open();
		this->is_opened = true;
	}
	OnUpdate();
	return true;
}

void Splash::_Update() {
	//AsyncExec(0);
}

ObjectRef* Splash::GetRef(int *tmp) {
	return &this->ref;
}

Splash::Task* Splash::CreateTask(Task *parent, const char *name,
		int totalWork) {
}

void Splash::CloseTask(Task *task) {
}

void Splash::Done(Task *task) {
}

bool Splash::IsCanceled(Task *task) {
}

void Splash::SetCanceled(Task *task, bool value) {
}

void Splash::SetText(Task *task, const char *text) {
}

void Splash::SetWorked(Task *task, int work) {
}

void Splash::Run(void *args) {
	LoadPlugins();
}
IObject* SplashHandler() {
	return new Splash();
}
