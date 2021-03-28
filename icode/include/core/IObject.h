/*
 * IObject.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_CORE_IOBJECT_H_
#define ICODE_INCLUDE_CORE_IOBJECT_H_
#include "defs.h"
typedef wuint IID;
/**
 * id1 : plugin id
 * id2 : category id
 * id3 : interface id
 */
#define IID_GEN(id1,id2,id3) ((id1 >> 16) | (id2 >> 8) | (id3))

#define IID_CLASS(NAME,id1,id2,id3) static const IID iid_##NAME = IID_GEN(id1,id2,id3);\
				inline static IID __GetIID(){return iid_##NAME;}

#define IID_CLASS_PLATFORM(NAME,id2,id3) IID_CLASS(NAME,0,id2,id3)
#define IID_PROPERTY_PLATFORM(id2,id3) IID_GEN(0,id2,id3)
class ICODE_PUBLIC IObject: public IDestruct {
public:
	IID_CLASS_PLATFORM(IObject,0,0)
	virtual IObject* QueryInterface(IID Id);
	virtual int IncRef();
	virtual int DecRef();
	virtual bool GetProperty(IID Id,WValue& value);
	virtual bool SetProperty(IID Id,WValue& value);
public:
	template<typename T>
	T* QueryInterfaceT() {
		return (T*) QueryInterface(T::__GetIID());
	}
protected:
	static int IncRef(int &ref);
	int DecRef(int &ref);
protected:
	template<typename T>
	T* Cast() {
		return static_cast<T*>(this);
	}
};

#endif /* ICODE_INCLUDE_CORE_IOBJECT_H_ */
