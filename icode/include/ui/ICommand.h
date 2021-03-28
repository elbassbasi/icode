/*
 * ICommand.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_UI_ICOMMAND_H_
#define ICODE_INCLUDE_UI_ICOMMAND_H_
#include "../core/core.h"
class ExecutionEvent {
public:
	IObject* source;
};
class ICommand: public IObject {
public:
	virtual bool Execute(ExecutionEvent &event) = 0;

};

class Command: public ICommand {
public:
	IObject* QueryInterface(IID Id);
	int IncRef();
	int DecRef();
	bool Execute(ExecutionEvent &event);
private:
	int ref;
};

#endif /* ICODE_INCLUDE_UI_ICOMMAND_H_ */
