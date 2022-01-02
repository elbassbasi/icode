/*
 * Object.cpp
 *
 *  Created on: 16 mai 2020
 *      Author: Azeddine El Bassbasi
 */
#include "../icode.h"

IObject* IObject::QueryInterface(const IID interfaceId) {
	if (interfaceId == IID_IObject) {
		return this;
	}
	return 0;
}

int IObject::AddRef() {
	int tmp[2];
	ObjectRef *ref = GetRef(tmp);
	if (ref->ref >= 0) {
		return __atomic_fetch_add(&ref->ref, 1, __ATOMIC_SEQ_CST);
	} else
		return ref->ref;
}

int IObject::Release() {
	int tmp[2];
	ObjectRef *ref = GetRef(tmp);
	if (ref->ref > 1) {
		return __atomic_fetch_sub(&ref->ref, 1, __ATOMIC_SEQ_CST);
	} else if (ref->ref < 0) {
		return ref->ref;
	} else {
		delete this;
		return 0;
	}
}
bool IObject::GetProperty(IID Id, WValue &value) {
	return false;
}

bool IObject::SetProperty(IID Id, WValue &value) {
	return false;
}
