/*
 * DBConfig.cpp
 *
 *  Created on: 25 sept. 2021
 *      Author: azeddine
 */

#include "DBConfig.h"
#include <new>
#include <memory>
wuchar DBConfig::valuesLength[] = { 4, 1, 8, 4, 8, 6, 6, 4 };
DBConfig::DBConfig() {
	Init();
}
DBConfig::~DBConfig() {
}
ObjectRef* DBConfig::GetRef(int *tmp) {
	return &this->ref;
}
int DBConfig::Free() {
	DBConfig *c = this, *p;
	while (1) {
		if (c->ref.ref == 0 && c->firstChild == 0) {
			p = c->parent;
			if (p == 0)
				break;
			AVLNode *node = p->RemoveNode(c);
			if (node == 0)
				p->firstChild = 0;
			else
				p->firstChild = static_cast<DBConfig*>(node);
			c->~DBConfig();
			free(c);
			c = p;
		} else
			break;
	}
	return 0;
}
int DBConfig::Release() {
	int tmp[2];
	ObjectRef *ref = GetRef(tmp);
	if (ref->ref > 1) {
		return __atomic_fetch_sub(&ref->ref, 1, __ATOMIC_SEQ_CST);
	} else if (ref->ref < 0) {
		return ref->ref;
	} else {
		return Free();
	}
}
int DBConfig::CompareNode(const AVLNode *node1, const AVLNode *node2,
		void *userData) {
	return CompareString(static_cast<const DBConfig*>(node1)->name,
			static_cast<const DBConfig*>(node2)->name);
}

int DBConfig::CompareKey(const AVLNode *node, const void *key, void *userData) {
	return CompareString(static_cast<const DBConfig*>(node)->name,
			(const char*) key);
}
const char* DBConfig::NextName(const char *source, char *key, wuint &length) {
	if (*source == '\\' || *source == '/')
		source++;
	const char *s;
	uint sz = 0;
	s = source;
	while (s[sz] != 0 && s[sz] != '\\' && s[sz] != '/') {
		sz++;
	}
	length = sz;
	if (sz < 0xff) {
		memcpy(key, s, sz);
		key[sz] = 0;
	}
	if (s[sz] == 0)
		return 0;
	else
		return &s[sz];
}
DBConfig* DBConfig::NewConfig(const char *name, int length) {
	void *ptr = calloc(1, sizeof(DBConfig) + length + 1);
	if (ptr == 0)
		return 0;
	DBConfig *ret = new (ptr) DBConfig();
	memcpy(ret->name, name, length);
	ret->name[length] = 0;
	ret->name_length = length;
	return ret;
}
void DBConfig::Init() {
}

int DBConfig::GetAttribute() {
	return this->attr & ((1 << 5) - 1);
}

int DBConfig::GetSize() {
	switch ((this->attr & Config::TYPE_MASK)) {
	case Config::INT:
		return sizeof(int);
		break;
	case Config::BOOL:
		return sizeof(bool);
		break;
	case Config::FLOAT:
		return sizeof(float);
		break;
	case Config::DOUBLE:
		return sizeof(double);
		break;
	case Config::STRING:
	case Config::BIN:
		return this->_size;
		break;
	default:
		return 0;
		break;
	}
}
int DBConfig::Delete() {
	if (this->ref.ref == 0 || this->ref.ref == 1) {
		if (this->firstChild == 0) {
			int ret = 1;

			return ret;
		}
	}
	return -1;
}

bool DBConfig::AddListenner(ConfigLintener *listener) {
	return this->listenner.Add(*listener) != -1;
}

int DBConfig::GetListennerLength() {
	return this->listenner.GetCount();
}

bool DBConfig::GetListenner(int index, ConfigLintener *listener) {
	ConfigLintener *_listener = this->listenner.Get0(index);
	if (_listener != 0) {
		memcpy(listener, _listener, sizeof(ConfigLintener));
		return true;
	} else
		return false;
}

