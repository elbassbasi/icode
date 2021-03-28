/*
 * DBConfigManager.cpp
 *
 *  Created on: 20 nov. 2019
 *      Author: azeddine
 */

#include "DBConfigManager.h"
#include "DBConfig.h"

int DBConfigManager::Open(const char *configuration, IConfig **config) {
	Lock();
	DBConfig *c = 0;
	int ret = this->root._OpenChild(configuration, &c, 0);
	*config = c;
	UnLock();
	return ret;
}

int DBConfigManager::Create(const char *configuration, IConfig **config) {
	Lock();
	DBConfig *c = 0;
	int ret = this->root._OpenChild(configuration, &c, DB_FLAGS_CREATE);
	*config = c;
	UnLock();
	return ret;
}

DBConfigManager::DBConfigManager() {
	memset(&this->root, 0, sizeof(DBConfig) + sizeof(this->name));
	this->root.manager = this;
}

int DBConfigManager::GetRoot(IConfig **config) {
	Lock();
	*config = &this->root;
	UnLock();
	return 1;
}

const char* DBConfigManager::NextName(const char *source, const char **key,
		uint *size) {
	if (*source == '\\' || *source == '/')
		source++;
	const char *s;
	uint sz = 0;
	*key = s = source;
	while (s[sz] != 0 && s[sz] != '\\' && s[sz] != '/') {
		sz++;
	}
	*size = sz;
	if (s[sz] == 0)
		return 0;
	else
		return &s[sz];
}
int DBConfigManager::ReadH(DBTreeH *h, size_t h_sz, char *name) {
	int ret;
	unsigned char l, *header = (unsigned char*) h->GetHeader();
	ret = this->SeekRead(h->page, header, h_sz);
	if (ret < 0)
		return ret;
	l = header[h_sz - 1];
	ret = this->Read(name, l);
	if (ret < 0)
		return ret;
	return ret;
}
int DBConfigManager::FindH(const char *name, size_t sz, DBTreeH *h,
		size_t h_sz) {
	int ret;
	char tmp[MAX_NAME_LENGTH + 1];
	while (h->page != 0) {
		ret = ReadH(h, h_sz, tmp);
		if (ret < 0)
			return ret;
		if ((uint) ret == sz && !strncmp(name, tmp, sz)) {
			return 1;
		}
		h->prev = h->page;
		h->page = h->next;
	}
	return 0;
}
int DBConfigManager::CreateHC(const char *name, DBTreeHC *h) {
	int ret;
	ret = this->Alloc(&h->page, h->name_length + h->GetSize());
	if (ret >= 0) {
		ret = this->SeekWrite(h->page, h->GetHeader(), h->GetSize());
		if (ret >= 0) {
			ret = this->Write(name, h->name_length);
			if (ret >= 0) {
				if (h->prev == 0) {
					if (h->parent == 0) {
						ret = this->SetRoot(h->page);
					} else {
						ret = this->SeekWrite(h->parent, sizeof(DBPage),
								&h->page, sizeof(DBPage));
					}
				} else {
					ret = this->SeekWrite(h->prev, &h->page, sizeof(DBPage));
				}
			}
		}
		if (ret < 0) {
			this->Free(h->page, h->name_length + h->GetSize());
		}
	}
	return ret;
}
int DBConfigManager::CreateHV(const char *name, DBTreeHV *h) {
	int ret;
	ret = this->Alloc(&h->page, h->name_length + h->GetSize(), 0);
	if (ret >= 0) {
		ret = this->SeekWrite(h->page, h->GetHeader(), h->GetSize());
		if (ret >= 0) {
			ret = this->Write(name, h->name_length);
			if (ret >= 0) {
				if (h->prev == 0) {
					ret = this->SeekWrite(h->parent, 2 * sizeof(DBPage),
							&h->page, sizeof(DBPage));
				} else {
					ret = this->SeekWrite(h->prev, 0, &h->page, sizeof(DBPage));
				}
			}
		}
		if (ret < 0) {
			this->Free(h->page, h->name_length + h->GetSize());
		}
	}
	return ret;
}
int DBConfigManager::OpenChildDB(const char *name, size_t sz,
		const DBTreeHC *parent, DBTreeHC *child, int flags) {
	int ret;
	memset(child, 0, sizeof(DBTreeHC));
	child->page = parent->children;
	child->parent = parent->page;
	ret = FindH(name, sz, child, child->GetSize());
	if (ret < 0)
		return ret;
	if (ret == 0) {
		if (flags & DB_FLAGS_CREATE) {
			child->name_length = sz;
			ret = CreateHC(name, child);
		} else {
			return 0;
		}
	}
	return 1;
}
int DBConfigManager::OpenValueDB(const char *name, const DBTreeHC *parent,
		DBTreeHV *child, int flags) {
	int ret, sz = strlen(name);
	memset(child, 0, sizeof(DBTreeHV));
	child->page = parent->values;
	child->parent = parent->page;
	ret = FindH(name, sz, child, child->GetSize());
	if (ret < 0)
		return ret;
	if (ret == 0) {
		if (flags & DB_FLAGS_CREATE) {
			child->name_length = sz;
			ret = CreateHV(name, child);
		} else {
			return 0;
		}
	}
	return 1;
}
void DBConfigManager::Init() {
	DB::GetRoot(&this->root.header.children);
}
void DBConfigManager::InitChild(DBConfig *c, DBConfig *parent, const char *name,
		const DBTreeHC *h) {
	memset(c, 0, sizeof(DBConfig));
	memcpy(c->name, name, h->name_length);
	c->name[h->name_length] = 0;
	if (parent != 0) {
		c->parent = parent;
		c->manager = parent->manager;
		c->next = parent->firstChild;
		parent->firstChild = c;
	}
	memcpy(&c->header, h, sizeof(DBTreeHC));
}
void DBConfigManager::InitValue(DBConfigValue *value, DBConfig *parent,
		const char *name, const DBTreeHV *h) {
	memset(value, 0, sizeof(DBConfigValue));
	memcpy(value->name, name, h->name_length);
	value->name[h->name_length] = 0;
	if (parent != 0) {
		value->parent = parent;
		value->next = parent->values;
		parent->values = value;
	}
	memcpy(&value->header, h, sizeof(DBTreeHV));
}
int DBConfigManager::FreeValueV(DBTreeHV *h) {
	int ret = 1;
	switch (h->type) {
	case W_VALUE_INT64:
	case W_VALUE_DOUBLE:
		if (h->value != 0) {
			ret = this->Free(h->value, sizeof(double));
		}
		break;
	case W_VALUE_POINTER:
	case W_VALUE_STRING_REF:
	//case W_VALUE_UTF8:
		if (h->value != 0) {
			ret = this->FreeValueP(h->value);
		}
		break;
	}
	h->value = 0;
	return ret;
}
int DBConfigManager::WriteValue(DBTreeHV *h, const WValue &v) {
	int ret = 1;
	uint sz;
	DBPage lastp;
	switch (v.clazz->type) {
	case W_VALUE_UNKNOWN:
		this->FreeValueV(h);
		h->type = v.clazz->type;
		h->value = 0;
		ret = this->SeekWrite(h->page, h->GetHeader(), h->GetSize());
		break;
	case W_VALUE_BOOL:
	case W_VALUE_CHAR:
	//case W_VLAUE_SHORT:
	case W_VALUE_INT:
	case W_VALUE_FLOAT:
		this->FreeValueV(h);
		h->value = v.INT64;
		h->type = v.clazz->type;
		ret = this->SeekWrite(h->page, h->GetHeader(), h->GetSize());
		break;
	//case W_VALUE_INT64:
	case W_VALUE_DOUBLE: {
		lastp = h->value;
		if (h->type != W_VALUE_INT64 && h->type != W_VALUE_DOUBLE) {
			this->FreeValueV(h);
		}
		if (h->value == 0) {
			ret = this->Alloc(&h->value, sizeof(double));
			if (ret > 0) {
				this->SeekWrite(h->value, &v.DOUBLE, sizeof(double));
				if (ret > 0) {
					if (h->type != v.clazz->type || lastp != h->value) {
						h->type = v.clazz->type;
						ret = this->SeekWrite(h->page, h->GetHeader(),
								h->GetSize());
					}
				} else {
					this->Free(h->value, sizeof(double));
				}
			}
		} else {
			ret = this->SeekWrite(h->value, &v.DOUBLE, sizeof(double));
			if (h->type != v.clazz->type) {
				ret = this->SeekWrite(h->page, h->GetHeader(), h->GetSize());
			}
		}
	}
		break;
	//case W_VALUE_POINTER:
	case W_VALUE_STRING_REF: {
		lastp = h->value;
		if (v.clazz->type == W_VALUE_STRING_REF) {
			w_string_ref* ref =(w_string_ref*) v.pointer;
			sz = w_string_get_length(ref);
		} else {
			if (v.size == 0xffffff)
				sz = strlen(v.string);
			else
				sz = v.size;
		}
		if (sz == 0) {
			if (h->value != 0) {
				this->FreeValueP(h->value);
				h->value = 0;
			}
		} else {
			ret = this->ReAllocP(h, sz + sizeof(sz));
			if (ret > 0 && h->value != 0) {
				ret = this->SeekWrite(h->value, &sz, sizeof(sz));
				if (ret > 0) {
					if (v.clazz->type == W_VALUE_STRING_REF) {
						w_string_ref* ref =(w_string_ref*) v.pointer;
						ret = this->Write(ref->data, sz);
					} else
						ret = this->Write(v.pointer, sz);
				}

			}
		}
		if (h->type != v.clazz->type || lastp != h->value) {
			h->type = v.clazz->type;
			ret = this->SeekWrite(h->page, h->GetHeader(), h->GetSize());
		}
	}
		break;
	default:
		ret = -1;
		break;
	}
	if (ret > 0) {
		h->type = v.clazz->type;
		ret = this->SeekWrite(h->page, h->GetHeader(), h->GetSize());
	}
	return ret;
}

int DBConfigManager::ReadValue(DBTreeHV *h, WValue &v) {
}

int DBConfigManager::FreeValueP(DBPage p) {
	uint sz;
	int ret = this->SeekRead(p, &sz, sizeof(sz));
	if (ret > 0) {
		ret = this->Free(p, sz);
	}
	return ret;
}

int DBConfigManager::ReAllocP(DBTreeHV *h, uint newsize) {
	int ret;
	uint sz;
	if (h->value == 0) {
		ret = this->Alloc(&h->value, newsize);
	} else {
		int ret = this->SeekRead(h->value, &sz, sizeof(sz));
		if (ret > 0) {
			ret = this->Realloc(&h->value, newsize, sz);
		}
	}
	return ret;
}

void DBConfigManager::Test() {
	/*	Ptr<IConfig> config = IConfigManager::Create("/test1/test2/test3/test4");
	 if (config.IsOk()) {
	 config->SetValue("value1", "Hello Azeddin2");
	 }*/
}

