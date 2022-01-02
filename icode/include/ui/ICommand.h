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
template<>
inline const IID __IID<ICommand>() {
	return IID_ICommand;
}
class Command: public ICommand {
public:
	IObject* QueryInterface(IID Id);
	ObjectRef* GetRef(int *tmp);
	bool Execute(ExecutionEvent &event);
private:
	ObjectRef ref;
};

#endif /* ICODE_INCLUDE_UI_ICOMMAND_H_ */
