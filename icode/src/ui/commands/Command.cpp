/*
 * Command.cpp
 *
 *  Created on: 17 mai 2020
 *      Author: Azeddine El Bassbasi
 */
#include "../../icode.h"

IObject* Command::QueryInterface(IID Id) {
	if (Id == IID_ICommand) {
		return static_cast<ICommand*>(this);
	}
	return IObject::QueryInterface(Id);
}

bool Command::Execute(ExecutionEvent &event) {
	return false;
}

ObjectRef* Command::GetRef(int *tmp) {
	return &this->ref;
}
