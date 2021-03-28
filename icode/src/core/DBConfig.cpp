/*
 * DBConfig.cpp
 *
 *  Created on: 12 nov. 2019
 *      Author: azeddine
 */

#include "DBConfig.h"
#include "DBConfigManager.h"
#include <new>
IConfigValue::~IConfigValue() {
}

IConfig::~IConfig() {
}
DBConfig::DBConfig() {
}
void DBConfig::Free() {
}
DBConfig::~DBConfig() {
	Free();
}

int DBConfig::Delete() {
}

const char* DBConfig::GetName() {
	return this->name;
}

int DBConfig::OpenValue(const char *name, IConfigValue **configvalue) {
	this->manager->Lock();
	DBConfigValue *v = 0;
	int ret = this->_OpenValue(name, &v, 0);
	*configvalue = v;
	this->manager->UnLock();
	return ret;
}

int DBConfig::CreateValue(const char *name, const WValue &value,
		IConfigValue **configvalue) {
	this->manager->Lock();
	DBConfigValue *v = 0;
	int ret = this->_OpenValue(name, &v, DB_FLAGS_CREATE);
	*configvalue = v;
	this->manager->UnLock();
	return ret;
}

int DBConfig::GetValue(const char *name, WValue &value) {
	this->manager->Lock();
	DBConfigValue *v;
	DBConfigValueTmp tmp;
	v = &tmp;
	int ret = this->_OpenValue(name, &v, 0);
	if (ret > 0) {
		ret = v->_GetValue(value);
	}
	this->manager->UnLock();
	return ret;
}
int DBConfig::SetValue(const char *name, const WValue &value) {
	this->manager->Lock();
	DBConfigValue *v;
	DBConfigValueTmp tmp;
	v = &tmp;
	int ret = this->_OpenValue(name, &v, DB_FLAGS_CREATE);
	if (ret > 0) {
		v->parent = this;
		ret = v->_SetValue(value);
	}
	this->manager->UnLock();
	return ret;
}
struct DBConfig_GetValues {
	struct _w_iterator_class *clazz;
	int count;

};
wresult DBConfig_GetValues_next(w_iterator *it, void *obj) {
	DBConfig_GetValues *vi = (DBConfig_GetValues*) it;

}
wresult DBConfig_GetValues_reset(w_iterator *it) {
	DBConfig_GetValues *vi = (DBConfig_GetValues*) it;

}
wresult DBConfig_GetValues_close(w_iterator *it) {
	DBConfig_GetValues *vi = (DBConfig_GetValues*) it;

}
wresult DBConfig_GetValues_remove(w_iterator *it) {
	DBConfig_GetValues *vi = (DBConfig_GetValues*) it;

}
size_t DBConfig_GetValues_get_count(w_iterator *it) {
	DBConfig_GetValues *vi = (DBConfig_GetValues*) it;
	if (vi->count >= 0)
		return vi->count;
	else {

	}
}
class DBIteratorValues: public WBasicIterator<IConfigValue*> {
public:
	int count;
	DBIteratorValues(){

	}
	bool Next(IConfigValue *&t) {

	}
	WResult Reset() {

	}
	WResult Remove() {
		return false;
	}
	size_t GetCount() {
		return count;
	}
};
void DBConfig::GetValues(WIterator<IConfigValue*> &values) {
	/*values.Close();
	 ((DBConfig_GetValues*) &values)->clazz = &DBConfig_GetValues_class;*/
}

int DBConfig::OpenChild(const char *key, IConfig **config) {
	this->manager->Lock();
	DBConfig *c = 0;
	int ret = this->_OpenChild(key, &c, 0);
	*config = c;
	this->manager->UnLock();
	return ret;
}

int DBConfig::CreateChild(const char *name, IConfig **config) {
	this->manager->Lock();
	DBConfig *c = 0;
	int ret = this->_OpenChild(name, &c, DB_FLAGS_CREATE);
	*config = c;
	this->manager->UnLock();
	return ret;
}

bool DBConfig::AddListenner(IConfigListener *listener) {
	this->manager->Lock();
	IConfigListener *last = this->firstListener;
	this->firstListener = listener;
	this->manager->UnLock();
	return last;
}

