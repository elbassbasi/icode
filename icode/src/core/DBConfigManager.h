/*
 * DBConfigManager.h
 *
 *  Created on: 20 nov. 2019
 *      Author: azeddine
 */

#ifndef SRC_ICODE_CORE_DB_DBCONFIGMANAGER_H_
#define SRC_ICODE_CORE_DB_DBCONFIGMANAGER_H_
#include "DBConfig.h"

class DBConfigManager: public IConfigManager, public DB {
public:
	DBConfig root;
	char name[2];
	DBConfigManager();
	void Init();
	int Open(const char *configuration, IConfig **config);
	int Create(const char *configuration, IConfig **config);
	int GetRoot(IConfig **config);
public:
	static const char* NextName(const char *source, const char **key,
			uint *size);
	int ReadH(DBTreeH *h, size_t h_sz, char *name);
	int FindH(const char *name, size_t sz, DBTreeH *h, size_t h_sz);
	int CreateHC(const char *name, DBTreeHC *h);
	int CreateHV(const char *name, DBTreeHV *v);
	static void InitChild(DBConfig *child, DBConfig *parent,
			const char *name, const DBTreeHC *h);
	static void InitValue(DBConfigValue *value, DBConfig *parent,
			const char *name, const DBTreeHV *h);
	int OpenChildDB(const char *name, size_t sz, const DBTreeHC *parent,
			DBTreeHC *child, int flags);
	int OpenValueDB(const char *name, const DBTreeHC *parent, DBTreeHV *child,
			int flags);
	int WriteValue(DBTreeHV *h, const WValue &v);
	int ReadValue(DBTreeHV *h, WValue &v);
	int FreeValueP(DBPage p);
	int FreeValueV(DBTreeHV *h);
	int ReAllocP(DBTreeHV *h,uint newsize);
public:
	void Test();
};

#endif /* SRC_ICODE_CORE_DB_DBCONFIGMANAGER_H_ */
