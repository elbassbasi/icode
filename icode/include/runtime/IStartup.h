/*
 * IStartup.h
 *
 *  Created on: 3 déc. 2021
 *      Author: azeddine
 */

#ifndef ICODE_INCLUDE_RUNTIME_ISTARTUP_H_
#define ICODE_INCLUDE_RUNTIME_ISTARTUP_H_
#include "../core/core.h"
class IStartup: public IObject {
public:
	virtual bool Start()=0;
};
template<>
inline const IID __IID<IStartup>() {
	return IID_IStartup;
}
#endif /* ICODE_INCLUDE_RUNTIME_ISTARTUP_H_ */
