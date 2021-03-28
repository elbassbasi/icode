/*
 * FullScreenCommand.cpp
 *
 *  Created on: 17 mai 2020
 *      Author: Azeddine El Bassbasi
 */
#include "Commands.h"
#include "../../model/MWindow.h"
class FullScreenCommand : public Command {
public:
	bool Execute(ExecutionEvent &event) {
		MWindow *window = static_cast<MWindow*>(event.source);
		window->SetFullScreen(!window->GetFullScreen());
		return true;
	}
};
void FullScreenCommandRegistre() {
	IManagers::Get()->RegistreCommand("FullScreen", new FullScreenCommand());
}



