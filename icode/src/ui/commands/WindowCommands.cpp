/*
 * WindowCommands.cpp
 *
 *  Created on: 13 oct. 2021
 *      Author: azeddine
 */
#include "../../model/MWindow.h"
typedef bool (*ExecuteFunction)(ExecutionEvent &event);
class WindowCommand: public Command {
public:
	WindowCommand(ExecuteFunction function){
		this->function = function;
	}
	ExecuteFunction function;
	bool Execute(ExecutionEvent &event) {
		return function(event);
	}
};
bool QuitExecuteFunction(ExecutionEvent &event) {
	IWindow *window = static_cast<IWindow*>(event.source);
	window->Close();
	return true;
}
bool RestartExecuteFunction(ExecutionEvent &event) {
	IWindow *window = static_cast<IWindow*>(event.source);
	return true;
}
bool FullScreenExecuteFunction(ExecutionEvent &event) {
	IWindow *window = static_cast<IWindow*>(event.source);
	window->SetFullScreen(!window->GetFullScreen());
	return true;
}

IObject* QuitCommand(){
	return new WindowCommand(QuitExecuteFunction);
}
IObject* RestartCommand(){
	return new WindowCommand(RestartExecuteFunction);
}
IObject* FullScreenCommand(){
	return new WindowCommand(FullScreenExecuteFunction);
}