bool DBConfig::RemoveListenner(ConfigLintener *listener) {
}

const char* DBConfig::GetName() {
	return this->name;
}
int DBConfig::GetValue(int type, void *data, size_t size) {
	if (type != (attr & Config::TYPE_MASK))
		return -1;
	switch ((attr & Config::TYPE_MASK)) {
	case Config::INT:
		if (size < 4)
			return -1;
		Database::PutInt((wuchar*) data, this->_int);
		return 4;
		break;
	case Config::BOOL:
		if (size < 1)
			return -1;
		((wuchar*) data)[0] = this->_bool;
		return 1;
		break;
	case Config::FLOAT:
		if (size < 4)
			return -1;
		((float*) data)[0] = this->_float;
		return 4;
		break;
	case Config::DOUBLE:
		if (size < sizeof(double))
			return -1;
		((double*) data)[0] = this->_double;
		return 8;
		break;
	case Config::LONG:
		if (size < sizeof(wint64))
			return -1;
		((wint64*) data)[0] = this->_long;
		return 8;
		break;
	case Config::BIN:
	case Config::STRING: {
		wuint b;
		int off;
		if (this->attr & VALUE_LOCAL) {
			b = this->block;
			off = INDEX_NAME + this->name_length + 1;
		} else {
			b = this->_value;
			off = 0;
		}
		if (b == 0)
			return 0;
		DatabaseChunk chunk;
		wushort _lr = WMIN(this->_size, size);
		int ret = db->LoadChunk(b, off, _lr, chunk);
		if (ret > 0) {
			ret = db->GetChunkBytes(chunk, b, off, data, _lr);
			db->ReleaseChunk(chunk);
		}
		return ret;
	}
		break;
	case Config::DIR:
		return -1;
		break;
	}
	return -1;
}

