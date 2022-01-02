/*
 * Database.h
 *
 *  Created on: 8 nov. 2021
 *      Author: azeddine
 */

#ifndef ICODE_SRC_DB_DATABASE_H_
#define ICODE_SRC_DB_DATABASE_H_
#include "../icode.h"
class Database;
class _DatabaseChunk {
private:
	friend class Database;
	_DatabaseChunk *next;
	wuint index;
	wushort write_start;
	wushort write_end;
	wushort read_end;
	wushort ref;
	char fBuffer[0];
public:

};
class DatabaseChunk {
public:
	DatabaseChunk() {
		memset(this->data, 0, sizeof(this->data));
	}
	void *data[2];
};
typedef int BTreeComparator(Database *db, wuint record1, wuint record2,
		void *userdata);
typedef int BTreeVisitor(Database *db, wuint record, void *userdata);
class Database {
public:
	enum {
		READONLY = 1 << 0, //
		CHUNK_SIZE = 1024 * 4, //
		CHUNK_CACHE_COUNT = 16,
	};
	Database();
	~Database();
private:
	wintptr fFile;
	_DatabaseChunk *firstchunks;
	wushort chunksCount;
	wuchar blockSizesBits;
	WLock fCache;
	int OpenFile(const char *location, int flags);
	int Read(void *buf, size_t size);
	int Write(const void *buf, size_t size);
	int Seek(wuint64 position);
	wuint64 GetFileSize();
	void InitFile(int rootsize);
	void InitChuncks(int chunksCaches);
	void FlushChunk(_DatabaseChunk *chunk);
public:
	int Open(const char *location, int flags, int rootsize, int chunksCaches);
	void Close();
	int GetRoot(wuint &block);
	int Malloc(wuint &block, int datasize);
	int MallocZero(wuint &block, int datasize);
	int Realloc(wuint &block, uint newsize, uint lastsize);
	int Free(wuint block, int datasize);
	int GetAllocatedSize(int size) {
		return ((size + (1 << this->blockSizesBits) - 1) >> this->blockSizesBits)
				<< this->blockSizesBits;
	}
	void Flush();
public:
	int LoadChunk(wuint offset, size_t size, DatabaseChunk &chunk);
	int LoadChunk(wuint block, wuint offset, size_t size,
			DatabaseChunk &chunk) {
		return LoadChunk((block << this->blockSizesBits) + offset, size, chunk);
	}
	int ReleaseChunk(DatabaseChunk &chunk);
	void FlushChunk(DatabaseChunk &chunk);
	int PutChunkBytes(DatabaseChunk &chunk, wuint Offset, const void *data,
			size_t size, bool SetZero);
	int PutChunkBytes(DatabaseChunk &chunk, wuint Offset, const void *data,
			size_t size) {
		return PutChunkBytes(chunk, Offset, data, size, false);
	}
	int PutChunkZeros(DatabaseChunk &chunk, wuint Offset, size_t size) {
		return PutChunkBytes(chunk, Offset, 0, size, true);
	}
	int PutChunkZeros(DatabaseChunk &chunk, wuint block, wuint Offset,
			size_t size) {
		return PutChunkZeros(chunk, (block << this->blockSizesBits) + Offset,
				size);
	}
	int PutChunkBytes(DatabaseChunk &chunk, wuint block, wuint Offset,
			const void *data, size_t size) {
		return PutChunkBytes(chunk, (block << this->blockSizesBits) + Offset,
				data, size);
	}
	int GetChunkBytes(DatabaseChunk &chunk, wuint Offset, void *data,
			size_t size);
	int GetChunkBytes(DatabaseChunk &chunk, wuint block, wuint Offset,
			void *data, size_t size) {
		return GetChunkBytes(chunk, (block << this->blockSizesBits) + Offset,
				data, size);
	}
	int PutChunk3ByteInt(DatabaseChunk &chunk, wuint Offset, int value) {
		wuchar v[3];
		Put3ByteInt(v, value);
		return PutChunkBytes(chunk, Offset, v, 3);
	}
	int PutChunk3ByteInt(DatabaseChunk &chunk, wuint block, wuint Offset,
			int value) {
		wuchar v[3];
		Put3ByteInt(v, value);
		return PutChunkBytes(chunk, (block << this->blockSizesBits) + Offset, v,
				3);
	}
	int GetChunk3ByteInt(DatabaseChunk &chunk, wuint Offset, int *value) {
		wuchar v[3];
		int ret = GetChunkBytes(chunk, Offset, v, 3);
		if (ret >= 3)
			*value = Get3ByteInt(v);
		return ret;
	}
	int GetChunk3ByteInt(DatabaseChunk &chunk, wuint block, wuint Offset,
			int *value) {
		wuchar v[3];
		int ret = GetChunkBytes(chunk, (block << this->blockSizesBits) + Offset,
				v, 3);
		if (ret >= 3)
			*value = Get3ByteInt(v);
		return ret;
	}
	int PutChunkInt(DatabaseChunk &chunk, wuint Offset, wuint value) {
		wuchar v[4];
		PutInt(v, value);
		return PutChunkBytes(chunk, Offset, v, 4);
	}
	int PutChunkInt(DatabaseChunk &chunk, wuint block, wuint Offset,
			wuint value) {
		wuchar v[4];
		PutInt(v, value);
		return PutChunkBytes(chunk, (block << this->blockSizesBits) + Offset, v,
				4);
	}
	int GetChunkInt(DatabaseChunk &chunk, wuint Offset, wuint *value) {
		wuchar v[4];
		int ret = GetChunkBytes(chunk, Offset, v, 4);
		if (ret >= 4)
			*value = GetInt(v);
		return ret;
	}
	int GetChunkInt(DatabaseChunk &chunk, wuint block, wuint Offset,
			wuint *value) {
		wuchar v[4];
		int ret = GetChunkBytes(chunk, (block << this->blockSizesBits) + Offset,
				v, 4);
		if (ret >= 4)
			*value = GetInt(v);
		return ret;
	}
	int PutChunkShort(DatabaseChunk &chunk, wuint Offset, wushort value) {
		wuchar v[2];
		PutShort(v, value);
		return PutChunkBytes(chunk, Offset, v, 2);
	}
	int PutChunkShort(DatabaseChunk &chunk, wuint block, wuint Offset,
			wushort value) {
		wuchar v[2];
		PutShort(v, value);
		return PutChunkBytes(chunk, (block << this->blockSizesBits) + Offset, v,
				2);
	}
	int GetChunkShort(DatabaseChunk &chunk, wuint Offset, wushort *value) {
		wuchar v[2];
		int ret = GetChunkBytes(chunk, Offset, v, 2);
		if (ret >= 2)
			*value = GetShort(v);
		return ret;
	}
	int GetChunkShort(DatabaseChunk &chunk, wuint block, wuint Offset,
			wushort *value) {
		wuchar v[2];
		int ret = GetChunkBytes(chunk, (block << this->blockSizesBits) + Offset,
				v, 2);
		if (ret >= 2)
			*value = GetShort(v);
		return ret;
	}
	int PutChunkByte(DatabaseChunk &chunk, wuint Offset, wuchar value) {
		return PutChunkBytes(chunk, Offset, &value, 1);
	}
	int PutChunkByte(DatabaseChunk &chunk, wuint block, wuint Offset,
			wuchar value) {
		return PutChunkBytes(chunk, (block << this->blockSizesBits) + Offset,
				&value, 1);
	}
	int GetChunkByte(DatabaseChunk &chunk, wuint Offset, wuchar *value) {
		int ret = GetChunkBytes(chunk, Offset, value, 1);
		return ret;
	}
	int GetChunkByte(DatabaseChunk &chunk, wuint block, wuint Offset,
			wuchar *value) {
		int ret = GetChunkBytes(chunk, (block << this->blockSizesBits) + Offset,
				value, 1);
		return ret;
	}
	int GetChunkLong(DatabaseChunk &chunk, wuint Offset, wuint64 *value) {
		wuchar v[4];
		int ret = GetChunkBytes(chunk, Offset, v, 4);
		if (ret >= 4)
			*value = GetLong(v);
		return ret;
	}
	int GetChunkLong(DatabaseChunk &chunk, wuint block, wuint Offset,
			wuint64 *value) {
		wuchar v[4];
		int ret = GetChunkBytes(chunk, (block << this->blockSizesBits) + Offset,
				v, 4);
		if (ret >= 4)
			*value = GetLong(v);
		return ret;
	}
	int PutChunkLong(DatabaseChunk &chunk, wuint Offset, wuint64 value) {
		wuchar v[8];
		PutLong(v, value);
		return PutChunkBytes(chunk, Offset, v, 8);
	}
	int PutChunkLong(DatabaseChunk &chunk, wuint block, wuint Offset,
			wuint64 value) {
		wuchar v[8];
		PutLong(v, value);
		return PutChunkBytes(chunk, (block << this->blockSizesBits) + Offset, v,
				8);
	}
	static void Put3ByteInt(wuchar *fBuffer, int value) {
		fBuffer[0] = (value >> 16);
		fBuffer[1] = (value >> 8);
		fBuffer[2] = (value);
	}
	static int Get3ByteInt(wuchar *fBuffer) {
		return ((fBuffer[0] & 0xff) << 16) | ((fBuffer[1] & 0xff) << 8)
				| ((fBuffer[2] & 0xff) << 0);
	}
	static void PutInt(wuchar *fBuffer, int value) {
		fBuffer[0] = (value >> 24);
		fBuffer[1] = (value >> 16);
		fBuffer[2] = (value >> 8);
		fBuffer[3] = (value);
	}
	static int GetInt(wuchar *fBuffer) {
		return ((fBuffer[0] & 0xff) << 24 | (fBuffer[1] & 0xff) << 16)
				| ((fBuffer[2] & 0xff) << 8) | ((fBuffer[3] & 0xff) << 0);
	}
	static void PutShort(wuchar *fBuffer, wushort value) {
		fBuffer[0] = (value >> 8);
		fBuffer[1] = (value);
	}
	static wushort GetShort(wuchar *fBuffer) {
		return (wushort) (((fBuffer[0] << 8) | (fBuffer[1] & 0xff)));
	}
	static wuint64 GetLong(wuchar *fBuffer) {
		return ((((wuint64) fBuffer[0] & 0xff) << 56)
				| (((wuint64) fBuffer[1] & 0xff) << 48)
				| (((wuint64) fBuffer[2] & 0xff) << 40)
				| (((wuint64) fBuffer[3] & 0xff) << 32)
				| (((wuint64) fBuffer[4] & 0xff) << 24)
				| (((wuint64) fBuffer[5] & 0xff) << 16)
				| (((wuint64) fBuffer[6] & 0xff) << 8)
				| (((wuint64) fBuffer[7] & 0xff) << 0));
	}
	static void PutLong(wuchar *fBuffer, wuint64 value) {
		fBuffer[0] = (value >> 56);
		fBuffer[1] = (value >> 48);
		fBuffer[2] = (value >> 40);
		fBuffer[3] = (value >> 32);
		fBuffer[4] = (value >> 24);
		fBuffer[5] = (value >> 16);
		fBuffer[6] = (value >> 8);
		fBuffer[7] = (value);
	}
public:
	/* BTree */
	int BTreeVisit(wuint rootOffset, BTreeVisitor compare, BTreeVisitor visit,
			void *userdata, int degree);
	int BTreeVisit(wuint block, wuint Offset, BTreeVisitor compare,
			BTreeVisitor visit, void *userdata, int degree) {
		return BTreeVisit((block << this->blockSizesBits) + Offset, compare,
				visit, userdata, degree);
	}
	int BTreeInsert(wuint rootOffset, wuint record, BTreeComparator cmp,
			void *userdata, int degree);
	int BTreeInsert(wuint block, wuint Offset, wuint record,
			BTreeComparator cmp, void *userdata, int degree) {
		return BTreeInsert((block << this->blockSizesBits) + Offset, record,
				cmp, userdata, degree);
	}
	int BTreeDelete(wuint rootOffset, wuint record, BTreeComparator cmp,
			void *userdata, int degree);
	int BTreeDelete(wuint block, wuint Offset, wuint record,
			BTreeComparator cmp, void *userdata, int degree) {
		return BTreeDelete((block << this->blockSizesBits) + Offset, record,
				cmp, userdata, degree);
	}
private:
	class BTNode {
	public:
		wuint node;
		int keyCount;
		DatabaseChunk chunk;
		int Init(Database *db, wuint node, int degree);
		void Release(Database *db);
		int GetChild(Database *db, int index, int degree, BTNode &node);
	};
	int _BTreeInsert0(wuint rootOffset, wuint record, BTreeComparator cmp,
			void *userdata, int degree, wuint parent, wuint iParent,
			wuint node);
	int _BTreeDelete0(wuint rootOffset, BTreeComparator cmp, void *userdata,
			int degree, wuint key, wuint nodeRecord, wuint *subst, int mode);
	int _BTreeMergeNodes(wuint rootOffset, int degree, BTNode *src,
			BTNode *keyProvider, int kIndex, BTNode *dst);
	int _BTreeNodeContentCopy(int degree, BTNode *src, int srcPos, BTNode *dst,
			int dstPos, int length);
	int _BTreeNodeContentDelete(int degree, BTNode *node, int i, int length);
	int _BTreeVisit0(wuint node, BTreeVisitor compare, BTreeVisitor visit,
			void *userdata, int degree);
public:
	/* Red-Black Tree*/
	typedef int RBTreeComparator(Database *db, wuint record, void *userdata);
	enum {
		RBTREE_FIND = 0, //
		RBTREE_INSERT = 1 << 0, //
		RBTREE_ALLOC = 1 << 1, //
		RBTREE_ALLOC_INSERT = RBTREE_ALLOC | RBTREE_INSERT, //

	};
	int RBTreeVisit(wuint rootOffset, wuint recordOffset, BTreeVisitor compare,
			BTreeVisitor visit, void *userdata);
	int RBTreeVisit(wuint block, wuint Offset, wuint recordOffset,
			BTreeVisitor compare, BTreeVisitor visit, void *userdata) {
		return RBTreeVisit((block << this->blockSizesBits) + Offset,
				recordOffset, compare, visit, userdata);
	}
	int RBTreeInsert(wuint rootOffset, wuint &record, wuint recordOffset,
			RBTreeComparator cmp, void *userdata, int flags);
	int RBTreeInsert(wuint block, wuint Offset, wuint &record,
			wuint recordOffset, RBTreeComparator cmp, void *userdata,
			int flags) {
		return RBTreeInsert((block << this->blockSizesBits) + Offset, record,
				recordOffset, cmp, userdata, flags);
	}
	int RBTreeDelete(wuint rootOffset, wuint record, wuint recordOffset,
			BTreeComparator cmp, void *userdata);
	int RBTreeDelete(wuint block, wuint Offset, wuint record,
			wuint recordOffset, BTreeComparator cmp, void *userdata) {
		return RBTreeDelete((block << this->blockSizesBits) + Offset, record,
				recordOffset, cmp, userdata);
	}
	int RBTreePrint(wuint rootOffset, wuint recordOffset);
	int RBTreePrint(wuint block, wuint Offset, wuint recordOffset) {
		return RBTreePrint((block << this->blockSizesBits) + Offset,
				recordOffset);
	}
	int _RBTreePrint0(wuint block, wuint recordOffset, int depth, char *path,
			bool is_right);
	int RBTreeForAll(wuint rootOffset, wuint recordOffset, RBTreeComparator cmp,
			void *userdata);
	int RBTreeForAll(wuint block, wuint Offset, wuint recordOffset,
			RBTreeComparator cmp, void *userdata) {
		return RBTreeForAll((block << this->blockSizesBits) + Offset,
				recordOffset, cmp, userdata);
	}
	int _RBTreeForAll0(wuint block, wuint recordOffset, int depth,
			RBTreeComparator cmp, void *userdata);
};

#endif /* ICODE_SRC_DB_DATABASE_H_ */
