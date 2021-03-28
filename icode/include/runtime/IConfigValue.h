/*
 * IConfigValue.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_RUNTIME_ICONFIGVALUE_H_
#define ICODE_INCLUDE_RUNTIME_ICONFIGVALUE_H_
#include "../core/core.h"
class IConfigValue;
class IConfig;
class IConfigListener;
struct ConfigEvent {
	enum {
		VALUE_UPDATED, CONFIG_UPDATED, CONFIG_DELETED
	};
	int type;
	IConfigValue *valueUpdated;
	IConfig *configUpdated;
	IConfig *config;
	IConfigListener *listener;
};
class IConfigListener: public IDestruct {
public:
	virtual void ConfigUpdate(ConfigEvent *e)=0;
};
class IConfigValue: public IObject {
protected:
	virtual ~IConfigValue();
public:
	enum {
		/*
		 * used in value flags
		 * v.value.flags |= IConfigValue::VALUE_COMPRESSED
		 */
		VALUE_COMPRESSED = 1 << 8,
	};
	virtual bool AddListenner(IConfigListener *listener) = 0;
	virtual bool RemoveListenner(IConfigListener *listener) = 0;
	virtual int Delete()=0;
	virtual const char* GetName() = 0;
	virtual int GetType() = 0;
	virtual int GetSize() = 0;
	virtual int GetValue(WValue &value)=0;
	virtual int SetValue(const WValue &value)=0;
public:
	int GetValue(void *data, size_t size) {
		WValue v;
		v.SetUserPointer(data, size);
		return GetValue(v);
	}
	int SetValue(void *data, size_t size) {
		WValue v;
		v.SetUserPointer(data, size);
		return SetValue(v);
	}
	int SetValueCompressed(void *data, size_t size) {
		WValue v;
		v.SetUserPointer(data, size);
		v.flags |= VALUE_COMPRESSED;
		return SetValue(v);
	}
	int SetValue(const char *string) {
		return SetValue(WValue(string, false));
	}
	int SetValueCompressed(const char *string) {
		WValue v;
		v.SetString(string);
		v.flags |= VALUE_COMPRESSED;
		return SetValue(v);
	}
	void Close(){
		this->DecRef();
	}
};
#endif /* ICODE_INCLUDE_RUNTIME_ICONFIGVALUE_H_ */