int DBConfig::SetValue(int type, const void *data, size_t size) {
	if (type != (attr & Config::TYPE_MASK))
		return -1;
	int ret = -1;
	int value_offset = INDEX_NAME + this->name_length;
	int allocSize = value_offset + valuesLength[(attr & Config::TYPE_MASK)];
	int allocatedSize = db->GetAllocatedSize(allocSize);
	DatabaseChunk chunk;
	ret = db->LoadChunk(this->block, 0, allocatedSize, chunk);
	if (ret < 0)
		return ret;
	switch ((this->attr & Config::TYPE_MASK)) {
	case Config::INT:
		if (size < 4)
			return -1;
		this->_int = Database::GetInt((wuchar*) data);
		ret = db->PutChunkInt(chunk, this->block, value_offset, this->_int);
		break;
	case Config::BOOL: {
		if (size < 1)
			return -1;
		wuchar c = *((wuchar*) data);
		this->_int = c;
		ret = db->PutChunkBytes(chunk, this->block, value_offset, &c,
				sizeof(c));
	}
		break;
	case Config::FLOAT:
		if (size < 4)
			return -1;
		this->_float = *((float*) data);
		ret = db->PutChunkBytes(chunk, this->block, value_offset, &this->_float,
				sizeof(&this->_float));
		break;
	case Config::DOUBLE:
		if (size < sizeof(double))
			return -1;
		ret = db->PutChunkBytes(chunk, this->block, value_offset, data,
				sizeof(double));
		break;
	case Config::LONG:
		if (size < sizeof(wint64))
			return -1;
		ret = db->PutChunkBytes(chunk, this->block, value_offset, data,
				sizeof(wint64));
		break;
	case Config::BIN:
	case Config::STRING: {
		if (type == Config::STRING && size == -1) {
			if (data == 0)
				size = 0;
			else {
				size = strlen((const char*) data);
			}
		}
		if (size <= (allocatedSize - value_offset - 1)) {
			wuchar l;
			if ((this->attr & VALUE_LOCAL) == 0) {
				if (this->_value != 0) {
					db->Free(this->_value, this->_size);
					this->_value = 0;
				}
				this->attr |= VALUE_LOCAL;
				l = this->attr;
				db->PutChunkBytes(chunk, this->block, INDEX_ATTR, &l,
						sizeof(l));
			}
			l = size;
			db->PutChunkBytes(chunk, this->block, value_offset, &l, sizeof(l));
			db->PutChunkBytes(chunk, this->block, value_offset + 1, data, size);
		} else {
			if ((this->attr & VALUE_LOCAL) != 0) {
				ret = db->Malloc(this->_value, size);
				if (ret > 0) {
					wuchar l;
					this->attr &= ~VALUE_LOCAL;
					l = this->attr;
					ret = db->PutChunkBytes(chunk, this->block, INDEX_ATTR, &l,
							sizeof(l));
				}
			} else {
				ret = db->Realloc(this->_value, size, this->_size);
			}
			if (ret > 0) {
				wushort l = size;
				ret = db->PutChunkShort(chunk, this->block, value_offset, l);
				ret = db->PutChunkInt(chunk, this->block,
						value_offset + sizeof(l), this->_value);
				if (this->_value != 0) {
					DatabaseChunk vChunk;
					ret = db->LoadChunk(this->_value, 0, size, vChunk);
					if (ret > 0) {
						ret = db->PutChunkBytes(vChunk, this->_value, 0, data,
								size);
						db->ReleaseChunk(vChunk);
					}
				}
			}
		}
	}
		break;
	}
	db->ReleaseChunk(chunk);
	if (ret >= 0) {
		db->Flush();
		DBConfig *c = this;
		ConfigEvent e;
		e.type = ConfigEvent::CONFIG_UPDATED;
		e.configUpdated.config = this;
		while (c != 0) {
			ConfigLintener *l;
			int count = c->listenner.GetCount(l);
			for (int i = 0; i < count; i++) {
				e.userdata = l[i].userdata;
				e.config.config = c;
				l[i].listener(e);
			}
			c = c->parent;
		}
		e.config.config = 0;
		e.configUpdated.config = 0;
	}
	return ret;
}
int DBConfig::LoadValue() {
	int value_offset = INDEX_NAME + this->name_length;
	DatabaseChunk chunk;
	int ret = db->LoadChunk(this->block, 0, value_offset + 6, chunk);
	if (ret < 0)
		return ret;
	wuchar attr;
	db->GetChunkByte(chunk, this->block, INDEX_ATTR, &attr);
	this->attr = attr;
	switch ((attr & Config::TYPE_MASK)) {
	case Config::INT:
		db->GetChunkInt(chunk, this->block, value_offset, &this->_int);
		break;
	case Config::BOOL:
		db->GetChunkBytes(chunk, this->block, value_offset, &this->_bool,
				sizeof(this->_bool));
		break;
	case Config::FLOAT:
		db->GetChunkBytes(chunk, this->block, value_offset, &this->_float,
				sizeof(this->_float));
		break;
	case Config::DOUBLE:
		db->GetChunkBytes(chunk, this->block, value_offset, &this->_double,
				sizeof(this->_double));
		break;
	case Config::LONG:
		db->GetChunkBytes(chunk, this->block, value_offset, &this->_long,
				sizeof(this->_long));
		break;
	case Config::BIN:
	case Config::STRING:
		if (attr & VALUE_LOCAL) {
			wuchar l = 0;
			db->GetChunkByte(chunk, this->block, value_offset, &l);
			this->_size = l;
			this->_value = 0;
		} else {
			db->GetChunkShort(chunk, this->block, value_offset, &this->_size);
			db->GetChunkInt(chunk, this->block,
					value_offset + sizeof(this->_size), &this->_value);
		}
		break;
	case Config::DIR:
		db->GetChunkInt(chunk, this->block, value_offset, &this->_children);
		break;
	}
	db->ReleaseChunk(chunk);
	return ret;
}
DBConfig* DBConfig::FindConfig(const char *name) {
	AVLNode *node = 0;
	if (this->firstChild != 0)
		node = this->firstChild->FindNode(name, DBConfig::CompareKey, 0);
	if (node == 0)
		return 0;
	else
		return static_cast<DBConfig*>(node);
}
struct BTreeVisitKey {
	wuint block;
	const char *key;
};
int DBConfig::RedBlackCompare(Database *db, wuint record, void *userdata) {
	DatabaseChunk chunk;
	int ret = db->LoadChunk(record, 0, INDEX_NAMELENGTH + 1, chunk);
	if (ret > 0) {
		wuchar kLength;
		char k[0x100];
		db->GetChunkBytes(chunk, record, INDEX_NAMELENGTH, &kLength,
				sizeof(kLength));
		db->ReleaseChunk(chunk);
		ret = db->LoadChunk(record, INDEX_NAME, kLength, chunk);
		if (ret > 0) {
			db->GetChunkBytes(chunk, record, INDEX_NAME, k, kLength);
			db->ReleaseChunk(chunk);
			k[kLength] = 0;
			return AVLNode::CompareString(k, (const char*) userdata);
		}
	}
	return -2;
}
DBConfig* DBConfig::LoadConfig(const char *name, int attr) {
	if ((this->attr & Config::TYPE_MASK) != Config::DIR)
		return 0;
	/*BTreeVisitKey _visit;
	 _visit.block = 0;
	 _visit.key = name;*/
	/*int ret = db->BTreeVisit(this->block, INDEX_NAME + this->name_length,
	 KeyCompare, KeyVisit, &_visit, DEGREE);
	 if (ret < 0)
	 return 0;
	 if (_visit.block == 0 && (attr & Config::CREATE) != 0) {
	 CreateConfig(_visit.block, name, attr);
	 if (_visit.block != 0) {
	 ret = db->BTreeInsert(this->block, INDEX_NAME + this->name_length,
	 _visit.block, KeyCompare2, &_visit, DEGREE);
	 db->Flush();
	 }
	 }*/
	int flags;
	wuint record = 0;
	wuchar l = strlen(name);
	int allocSize = INDEX_NAME + l + valuesLength[(attr & Config::TYPE_MASK)];
	if ((attr & Config::CREATE) != 0) {
		flags = Database::RBTREE_ALLOC_INSERT;
		record = allocSize;
	} else {
		flags = Database::RBTREE_FIND;
	}
	int ret = db->RBTreeInsert(this->block, INDEX_NAME + this->name_length,
			record, INDEX_RED_BLACK, RedBlackCompare, (void*) name, flags);
	if (ret < 0)
		return 0;
	if ((attr & Config::CREATE) != 0 && ret == 0) {
		DatabaseChunk chunk;
		ret = db->LoadChunk(record, 0, allocSize, chunk);
		if (ret > 0) {
			db->PutChunkByte(chunk, record, INDEX_ATTR,
					(attr & Config::TYPE_MASK));
			db->PutChunkByte(chunk, record, INDEX_NAMELENGTH, l);
			db->PutChunkShort(chunk, record, INDEX_SIGNATURE, CONFIG_SIGNATURE);
			db->PutChunkBytes(chunk, record, INDEX_NAME, name, l);
			db->ReleaseChunk(chunk);
		}
		db->Flush();
	}
	DBConfig *c = 0;
	if (record != 0) {
		c = DBConfig::NewConfig(name, strlen(name));
		if (c != 0) {
			c->block = record;
			c->db = this->db;
			c->parent = this;
			this->InsertNode(c, DBConfig::CompareNode, 0);
			c->LoadValue();
		}
	}
	return c;
}
int DBConfig::Open(const char *name, int attr, Config &config) {
	int ret;
	DBConfig *c = this, *c1;
	char key[0x100];
	const char *s = name;
	wuint length = 0;
	int _attr = (attr & ~Config::TYPE_MASK) | Config::DIR;
	while (true) {
		s = NextName(s, key, length);
		if (length > sizeof(key)) {
			c == 0;
			ret = -1;
			break;
		}
		if (s == 0)
			_attr = attr;
		c1 = c->FindConfig(key);
		if (c1 == 0) {
			c1 = c->LoadConfig(key, _attr);
		}
		if (c1 == 0) {
			c = 0;
			ret = -1;
			break;
		}
		c = c1;
		if (s == 0) {
			ret = 1;
			break;
		}
	}
	IConfig *last = config.config;
	config.config = c;
	if (c != 0) {
		c->AddRef();
	}
	if (last != 0) {
		last->Release();
	}
	return ret;
}
struct ForAllKey {
	Config parent;
	ConfigEnumeration notify;
	void *userdata;
};