bool DBConfig::RemoveListenner(IConfigListener *listener) {
}
struct DBConfig_GetChildren {
	struct _w_iterator_class *clazz;

};
wresult DBConfig_GetChildren_next(w_iterator *it, void *obj) {

}
wresult DBConfig_GetChildren_reset(w_iterator *it) {

}
wresult DBConfig_GetChildren_close(w_iterator *it) {

}
wresult DBConfig_GetChildren_remove(w_iterator *it) {

}
size_t DBConfig_GetChildren_get_count(w_iterator *it) {
	return 0;
}
/*struct _w_iterator_class DBConfig_GetChildren_class = {
 sizeof(DBConfig_GetChildren), sizeof(_w_iterator_class),
 DBConfig_GetChildren_next, DBConfig_GetChildren_reset,
 DBConfig_GetChildren_close, DBConfig_GetChildren_remove,
 DBConfig_GetChildren_get_count };*/
void DBConfig::GetChildren(WIterator<IConfig*> &children) {
	/*children.Close();
	 ((DBConfig_GetValues*) &children)->clazz = &DBConfig_GetValues_class;*/
}

IObject* DBConfig::QueryInterface(const IID interfaceId) {
	return 0;
}

int DBConfig::IncRef() {
	IObject::IncRef(this->ref);
}

int DBConfig::DecRef() {
	int ref = IObject::DecRef(this->ref);
	if (ref <= 0) {
		CheckFree();
	}
}

int DBConfigValue::Delete() {
}

const char* DBConfigValue::GetName() {
	return this->name;
}

int DBConfigValue::GetType() {
	return this->header.type;
}

int DBConfigValue::GetSize() {
	this->parent->manager->Lock();
	int sz = this->_GetSize();
	this->parent->manager->UnLock();
	return sz;
}
int DBConfigValue::GetValue(WValue &value) {
	this->parent->manager->Lock();
	int ret = this->_GetValue(value);
	this->parent->manager->UnLock();
	return ret;
}

void DBConfigValue::Free() {
}

DBConfigValue::DBConfigValue() {
}

int DBConfigValue::SetValue(const WValue &value) {
	this->parent->manager->Lock();
	int ret = this->_SetValue(value);
	this->parent->manager->UnLock();
	return ret;
}
bool DBConfigValue::AddListenner(IConfigListener *listener) {
	IConfigListener *last = this->firstListener;
	this->firstListener = listener;
	return last;
}
bool DBConfigValue::RemoveListenner(IConfigListener *listener) {
	return this->firstListener;
}

IObject* DBConfigValue::QueryInterface(const IID interfaceId) {
	return 0;
}

int DBConfigValue::IncRef() {
	IObject::IncRef(this->ref);
}

int DBConfigValue::DecRef() {
	IObject::DecRef(this->ref);
}

DBConfigValue::~DBConfigValue() {
	Free();
}

void DBConfig::CheckFree() {
	if (this->ref > 0)
		return;
	if (this->firstChild != 0)
		return;
	if (this->firstListener != 0)
		return;
	if (this->listeners != 0)
		return;
	if (this->values != 0)
		return;
	if (parent == 0)
		return;
	this->manager->Lock();
	DBConfig *c = parent->firstChild, *prev;
	while (c != this) {
		prev = c;
		c = c->next;
	}
	if (prev != 0) {
		prev->next = this->next;
	} else {
		parent->firstChild = this->next;
	}
	this->manager->UnLock();
	parent->CheckFree();
}

void DBConfigValue::CheckFree() {
	if (this->ref > 0)
		return;
	if (this->firstListener != 0)
		return;
	if (this->listeners != 0)
		return;
	this->parent->manager->Lock();
	DBConfigValue *v = this->parent->values, *prev = 0;
	while (v != this) {
		prev = v;
		v = v->next;
	}
	if (prev == 0) {
		this->parent->values = this->next;
	} else {
		prev->next = this->next;
	}
	this->parent->manager->UnLock();
	this->parent->CheckFree();
}

int DBConfigValue::_GetValue(WValue &value) {
}

