/*
 * QuitCommand.cpp
 *
 *  Created on: 17 mai 2020
 *      Author: Azeddine El Bassbasi
 */
#include "Commands.h"
#include "../../model/MWindow.h"

class QuitCommand: public Command {
	bool Execute(ExecutionEvent &event) {
		IWindow *window = static_cast<IWindow*>(event.source);
		window->Close();
		return true;
	}
};
void QuitCommandRegistre() {
	IManagers::Get()->RegistreCommand("Quit", new QuitCommand());
}