int DBConfig::RedBlackForAll(Database *db, wuint record, void *userdata) {
	ForAllKey *forAll = (ForAllKey*) userdata;
	DatabaseChunk chunk;
	int ret = db->LoadChunk(record, 0, INDEX_NAMELENGTH + 1, chunk);
	if (ret > 0) {
		wuchar kLength;
		char k[0x100];
		db->GetChunkBytes(chunk, record, INDEX_NAMELENGTH, &kLength,
				sizeof(kLength));
		db->ReleaseChunk(chunk);
		ret = db->LoadChunk(record, INDEX_NAME, kLength, chunk);
		if (ret > 0) {
			db->GetChunkBytes(chunk, record, INDEX_NAME, k, kLength);
			db->ReleaseChunk(chunk);
			k[kLength] = 0;
			forAll->notify(forAll->parent, forAll->userdata, k);
		}
	}
	return -2;
}

int DBConfig::ForAll(ConfigEnumeration notify, void *userdata) {
	if ((this->attr & Config::TYPE_MASK) != Config::DIR)
		return 0;
	ForAllKey forAll;
	forAll.parent.config = this;
	this->AddRef();
	forAll.notify = notify;
	forAll.userdata = userdata;
	int ret;
	/*ret = db->BTreeVisit(this->block, INDEX_NAME + this->name_length,
	 ForAllCompare, ForAllVisit, &forAll, DEGREE);
	 */
	ret = db->RBTreeForAll(this->block, INDEX_NAME + this->name_length,
			INDEX_RED_BLACK, RedBlackForAll, &forAll);
	return ret;

}
/* */
DBConfigManager::DBConfigManager() {
	memset(&root, 0, sizeof(root));
	new (&root) DBConfig();
	root.db = this;
}

