/*
 * Object.cpp
 *
 *  Created on: 16 mai 2020
 *      Author: Azeddine El Bassbasi
 */
#include "../icode.h"

IObject* IObject::QueryInterface(const IID interfaceId) {
	if (interfaceId == IObject::iid_IObject) {
		return this;
	}
	return 0;
}

int IObject::IncRef() {
	return 0;
}

int IObject::DecRef() {
	return 0;
}

int IObject::IncRef(int &ref) {
	if (ref >= 0) {
		return __atomic_fetch_add(&ref, 1, __ATOMIC_SEQ_CST);
	} else
		return ref;
}

int IObject::DecRef(int &ref) {
	if (ref > 1) {
		return __atomic_fetch_sub(&ref, 1, __ATOMIC_SEQ_CST);
	} else if (ref < 0) {
		return ref;
	} else {
		delete this;
		return 0;
	}
}

bool IObject::GetProperty(IID Id,WValue& value) {
	return false;
}

bool IObject::SetProperty(IID Id,WValue& value) {
	return false;
}
