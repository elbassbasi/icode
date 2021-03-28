/*
 * RestartCommand.cpp
 *
 *  Created on: 17 mai 2020
 *      Author: Azeddine El Bassbasi
 */
#include "Commands.h"
class RestartCommand: public Command {
	bool Execute(ExecutionEvent &event) {
		return true;
	}
};
void RestartCommandRegistre() {
	IManagers::Get()->RegistreCommand("Restart", new RestartCommand());
}

