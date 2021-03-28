/*
 * DBConfig.h
 *
 *  Created on: 12 nov. 2019
 *      Author: azeddine
 */

#ifndef SRC_ICODE_CORE_DB_DBCONFIG_H_
#define SRC_ICODE_CORE_DB_DBCONFIG_H_
#include "../icode.h"
#include "DB.h"

#define MAX_NAME_LENGTH 0xFF
class DBConfigManager;
class DBConfig;
class DBConfigValue;
struct DBTreeH {
	DBPage parent;
	DBPage page;
	DBPage prev;
	//header
	DBPage next; // 0*sizeof(DBPage)
	inline void* GetHeader() {
		return &this->next;
	}
};
struct DBTreeHC: public DBTreeH {
	DBPage children; // 1*sizeof(DBPage)
	DBPage values; // 2*sizeof(DBPage)
	unsigned char name_length;
	char name[0];
	inline int GetSize() {
		return (3 * sizeof(DBPage)) + 1 * sizeof(unsigned char);
	}
};
struct _DBTreeHC: public DBTreeHC {
	char tmp[MAX_NAME_LENGTH + 1];
};
struct DBTreeHV: public DBTreeH {
	union {
		DBPage value;
		int int_;
		float float_;
	};
	unsigned char type;
	unsigned char name_length;
	char name[0];
	inline int GetSize() {
		return 2 * sizeof(DBPage) + 2 * sizeof(unsigned char);
	}
};
struct _DBTreeHV: public DBTreeHV {
	char tmp[MAX_NAME_LENGTH + 1];
};
enum {
	DB_FLAGS_CREATE = 1 << 0, DB_FLAGS_ADD = 1 << 1,
};

struct DBListeners {
	int alloc;
	int length;
	IConfigListener *listeners[0];
};

class DBConfig: public IConfig {
public:
	DBConfigManager *manager;
	DBConfig *parent;
	DBConfig *next;
	DBConfig *firstChild;
	DBConfigValue *values;
	IConfigListener *firstListener;
	DBListeners *listeners;
	DBTreeHC header;
	int ref;
	unsigned char flags;
	char name[0];
public:
	DBConfig();
public:
	static DBConfig* Create(DBConfig* parent,DBConfig* prev,int name_length);
	~DBConfig();
	void Free();
	IObject* QueryInterface(const IID interfaceId);
	int IncRef();
	int DecRef();
	int Delete();
	bool AddListenner(IConfigListener *listener);
	bool RemoveListenner(IConfigListener *listener);
	const char* GetName();
	int OpenValue(const char *name, IConfigValue **configvalue);
	int CreateValue(const char *name, const WValue &value,
			IConfigValue **configvalue);
	int GetValue(const char *name, WValue &value);
	int SetValue(const char *name, const WValue &value);
	void GetValues(WIterator<IConfigValue*> &values);
	int OpenChild(const char *key, IConfig **config);
	int CreateChild(const char *name, IConfig **config);
	void GetChildren(WIterator<IConfig*> &children);
public:
	void CheckFree();
	int _OpenChild(const char *key, DBConfig **config, int flags);
	int _OpenValue(const char *key, DBConfigValue **value, int flags);
};
class DBConfigTmp: public DBConfig {
public:
	char tmp[MAX_NAME_LENGTH + 1];
};
class DBConfigValue: public IConfigValue {
public:
	static DBConfigValue* Create(DBConfig* parent,DBConfigValue* prev,int name_length);
	DBConfig *parent;
	DBConfigValue *next;
	IConfigListener *firstListener;
	DBListeners *listeners;
	int ref;
	uint size;
	DBTreeHV header;
	unsigned char flags;
	char name[0];
public:
	DBConfigValue();
public:
	~DBConfigValue();
	void Free();
	IObject* QueryInterface(const IID interfaceId);
	int IncRef();
	int DecRef();
	int Delete();
	bool AddListenner(IConfigListener *listener);
	bool RemoveListenner(IConfigListener *listener);
	const char* GetName();
	int GetType();
	int GetSize();
	int GetValue(WValue &value);
	int SetValue(const WValue &value);
public:
	void CheckFree();
	int _GetSize();
	int _GetValue(WValue &value);
	int _SetValue(const WValue &value);
};
class DBConfigValueTmp: public DBConfigValue {
public:
	char tmp[MAX_NAME_LENGTH + 1];
};
#endif /* SRC_ICODE_CORE_DB_DBCONFIG_H_ */
