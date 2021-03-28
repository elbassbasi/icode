/*
 * IConfig.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_RUNTIME_ICONFIG_H_
#define ICODE_INCLUDE_RUNTIME_ICONFIG_H_
#include "IConfigValue.h"
class IConfig: public IObject {
protected:
	virtual ~IConfig();
public:
	virtual int Delete()=0;
	virtual bool AddListenner(IConfigListener *listener) = 0;
	virtual bool RemoveListenner(IConfigListener *listener) = 0;
	virtual const char* GetName() = 0;
	virtual int OpenValue(const char *name, IConfigValue **configvalue)=0;
	virtual int CreateValue(const char *name, const WValue &value,
			IConfigValue **configvalue)=0;
	virtual int GetValue(const char *name, WValue &value)=0;
	virtual int SetValue(const char *name, const WValue &value)=0;
	virtual void GetValues(WIterator<IConfigValue*> &values)=0;
	virtual int OpenChild(const char *key, IConfig **config)=0;
	virtual int CreateChild(const char *name, IConfig **config)=0;
	virtual void GetChildren(WIterator<IConfig*> &children)=0;
public:
	inline int OpenValue(const char *name, Ptr<IConfigValue> &configvalue) {
		return OpenValue(name, &configvalue.obj);
	}
	inline int CreateValue(const char *name, const WValue &value,
			Ptr<IConfigValue> &configvalue) {
		return CreateValue(name, value, &configvalue.obj);
	}
	inline int OpenChild(const char *key, Ptr<IConfig> &config) {
		return OpenChild(key, &config.obj);
	}
	inline int CreateChild(const char *name, Ptr<IConfig> &config) {
		return CreateChild(name, &config.obj);
	}
	int GetValue(const char *name, WValue &value, const WValue &defvalue) {
		int ret = GetValue(name, value);
		if (ret < 0) {
			value = defvalue;
		}
		return ret;
	}
};
class IConfigManager: public IDestruct {
public:
	virtual int Open(const char *configuration, IConfig **config)=0;
	virtual int Create(const char *configuration, IConfig **config)=0;
	virtual int GetRoot(IConfig **config)=0;
public:
	inline int Open(const char *configuration, Ptr<IConfig> &config) {
		if (config.obj != 0)
			config.obj->DecRef();
		return Open(configuration, &config.obj);
	}
	inline int Create(const char *configuration, Ptr<IConfig> &config) {
		if (config.obj != 0)
			config.obj->DecRef();
		return Create(configuration, &config.obj);
	}
	inline int GetRoot(Ptr<IConfig> &config) {
		if (config.obj != 0)
			config.obj->DecRef();
		return GetRoot(&config.obj);
	}
	inline Ptr<IConfig> Open(const char *configuration) {
		IConfig *config;
		Open(configuration, &config);
		return config;
	}
	inline Ptr<IConfig> Create(const char *configuration) {
		IConfig *config;
		Create(configuration, &config);
		return config;
	}
	inline Ptr<IConfig> GetRoot() {
		IConfig *config;
		GetRoot(&config);
		return config;
	}

};
#endif /* ICODE_INCLUDE_RUNTIME_ICONFIG_H_ */
