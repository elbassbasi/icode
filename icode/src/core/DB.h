/*
 * DB.h
 *
 *  Created on: 12 nov. 2019
 *      Author: azeddine
 */

#ifndef SRC_ICODE_CORE_DB_DB_H_
#define SRC_ICODE_CORE_DB_DB_H_
#include "../icode.h"
typedef unsigned int DBPage;
class DB {
private:
	WFile file;
	WLock lock;
	DBPage end_pos;
	DBPage rm_list;
	DBPage root;
	wint64 file_pos;
	wint64 file_size;
	unsigned short pageSize;
	unsigned short flags;
	void Init();
	int InitFile(int pageSize);
	int CheckFile();
	int WriteEnd(unsigned int sz);
	int NewPagesFromEnd(DBPage *block, int nbpages);
	int FindPagesFromRmList(DBPage *block, int nbpages);
	int NewPages(DBPage *block, uint nbpages);
	int DeletePages(DBPage blocks, uint nbpages);
public:
	DB();
	~DB();
	int OpenFile(const char *filename, const char *mode);
	int OpenFile(const char *filename);
	int OpenFileRO(const char *filename);
	int CreateFile(const char *filename, int blockSize = 0);
	bool IsOpened();
	int Alloc(DBPage *pages, uint size, uint *size_allocated);
	int Alloc(DBPage *pages, uint size) {
		return Alloc(pages, size, 0);
	}
	int Free(DBPage pages, uint size);
	int Realloc(DBPage *pages, uint newsize, uint lastsize,
			uint *size_allocated);
	int Realloc(DBPage *pages, uint newsize, uint lastsize) {
		return Realloc(pages, newsize, lastsize, 0);
	}
	int GetRoot(DBPage *root);
	int SetRoot(DBPage root);
	int SeekToPage(DBPage block, int pos);
	int SeekToPage(DBPage block){
		return SeekToPage(block,0);
	}
	int SeekRead(DBPage block, int pos, void *data, uint size);
	int SeekRead(DBPage block, void *data, uint size){
		return SeekRead(block,0,data,size);
	}
	int SeekWrite(DBPage block, int pos, const void *data, uint size);
	int SeekWrite(DBPage block, const void *data, uint size){
		return SeekWrite(block,0,data,size);
	}
	int Read(void *data, uint size);
	int Write(const void *data, uint size);
	void Lock() {
		lock.Lock();
	}
	void UnLock() {
		lock.UnLock();
	}
	void Flush() {
		file.Flush();
	}
};

#endif /* SRC_ICODE_CORE_DB_DB_H_ */
