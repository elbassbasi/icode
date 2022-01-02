/*
 * DBConfig.h
 *
 *  Created on: 25 sept. 2021
 *      Author: azeddine
 */

#ifndef ICODE_SRC_DB_DBCONFIG_H_
#define ICODE_SRC_DB_DBCONFIG_H_
#include "Database.h"
class DBConfigManager;
/*
 *  key :
 *  0   : signature
 *  2   : attribute
 *  3   : name length (l)
 *  4	: red black link
 *  12  : name
 *  12 + l : value or children
 *
 *  INT   : 4
 *  BOOL  : 1
 *  LONG  : 8
 *  FLOAT : 4
 *  DOUBLE: 8
 *  STRING: 6 (2 for size + 4 for block)
 *  BIN   : 6 (2 for size + 4 for block)
 *  DIR   : 4 for block
 *
 */
class DBConfig: public IConfig, public AVLNode {
public:
	ObjectRef ref;
	enum {
		DEGREE = 4, //
		INDEX_SIGNATURE = 0, //
		INDEX_ATTR = 2, //
		INDEX_NAMELENGTH = 3, //
		INDEX_RED_BLACK = 4,
		INDEX_NAME = 12, //
		MAX_NAME_LENGTH = 0xFF, //
		COLOR = 1 << 7, //
		VALUE_LOCAL = 1 << 4, //
		CONFIG_SIGNATURE = 0xDB,
	};
	DBConfig *parent;
	DBConfig *firstChild;
	DBConfigManager *db;
	WArray<ConfigLintener> listenner;
	wuint block;
	union {
		float _float;
		wuint _int;
		wuchar _bool;
		wint64 _long;
		double _double;
		wuint _children;
		struct {
			wuint _value;
			wushort _size;
		};
	};
	wuchar attr;
	wuchar name_length;
	char name[1];
	static wuchar valuesLength[];
	static int CompareNode(const AVLNode *node1, const AVLNode *node2,
			void *userData);
	static int CompareKey(const AVLNode *node, const void *key, void *userData);
	static int RedBlackCompare(Database *db, wuint record, void *userdata);
	static int RedBlackForAll(Database *db, wuint record, void *userdata);
	const char* NextName(const char *source, char *key, wuint &length);
	static DBConfig* NewConfig(const char *name, int length);
	DBConfig();
	~DBConfig();
	void Init();
	int Release();
	int Free();
	ObjectRef* GetRef(int *tmp);
	AVLNode* Find(const char *name) {
		return this->FindNode(name, CompareKey, 0);
	}
	int GetAttribute();
	int GetSize();
	int Delete();
	bool AddListenner(ConfigLintener *listener);
	int GetListennerLength();
	bool GetListenner(int index, ConfigLintener *listener);
	bool RemoveListenner(ConfigLintener *listener);
	const char* GetName();
	int GetValue(int type, void *data, size_t size);
	int SetValue(int type, const void *data, size_t size);
	DBConfig* FindConfig(const char *name);
	DBConfig* LoadConfig(const char *name, int attr);
	int LoadValue();
	int Open(const char *name, int attr,Config &config);
	int ForAll(ConfigEnumeration notify, void *userdata);
public:
};
class DBConfigManager: public Database {
public:
	DBConfig root;
	DBConfigManager();
	~DBConfigManager();
	int OpenConfig(const char *name, int attr,Config &config);
	int Open(const char *location, int flags);
};
#endif /* ICODE_SRC_DB_DBCONFIG_H_ */
