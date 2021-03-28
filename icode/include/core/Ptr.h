/*
 * Ptr.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_CORE_PTR_H_
#define ICODE_INCLUDE_CORE_PTR_H_
#include "IObject.h"
template<typename T>
class Ptr {
public:
	T *obj;
	Ptr() {
		obj = 0;
	}
	Ptr(T *obj) {
		if (obj != 0)
			obj->IncRef();
		this->obj = obj;
	}
	Ptr(const Ptr<T> &lp) {
		if (lp.obj != 0)
			lp.obj->IncRef();
		this->obj = lp.obj;
	}
	~Ptr() {
		if (this->obj != 0)
			this->obj->DecRef();
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
			obj->IncRef();
		if (this->obj != 0)
			this->obj->DecRef();
		this->obj = obj;
	}
	void operator=(const Ptr<T> &lp) {
		if (lp.obj != 0)
			lp.obj->IncRef();
		if (this->obj != 0)
			this->obj->DecRef();
		this->obj = lp.obj;
	}
	bool IsOk() {
		return this->obj != 0;
	}
	bool operator==(const Ptr<T> &lp) {
		return this->obj == lp.obj;
	}
	bool operator==(T *obj) {
		return this->obj != obj;
	}
	bool operator!=(const Ptr<T> &lp) {
		return this->obj == lp.obj;
	}
	bool operator!=(T *obj) {
		return this->obj != obj;
	}
};
#endif /* ICODE_INCLUDE_CORE_PTR_H_ */