int DBConfigValue::_GetSize() {
	uint sz = 0;
	int ret;
	switch (this->header.type) {
	case W_VALUE_BOOL:
	//case W_VALUE_CHAR:
		sz = sizeof(char);
		break;
	case W_VALUE_INT:
	case W_VALUE_FLOAT:
		sz = sizeof(int);
		break;
	//case W_VALUE_INT64:
	case W_VALUE_DOUBLE:
		sz = sizeof(double);
		break;
	case W_VALUE_POINTER:
	case W_VALUE_STRING_REF: {
		if (this->header.value != 0) {
			if (this->size == 0) {
				ret = this->parent->manager->SeekRead(this->header.value, &sz,
						sizeof(sz));
				if (ret > 0) {
					this->size = sz;
				}
			} else
				sz = this->size;
		}
	}
		break;
	}
	return sz;
}

DBConfigValue* DBConfigValue::Create(DBConfig *parent, DBConfigValue *prev,
		int name_length) {
	DBConfigValue *config = (DBConfigValue*) malloc(
			sizeof(DBConfigValue) + name_length + 1);
	if (config != 0) {
		memset(config, 0, sizeof(DBConfigValue));
		new (config) DBConfigValue();
		if (prev == 0) {
			parent->values = config;
		} else {
			prev->next = config;
		}
	}
	return config;
}

int DBConfigValue::_SetValue(const WValue &value) {
	int ret = this->parent->manager->WriteValue(&this->header, value);
	return ret;
}

int DBConfig::_OpenChild(const char *key, DBConfig **config, int flags) {
	const char *k, *next = key;
	DBConfig *s, *p = this;
	DBConfigManager *manager = this->manager;
	uint sz;
	int ret;
	DBTreeHC h;
	while (true) {
		next = DBConfigManager::NextName(next, &k, &sz);
		if (sz > MAX_NAME_LENGTH)
			ret = -1;
		else {
			ret = 1;
			s = p->firstChild;
			while (s != 0) {
				if (s->header.name_length == sz && !strncmp(s->name, k, sz)) {
					break;
				}
				s = s->next;
			}
			if (s == 0) {
				ret = manager->OpenChildDB(k, sz, &p->header, &h, flags);
				if (ret >= 0) {
					s = (DBConfig*) malloc(sizeof(DBConfig) + sz + 1);
					if (s != 0)
						DBConfigManager::InitChild(s, p, k, &h);
				}
			}
			if (s != 0) {
				p = s;
			} else {
				break;
			}
			if (next == 0)
				break;
		}
	}
	if (ret <= 0) {
		p->CheckFree();
		*config = 0;
	} else {
		if (flags & DB_FLAGS_CREATE) {
			this->manager->Flush();
		}
		*config = s;
	}
	return ret;
}

int DBConfig::_OpenValue(const char *key, DBConfigValue **value, int flags) {
	int _l = strlen(key), ret;
	if (_l > MAX_NAME_LENGTH)
		return -1;
	DBConfigValue *v = this->values;
	DBTreeHV h;
	while (v != 0) {
		if (v->header.name_length == _l && !strncmp(key, v->header.name, _l)) {
			*value = v;
			return 1;
		}
		v = v->next;
	}

	ret = this->manager->OpenValueDB(key, &this->header, &h, flags);
	if (ret > 0) {
		if (*value == 0) {
			*value = (DBConfigValue*) malloc(_l + sizeof(DBConfigValue));
			if (*value != 0) {
				DBConfigManager::InitValue(*value, this, key, &h);
			} else {
				return -1;
			}
		} else {
			DBConfigManager::InitValue(*value, 0, key, &h);
		}
	}
	return ret;
}

DBConfig* DBConfig::Create(DBConfig *parent, DBConfig *prev, int name_length) {
	DBConfig *config = (DBConfig*) malloc(sizeof(DBConfig) + name_length + 1);
	if (config != 0) {
		memset(config, 0, sizeof(DBConfig));
		new (config) DBConfig();
		if (prev == 0) {
			parent->firstChild = config;
		} else {
			prev->next = config;
		}
		config->manager = parent->manager;
	}
	return config;
}
