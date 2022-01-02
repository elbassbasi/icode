/*
 * Ptr.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_CORE_SHAREDPTR_H_
#define ICODE_INCLUDE_CORE_SHAREDPTR_H_
#include "IObject.h"
template<typename T>
class SharedPtr {
public:
	T *obj;
	SharedPtr() {
		obj = 0;
	}
	SharedPtr(T *obj) {
		if (obj != 0)
			obj->AddRef();
		this->obj = obj;
	}
	SharedPtr(const SharedPtr<T> &lp) {
		if (lp.obj != 0)
			lp.obj->AddRef();
		this->obj = lp.obj;
	}
	~SharedPtr() {
		if (this->obj != 0)
			this->obj->Release();
	}
	operator T*() {
		return this->obj;
	}
	T& operator*() {
		return *(this->obj);
	}
	T* operator->() {
		return this->obj;
	}
	void operator=(T *obj) {
		if (obj != 0)
			obj->AddRef();
		if (this->obj != 0)
			this->obj->Release();
		this->obj = obj;
	}
	void operator=(const SharedPtr<T> &lp) {
		if (lp.obj != 0)
			lp.obj->AddRef();
		if (this->obj != 0)
			this->obj->Release();
		this->obj = lp.obj;
	}
	bool IsOk() {
		return this->obj != 0;
	}
	bool operator==(const SharedPtr<T> &lp) {
		return this->obj == lp.obj;
	}
	bool operator==(T *obj) {
		return this->obj != obj;
	}
	bool operator!=(const SharedPtr<T> &lp) {
		return this->obj == lp.obj;
	}
	bool operator!=(T *obj) {
		return this->obj != obj;
	}
};
#endif /* ICODE_INCLUDE_CORE_SHAREDPTR_H_ */
