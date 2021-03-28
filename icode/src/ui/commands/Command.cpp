/*
 * Command.cpp
 *
 *  Created on: 17 mai 2020
 *      Author: Azeddine El Bassbasi
 */
#include "Commands.h"


IObject* Command::QueryInterface(IID Id) {
	return IObject::QueryInterface(Id);
}

int Command::IncRef() {
	return IObject::IncRef(this->ref);
}

int Command::DecRef() {
	return IObject::DecRef(this->ref);
}

bool Command::Execute(ExecutionEvent &event) {
	return false;
}