DBConfigManager::~DBConfigManager() {
}

int DBConfigManager::OpenConfig(const char *name, int attr, Config &config) {
	if (name == 0) {
		config.config = &this->root;
		return 1;
	} else {
		return this->root.Open(name, attr, config);
	}
}

int DBConfigManager::Open(const char *location, int flags) {
	int ret = Database::Open(location, flags, 10, CHUNK_CACHE_COUNT);
	if (ret >= 0) {
		wuint root;
		Database::GetRoot(root);
		this->root.block = root;
		this->root.attr = Config::DIR;
	}
	return ret;
}
void database_test_enumeration(Config &parent, void *userdata,
		const char *child) {
	Config c;
	parent.Open(child, c);
	printf("%d\n", c.GetInt(-1));
}
void database_test() {
	const char *filelocation = "/home/azeddine/Bureau/IcodeWorkTest/test.db";
	//remove(filelocation);
	DBConfigManager db;
	db.Open(filelocation, 0);
	Config root, c;
	DBConfig *_c;
	root.config = &db.root;
	for (int i = 0; i < 100; i++) {
		char txt[0x100];
		snprintf(txt, sizeof(txt), "%03d", i);
		if (i == 2) {
			c.GetInt(0);
		}
		root.CreateInt(txt, c);
		if (1) {
			puts("\n");
			db.RBTreePrint(db.root.block, DBConfig::INDEX_NAME,
					DBConfig::INDEX_RED_BLACK);
		}
		c.SetInt(i);
	}
	root.ForAll(database_test_enumeration, 0);
}
