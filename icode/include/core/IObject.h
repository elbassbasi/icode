/*
 * IObject.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_CORE_IOBJECT_H_
#define ICODE_INCLUDE_CORE_IOBJECT_H_
#include "IID.h"
class ObjectRef {
public:
	int ref;
	ObjectRef() {
		this->ref = -1;
	}
};
class ICODE_PUBLIC IObject: public IDestruct {
public:
	virtual IObject* QueryInterface(const IID Id);
	virtual ObjectRef* GetRef(int *tmp)=0;
	virtual int AddRef();
	virtual int Release();
	virtual bool GetProperty(IID Id, WValue &value);
	virtual bool SetProperty(IID Id, WValue &value);
	void CreateRef() {
		int tmp[2];
		ObjectRef *ref = GetRef(tmp);
		if (ref->ref < 0)
			ref->ref = 0;
	}
	void CreateWeakRef() {
		int tmp[2];
		ObjectRef *ref = GetRef(tmp);
		ref->ref = 0;
	}
public:
	template<typename T>
	T* QueryInterfaceT() {
		return (T*) ((void*) QueryInterface(__IID<T>()));
	}
protected:
	template<typename T>
	T* Cast() {
		return static_cast<T*>(this);
	}
};
template<>
inline const IID __IID<IObject>() {
	return IID_IObject;
}

#endif /* ICODE_INCLUDE_CORE_IOBJECT_H_ */
