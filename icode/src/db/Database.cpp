/*
 * Database.cpp
 *
 *  Created on: 8 nov. 2021
 *      Author: azeddine
 */

#include "Database.h"
#ifdef __linux
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#endif
Database::Database() {
#ifdef __linux
	this->fFile = -1;
#endif
	this->firstchunks = 0;
	this->chunksCount = 0;
	this->blockSizesBits = 3;
}
wuint64 Database::GetFileSize() {
#ifdef __linux
	struct stat buf;
	if (fstat(this->fFile, &buf) == 0) {
		return buf.st_size;
	} else {
		return -1;
	}
#endif
}
void Database::Close() {
	this->Flush();
	if (this->firstchunks != 0) {
		free(this->firstchunks);
		this->firstchunks = 0;
	}
#ifdef __linux
	if (this->fFile != -1) {
		close(this->fFile);
		this->fFile = -1;
	}
#endif
}

int Database::Read(void *buf, size_t size) {
#ifdef __linux
	return read(this->fFile, buf, size);
#endif
}

int Database::Write(const void *buf, size_t size) {
#ifdef __linux
	return write(this->fFile, buf, size);
#endif
}

int Database::Seek(wuint64 position) {
#ifdef __linux
	return lseek(this->fFile, position, SEEK_SET);
#endif
}

int Database::OpenFile(const char *location, int flags) {
#ifdef __linux
	int oflag;
	if (flags & READONLY) {
		oflag = O_RDONLY | S_IRUSR | S_IWUSR;
	} else {
		oflag = O_RDWR | O_CREAT | O_EXCL | S_IRUSR | S_IWUSR;
	}
	int fd = open(location, oflag);
	if ((fd == -1) && (EEXIST == errno)) {
		/* open the existing file with write flag */
		oflag &= ~ O_CREAT | O_EXCL;
		fd = open(location, oflag);
	}
	if (fd == -1)
		return -1;

	this->fFile = fd;
	return 1;
#endif
}
enum {
	SIGNATURE = 0x44424900, //
	VERSION = 1, //
	BLOCK_SIZE_BITS = 3,
	BLOCK_SIZE = 1 << BLOCK_SIZE_BITS, //
	FREEBLOCK_INDEX = 2 * 4,
	TOTALBLOCK_INDEX = 3 * 4,
	HEADER_SIZE = 4 * 4,
	NBBLOCKS_INDEX = 2,
	NEXTBLOCK_INDEX = 4,
};
Database::~Database() {
	Close();
}
void Database::InitFile(int rootsize) {
	DatabaseChunk chunk;
	LoadChunk(0, 0, HEADER_SIZE, chunk);
	int nb_blocks1 = (HEADER_SIZE + (1 << blockSizesBits) - 1)
			>> blockSizesBits;
	int nb_blocks2 = (rootsize + (1 << blockSizesBits) - 1) >> blockSizesBits;
	this->PutChunkZeros(chunk, nb_blocks1, 0, nb_blocks2 << blockSizesBits);
	this->PutChunkInt(chunk, TOTALBLOCK_INDEX, nb_blocks1 + nb_blocks2); // total of Block
	this->PutChunkInt(chunk, 0 * 4, SIGNATURE); // signature
	this->PutChunkInt(chunk, 1 * 4, (VERSION << 8) | blockSizesBits); // version && blockSizes
	this->PutChunkInt(chunk, FREEBLOCK_INDEX, 0); // free Block
	ReleaseChunk(chunk);
	this->FlushChunk(chunk);
}

void Database::InitChuncks(int chunksCaches) {
	const int chunksAllocated = chunksCaches;
	this->firstchunks = (_DatabaseChunk*) calloc(1,
			chunksAllocated * (sizeof(_DatabaseChunk) + CHUNK_SIZE));
	if (this->firstchunks == 0)
		return;
	_DatabaseChunk *chunks = this->firstchunks;
	_DatabaseChunk *chunk, *prev = chunks;
	for (int i = 1; i < chunksAllocated; i++) {
		chunk = (_DatabaseChunk*) &((char*) chunks)[i
				* (sizeof(_DatabaseChunk) + CHUNK_SIZE)];
		if (i < chunksAllocated - 1) {
			prev->next = chunk;
		} else {
			prev->next = 0;
		}
		chunk->index = -1;
		prev = chunk;
	}
}
int Database::Open(const char *location, int flags, int rootsize,
		int chunksCaches) {
	int ret = OpenFile(location, flags);
	if (ret < 0)
		return ret;
	InitChuncks(chunksCaches);
	if (this->firstchunks == 0)
		return -2;
	//wuint64 size = GetFileSize();
	ret = this->Read(firstchunks->fBuffer, CHUNK_SIZE);
	if (ret < 0)
		return ret;
	firstchunks->read_end = ret;
	if (ret == 0) {
		if ((flags & READONLY) != 0)
			return -1;
		InitFile(rootsize);
	} else {
	}
	wuint version = 0;
	DatabaseChunk firstchunks;
	LoadChunk(0, HEADER_SIZE, firstchunks);
	this->GetChunkInt(firstchunks, 0, 1 * 4, &version); // version && blockSizes
	this->blockSizesBits = version & 0xFF;
	if (this->blockSizesBits < 3)
		this->blockSizesBits = 3;
	return ret;
}

int Database::LoadChunk(wuint offset, size_t size, DatabaseChunk &chunk) {
	memset(chunk.data, 0, sizeof(chunk.data));
	if (size > CHUNK_SIZE)
		return -1;
	const wuint index1 = offset / CHUNK_SIZE;
	const wuint index2 = (offset + size) / CHUNK_SIZE;
	if (index1 == 0 && index2 == 0) {
		chunk.data[0] = this->firstchunks;
		chunk.data[1] = 0;
		this->firstchunks->ref++;
		return 1;
	}
	fCache.Lock();
	_DatabaseChunk *chunk1 = 0, *chunk2 = 0, *prev = 0, *free1 = 0, *free2 = 0,
			*free1_prev = 0, *free2_prev = 0;
	_DatabaseChunk *chunks = this->firstchunks;
	while (chunks != 0) {
		if (chunks->index == index1) {
			if (prev != 0) {
				prev->next = chunks->next;
				chunk1 = chunks;
				chunks = chunks->next;
				chunk1->next = firstchunks->next;
				firstchunks->next = chunk1;
			} else {
				chunk1 = chunks;
				chunks = chunks->next;
			}
			if (index1 == index2 || chunk2 != 0) {
				fCache.UnLock();
				return 1;
			}
			prev = chunk1;
		} else if (index1 != index2 && chunks->index == index2) {
			if (prev != 0) {
				prev->next = chunks->next;
				chunk2 = chunks;
				chunks = chunks->next;
				chunk2->next = firstchunks->next;
				firstchunks->next = chunk2;
			} else {
				chunk2 = chunks;
				chunks = chunks->next;
			}
			if (chunk2 != 0) {
				fCache.UnLock();
				return 1;
			}
			prev = chunk2;
		} else {
			if (chunks->ref == 0) {
				if (index1 != index2) {
					free2 = free1;
					free2_prev = free1_prev;
				}
				free1 = chunks;
				free1_prev = prev;
			}
			prev = chunks;
			chunks = chunks->next;
		}
	}
	if (chunk1 == 0 && free1 == 0)
		return -1;
	if (index1 != index2 && chunk2 == 0 && free2 == 0)
		return -1;
	DatabaseChunk tmp;
	tmp.data[0] = free1;
	tmp.data[1] = free2;
	FlushChunk(tmp);
	int ret;
	if (chunk1 == 0) {
		chunk1 = free1;
		ret = this->Seek(index1 * CHUNK_SIZE);
		if (ret >= 0) {
			ret = this->Read(free1->fBuffer, CHUNK_SIZE);
			if (ret >= 0) {
				free1->index = index1;
				free1->write_start = 0;
				free1->write_end = 0;
				free1_prev->next = free1->next;
				free1->next = firstchunks->next;
				firstchunks->next = free1;
				free1->read_end = ret;
			}
		}
	}
	if (chunk2 == 0) {
		if (index1 != index2 && ret > 0) {
			chunk2 = free2;
			ret = this->Seek(index2 * CHUNK_SIZE);
			if (ret >= 0) {
				ret = this->Read(free2->fBuffer, CHUNK_SIZE);
				if (ret >= 0) {
					free2->index = index2;
					free2->write_start = 0;
					free2->write_end = 0;
					free2_prev->next = free2->next;
					free2->next = firstchunks->next;
					firstchunks->next = free2;
					free2->read_end = ret;
				}
			}
		}
	}
	if (ret >= 0) {
		chunk.data[0] = chunk1;
		chunk.data[1] = chunk2;
		chunk1->ref++;
		if (chunk2)
			chunk2->ref++;
	}
	fCache.UnLock();
	return ret;
}
int Database::ReleaseChunk(DatabaseChunk &chunk) {
	_DatabaseChunk *chunk1 = (_DatabaseChunk*) chunk.data[0];
	_DatabaseChunk *chunk2 = (_DatabaseChunk*) chunk.data[1];
	if (chunk1 != 0) {
		chunk1->ref--;
	}
	if (chunk2 != 0) {
		chunk2->ref--;
	}
	chunk.data[0] = 0;
	chunk.data[1] = 0;
	return 1;
}
int Database::PutChunkBytes(DatabaseChunk &chunk, wuint Offset,
		const void *data, size_t size, bool SetZero) {
	_DatabaseChunk *chunk1 = (_DatabaseChunk*) chunk.data[0];
	_DatabaseChunk *chunk2 = (_DatabaseChunk*) chunk.data[1];
	if (chunk1 == 0)
		return -1;
	int ret;
	if (Offset < HEADER_SIZE) {
		ret = -1;
	}
	wuint index1 = Offset / CHUNK_SIZE;
	wuint index2 = (Offset + size) / CHUNK_SIZE;
	wuint Chunkoffset = Offset % CHUNK_SIZE;
	if (index1 != chunk1->index)
		return 0;
	if (index1 != index2) {
		if (chunk2 == 0)
			return -1;
		if (index2 != chunk2->index)
			return 0;
	}
	int sz = size;
	if ((Chunkoffset + size) >= CHUNK_SIZE)
		sz = CHUNK_SIZE - Chunkoffset;
	if (chunk1->write_start == chunk1->write_end) {
		chunk1->write_start = Chunkoffset;
		chunk1->write_end = Chunkoffset + sz;
		if (chunk1->read_end < chunk1->write_end)
			chunk1->read_end = chunk1->write_end;
	} else {
		if (Chunkoffset < chunk1->write_start)
			chunk1->write_start = Chunkoffset;
		if ((Chunkoffset + sz) > chunk1->write_end) {
			chunk1->write_end = Chunkoffset + sz;
			if (chunk1->read_end < chunk1->write_end)
				chunk1->read_end = chunk1->write_end;
		}
	}
	if (SetZero) {
		memset(&chunk1->fBuffer[Chunkoffset], 0, sz);
	} else {
		memcpy(&chunk1->fBuffer[Chunkoffset], data, sz);
	}
	if (index1 != index2) {
		sz = (Chunkoffset + size) - CHUNK_SIZE;
		if (chunk2->write_start == chunk2->write_end) {
			chunk2->write_start = 0;
			chunk2->write_end = sz;
			if (chunk2->read_end < chunk2->write_end)
				chunk2->read_end = chunk2->write_end;
		} else {
			if (Chunkoffset < chunk2->write_start)
				chunk2->write_start = Chunkoffset;
			if (sz > chunk2->write_end) {
				chunk2->write_end = sz;
				if (chunk2->read_end < chunk2->write_end)
					chunk2->read_end = chunk2->write_end;
			}
		}
		if (SetZero) {
			memset(&chunk2->fBuffer, 0, sz);
		} else {
			memcpy(&chunk2->fBuffer, &((char*) data)[CHUNK_SIZE - Chunkoffset],
					sz);
		}
	}
	return 1;
}
int Database::GetChunkBytes(DatabaseChunk &chunk, wuint Offset, void *data,
		size_t size) {
	_DatabaseChunk *chunk1 = (_DatabaseChunk*) chunk.data[0];
	_DatabaseChunk *chunk2 = (_DatabaseChunk*) chunk.data[1];
	if (chunk1 == 0)
		return -1;
	int ret;
	wuint index1 = Offset / CHUNK_SIZE;
	wuint index2 = (Offset + size) / CHUNK_SIZE;
	wuint Chunkoffset = Offset % CHUNK_SIZE;
	if (index1 != chunk1->index)
		return 0;
	if (index1 != index2) {
		if (chunk2 == 0)
			return -1;
		if (index2 != chunk2->index)
			return 0;
	}
	int sz = size;
	if ((Chunkoffset + size) >= CHUNK_SIZE)
		sz = CHUNK_SIZE - Chunkoffset;
	if (Chunkoffset + sz > chunk1->read_end)
		return 0;
	memcpy(data, &chunk1->fBuffer[Chunkoffset], sz);
	if (index1 != index2) {
		sz = (Chunkoffset + size) - CHUNK_SIZE;
		if (sz > chunk2->read_end)
			return 0;
		memcpy(&((char*) data)[CHUNK_SIZE - Chunkoffset], &chunk2->fBuffer, sz);
	}
	return size;
}
void Database::FlushChunk(_DatabaseChunk *_chunk) {
	if (_chunk->write_start != _chunk->write_end) {
		int ret = this->Seek(_chunk->index * CHUNK_SIZE + _chunk->write_start);
		if (ret >= 0) {
			ret = this->Write(&_chunk->fBuffer[_chunk->write_start],
					_chunk->write_end - _chunk->write_start);
		}
		_chunk->write_start = 0;
		_chunk->write_end = 0;
	}
}
void Database::FlushChunk(DatabaseChunk &chunk) {
	for (int i = 0; i < 2; i++) {
		_DatabaseChunk *_chunk = (_DatabaseChunk*) chunk.data[i];
		if (_chunk != 0)
			FlushChunk(_chunk);
	}
}
void Database::Flush() {
	_DatabaseChunk *chunks = this->firstchunks;
	while (chunks != 0) {
		FlushChunk(chunks);
		chunks = chunks->next;
	}
#ifdef __linux
	//::fsync(fFile);
#endif
}
int Database::GetRoot(wuint &block) {
	block = (HEADER_SIZE + (1 << blockSizesBits) - 1) >> blockSizesBits;
	return 1;
}

int Database::Malloc(wuint &block, int datasize) {
	const int blockSize = 1 << this->blockSizesBits;
	int ret;
	int nb_blocks = (datasize + blockSize - 1) >> blockSizesBits;
	wuint freeBlocks = 0;
	DatabaseChunk chunk, firstchunks;
	LoadChunk(0, 0, HEADER_SIZE, firstchunks);
	GetChunkInt(firstchunks, 0, FREEBLOCK_INDEX, &freeBlocks);
	if (freeBlocks != 0) {
		wuint blocks = freeBlocks;
		wuint prevBlocks = 0;
		wushort length;
		while (1) {
			prevBlocks = blocks;
			int ret = LoadChunk(blocks, 0, NEXTBLOCK_INDEX + 4, chunk);
			if (ret < 0)
				break;
			GetChunkShort(chunk, blocks, NBBLOCKS_INDEX, &length);
			GetChunkInt(chunk, blocks, NEXTBLOCK_INDEX, &blocks);
			if (length >= nb_blocks) {
				if (prevBlocks == freeBlocks) {
					PutChunkInt(firstchunks, 0, FREEBLOCK_INDEX, blocks);
				} else {
					PutChunkInt(chunk, prevBlocks, 4, blocks);
				}
				if (length != nb_blocks) {
					Free(prevBlocks + nb_blocks,
							(length - nb_blocks) << this->blockSizesBits);
				}
				block = prevBlocks;
				ReleaseChunk(firstchunks);
				ReleaseChunk(chunk);
				return 1;
			}
			ReleaseChunk(chunk);
		}
	}
	wuint endBlocks = 0;
	GetChunkInt(firstchunks, 0, TOTALBLOCK_INDEX, &endBlocks);
	block = endBlocks;
	endBlocks += nb_blocks;
	PutChunkInt(firstchunks, 0, TOTALBLOCK_INDEX, endBlocks);
	ReleaseChunk(firstchunks);
	ret = 1;
	return ret;
}
int Database::MallocZero(wuint &block, int datasize) {
	int ret = Malloc(block, datasize);
	if (ret > 0) {
		DatabaseChunk chunk;
		ret = LoadChunk(block, 0, datasize, chunk);
		if (ret > 0) {
			PutChunkZeros(chunk, block, 0, datasize);
		}
		ReleaseChunk(chunk);
	}
	return ret;
}
int Database::Realloc(wuint &block, uint newsize, uint lastsize) {
	const int blockSize = 1 << this->blockSizesBits;
	int ret;
	int nb_blocks_1 = (newsize + blockSize - 1) >> this->blockSizesBits;
	int nb_blocks_2 = (lastsize + blockSize - 1) >> this->blockSizesBits;
	if (nb_blocks_2 >= nb_blocks_1) {
		size_t min_block_rest = 8 / blockSize;
		if (nb_blocks_1 != nb_blocks_2) {
			if ((nb_blocks_2 - nb_blocks_1) <= min_block_rest) {
				ret = 1;
			} else {
				ret = Free(block + nb_blocks_1,
						(nb_blocks_2 - nb_blocks_1) << this->blockSizesBits);
			}
		}
	} else {
		if (block != 0) {
			ret = Free(block, lastsize);
			if (ret < 0)
				return ret;
		}
		ret = Malloc(block, newsize);
	}
	return ret;
}

int Database::Free(wuint block, int datasize) {
	int headerBlock = (HEADER_SIZE + (1 << this->blockSizesBits) - 1)
			>> this->blockSizesBits;
	if (block < headerBlock)
		return -1;
	int nbBlocks = (datasize + (1 << this->blockSizesBits) - 1)
			>> this->blockSizesBits;
	DatabaseChunk chunk, firstchunks;
	int ret = LoadChunk(block, 0, NEXTBLOCK_INDEX + 4, chunk);
	if (ret < 0)
		return ret;
	wuint freeBlocks = 0;
	LoadChunk(0, 0, HEADER_SIZE, firstchunks);
	GetChunkInt(firstchunks, 0, FREEBLOCK_INDEX, &freeBlocks);
	PutChunkShort(chunk, block, NBBLOCKS_INDEX, nbBlocks);
	PutChunkInt(chunk, block, NEXTBLOCK_INDEX, freeBlocks);
	PutChunkInt(firstchunks, 0, FREEBLOCK_INDEX, block);
	ReleaseChunk(chunk);
	ReleaseChunk(firstchunks);
	return 1;
}
/* BTree */
int Database::BTreeInsert(wuint rootOffset, wuint record, BTreeComparator cmp,
		void *userdata, int degree) {
	int ret = 0;
	wuint root = 0;
	DatabaseChunk chunk;
	ret = LoadChunk(rootOffset, 4, chunk);
	if (ret < 0)
		return ret;
	GetChunkInt(chunk, rootOffset, &root);
	if (root == 0) {
		const int MAX_RECORDS = 2 * degree - 1;
		ret = MallocZero(root, (2 * MAX_RECORDS + 1) * 4);
		if (ret >= 0 && root != 0) {
			PutChunkInt(chunk, rootOffset, root);
			ReleaseChunk(chunk);
			ret = LoadChunk(root, 0, 4, chunk);
			if (ret > 0) {
				PutChunkInt(chunk, root, 0 * 4, record);
			}
		}
		ReleaseChunk(chunk);
	} else {
		ReleaseChunk(chunk);
		ret = _BTreeInsert0(rootOffset, record, cmp, userdata, degree, 0, 0,
				root);
	}
	return ret;
}

int Database::_BTreeInsert0(wuint rootOffset, wuint record, BTreeComparator cmp,
		void *userdata, int degree, wuint parent, wuint iParent, wuint node) {
	const int MAX_RECORDS = 2 * degree - 1;
	const int MEDIAN_RECORD = degree - 1;
	const int MAX_CHILDREN = 2 * degree;
	const int NODE_SIZE = (2 * MAX_RECORDS + 1) * 4;
	int ret = 0;
	DatabaseChunk chunk;
	ret = LoadChunk(node, 0, NODE_SIZE, chunk);
	if (ret < 0)
		return ret;
	wuint r;

	GetChunkInt(chunk, node, (MAX_RECORDS - 1) * 4, &r);
	// If this node is full (last record isn't null), split it.
	if (r != 0) {
		wuint median;
		GetChunkInt(chunk, node, MEDIAN_RECORD * 4, &median);
		if (median == record) {
			// Found it, never mind.
			return median;
		} else {
			// Split it.
			// Create the new node and move the larger records over.
			wuint newnode = 0;
			ret = MallocZero(newnode, NODE_SIZE);
			DatabaseChunk newchunk;
			LoadChunk(newnode, 0, NODE_SIZE, newchunk);
			for (int i = 0; i < MEDIAN_RECORD; ++i) {
				GetChunkInt(chunk, node, (MEDIAN_RECORD + 1 + i) * 4, &r);
				PutChunkInt(newchunk, newnode, i * 4, r);
				PutChunkInt(chunk, node, (MEDIAN_RECORD + 1 + i) * 4, 0);

				GetChunkInt(chunk, node,
						(MAX_RECORDS + MEDIAN_RECORD + 1 + i) * 4, &r);
				PutChunkInt(newchunk, newnode, (MAX_RECORDS + i) * 4, r);
				PutChunkInt(chunk, node,
						(MAX_RECORDS + MEDIAN_RECORD + 1 + i) * 4, 0);
			}
			GetChunkInt(chunk, node, (MAX_RECORDS + MAX_RECORDS) * 4, &r);
			PutChunkInt(newchunk, newnode, (MAX_RECORDS + MEDIAN_RECORD) * 4,
					r);
			PutChunkInt(chunk, node, (MAX_RECORDS + MAX_RECORDS) * 4, 0);
			DatabaseChunk pChunk;
			if (parent == 0) {
				// Create a new root
				ret = MallocZero(parent, NODE_SIZE);
				if (ret < 0)
					return ret;

				ret = LoadChunk(parent, 0, (MAX_RECORDS + 1) * 4, pChunk);
				if (ret < 0)
					return ret;
				PutChunkInt(pChunk, parent, MAX_RECORDS * 4, node);
				PutChunkInt(pChunk, parent, iParent * 4, median);
				PutChunkInt(pChunk, parent, (MAX_RECORDS + iParent + 1) * 4,
						newnode);
				ReleaseChunk(pChunk);
				ret = LoadChunk(rootOffset, 4, pChunk);
				if (ret < 0)
					return ret;
				PutChunkInt(pChunk, rootOffset, parent);
				ReleaseChunk(pChunk);
			} else {
				// Insert the median into the parent.
				ret = LoadChunk(parent, 0, (MAX_RECORDS + 1) * 4, pChunk);
				if (ret < 0)
					return ret;
				for (int i = MAX_RECORDS - 2; i >= iParent; --i) {
					GetChunkInt(pChunk, parent, i * 4, &r);
					if (r != 0) {
						PutChunkInt(pChunk, parent, (i + 1) * 4, r);
						GetChunkInt(pChunk, parent, (MAX_RECORDS + i + 1) * 4,
								&r);
						PutChunkInt(pChunk, parent, (MAX_RECORDS + i + 2) * 4,
								r);
					}
				}
				PutChunkInt(pChunk, parent, (iParent + 1) * 4, median);
				PutChunkInt(pChunk, parent, (MAX_CHILDREN + iParent + 1) * 4,
						newnode);
				ReleaseChunk(pChunk);
			}

			PutChunkInt(chunk, node, (MEDIAN_RECORD) * 4, 0);

			// Set the node to the correct one to follow.
			if (cmp(this, record, median, userdata) > 0) {
				node = newnode;
				ReleaseChunk(chunk);
				memcpy(&chunk, &newchunk, sizeof(chunk));
			} else {
				ReleaseChunk(newchunk);
			}
		}
	}

	// Binary search to find the insertion point.
	int lower = 0;
	int upper = MAX_RECORDS - 1;
	while (lower < upper) {
		GetChunkInt(chunk, node, (upper - 1) * 4, &r);
		if (r != 0)
			break;
		upper--;
	}

	while (lower < upper) {
		int middle = (lower + upper) >> 1;
		wuint checkRec = 0;
		GetChunkInt(chunk, node, middle * 4, &checkRec);
		if (checkRec == 0) {
			upper = middle;
		} else {
			int compare = cmp(this, checkRec, record, userdata);
			if (compare > 0) {
				upper = middle;
			} else if (compare < 0) {
				lower = middle + 1;
			} else {
				ReleaseChunk(chunk);
				// Found it, no insert, just return the matched record.
				return checkRec;
			}
		}
	}
	int i = lower;
	wuint child = 0;
	GetChunkInt(chunk, node, (MAX_RECORDS + i) * 4, &child);
	if (child != 0) {
		ReleaseChunk(chunk);
		// Visit the children.
		return _BTreeInsert0(rootOffset, record, cmp, userdata, degree, node, i,
				child);
	} else {
		// We are at the leaf, add us in.
		// First copy everything after over one.
		for (int j = MAX_RECORDS - 2; j >= i; --j) {
			GetChunkInt(chunk, node, j * 4, &r);
			if (r != 0) {
				PutChunkInt(chunk, node, (j + 1) * 4, r);
			}
		}
		PutChunkInt(chunk, node, i * 4, record);
		ReleaseChunk(chunk);
		return record;
	}
}
enum {
	DELMODE_NORMAL = 0, DELMODE_DELETE_MINIMUM = 1, DELMODE_DELETE_MAXIMUM = 2,
};
int Database::BTNode::Init(Database *db, wuint node, int degree) {
	const int MAX_RECORDS = 2 * degree - 1;
	this->node = 0;
	int ret = db->LoadChunk(node, MAX_RECORDS * 4, chunk);
	if (ret > 0) {
		this->node = node;
		int i = 0;
		while (i < MAX_RECORDS) {
			wuint r = 0;
			ret = db->GetChunkInt(chunk, node, i * 4, &r);
			if (r == 0)
				break;
			i++;
		}
		keyCount = i;
	}
	return ret;
}

void Database::BTNode::Release(Database *db) {
	db->ReleaseChunk(chunk);
	this->node = 0;
}

int Database::BTNode::GetChild(Database *db, int index, int degree,
		BTNode &node) {
	const int MAX_CHILDREN = 2 * degree;
	const int MAX_RECORDS = 2 * degree - 1;
	int ret = 0;
	node.node = 0;
	if (0 <= index && index < MAX_CHILDREN) {
		wuint child = 0;
		int ret = db->GetChunkInt(chunk, this->node, (MAX_RECORDS + index) * 4,
				&child);
		if (child != 0) {
			ret = node.Init(db, child, degree);
		}
	}
	return ret;
}
int Database::BTreeDelete(wuint rootOffset, wuint record, BTreeComparator cmp,
		void *userdata, int degree) {
	DatabaseChunk chunk;
	wuint rootNode = 0;
	int ret = LoadChunk(rootOffset, 4, chunk);
	if (ret > 0) {
		GetChunkInt(chunk, rootOffset, &rootNode);
		ReleaseChunk(chunk);
	}
	wuint subst = 0;
	return _BTreeDelete0(rootOffset, cmp, userdata, degree, record, rootNode,
			&subst, DELMODE_NORMAL);
}
int Database::_BTreeDelete0(wuint rootOffset, BTreeComparator cmp,
		void *userdata, int degree, wuint key, wuint nodeRecord, wuint *subst,
		int mode) {
	const int MAX_RECORDS = 2 * degree - 1;
	const int MIN_RECORDS = degree - 1;
	const int MEDIAN_RECORD = degree - 1;
	const int MAX_CHILDREN = 2 * degree;
	const int NODE_SIZE = (2 * MAX_RECORDS + 1) * 4;
	int ret;
	wuint r;
	BTNode node;
	ret = node.Init(this, nodeRecord, degree);
	if (ret < 0)
		return ret;

	// Determine index of key in current node, or -1 if its not in this node.
	int keyIndexInNode = -1;
	if (mode == DELMODE_NORMAL)
		for (int i = 0; i < node.keyCount; i++) {
			r = 0;
			ret = this->GetChunkInt(node.chunk, node.node, i * 4, &r);
			if (r == key) {
				keyIndexInNode = i;
				break;
			}
		}
	r = 0;
	this->GetChunkInt(node.chunk, node.node, (MAX_RECORDS + 0) * 4, &r);
	if (r == 0) {
		int i, length;
		/* Case 1: leaf node containing the key (by method precondition) */
		if (keyIndexInNode != -1) {
			*subst = key;
			i = keyIndexInNode;
			length = 1;
		} else {
			if (mode == DELMODE_DELETE_MINIMUM) {
				this->GetChunkInt(node.chunk, node.node, 0 * 4, subst);
				i = 0;
				length = 1;
			} else if (mode == DELMODE_DELETE_MAXIMUM) {
				this->GetChunkInt(node.chunk, node.node,
						(node.keyCount - 1) * 4, subst);
				i = node.keyCount - 1;
				length = 1;
			} else {
				/* key absent*/
				i = -1;
				length = -1;
				*subst = 0;
				ret = -2;
			}
		}
		/* nodeContentDelete */
		if (i >= 0) {
			_BTreeNodeContentDelete(degree, &node, i, length);
		}
		node.Release(this);
		return ret;
	} else {
		if (keyIndexInNode != -1) {
			/* Case 2: non-leaf node which contains the key itself */
			BTNode succ;
			ret = node.GetChild(this, keyIndexInNode + 1, degree, succ);
			if (ret > 0 && succ.keyCount > MIN_RECORDS) {
				/* Case 2a: Delete key by overwriting it with its successor (which occurs in a leaf node) */
				wuint _subst = 0;
				ret = _BTreeDelete0(rootOffset, cmp, userdata, degree, -1,
						succ.node, &_subst, DELMODE_DELETE_MINIMUM);
				if (ret > 0)
					this->PutChunkInt(node.chunk, node.node, keyIndexInNode * 4,
							_subst);
				*subst = key;
				node.Release(this);
				succ.Release(this);
				return ret;
			}

			BTNode pred;
			ret = node.GetChild(this, keyIndexInNode, degree, pred);
			if (ret > 0 && pred.keyCount > MIN_RECORDS) {
				/* Case 2b: Delete key by overwriting it with its predecessor (which occurs in a leaf node) */
				wuint _subst = 0;
				ret = _BTreeDelete0(rootOffset, cmp, userdata, degree, -1,
						pred.node, &_subst, DELMODE_DELETE_MAXIMUM);
				if (ret > 0)
					this->PutChunkInt(node.chunk, node.node, keyIndexInNode * 4,
							_subst);
				*subst = key;
				node.Release(this);
				succ.Release(this);
				pred.Release(this);
				return ret;
			}

			/* Case 2c: Merge successor and predecessor */
			// assert(pred != null && succ != null);
			if (ret > 0) {
				ret = _BTreeMergeNodes(rootOffset, degree, &succ, &node,
						keyIndexInNode, &pred);
				if (ret > 0)
					ret = _BTreeDelete0(rootOffset, cmp, userdata, degree, key,
							pred.node, subst, mode);
				node.Release(this);
				succ.Release(this);
				pred.Release(this);
				return ret;
			}
			*subst = key;
			node.Release(this);
			succ.Release(this);
			pred.Release(this);
			return ret;
		} else {
			/* Case 3: non-leaf node which does not itself contain the key */

			/* Determine root of subtree that should contain the key */
			int subtreeIndex;
			switch (mode) {
			case DELMODE_NORMAL:
				subtreeIndex = node.keyCount;
				for (int i = 0; i < node.keyCount; i++) {
					r == 0;
					this->GetChunkInt(node.chunk, node.node, i * 4, &r);
					int _compare = cmp(this, r, key, userdata);
					if (_compare > 0) {
						subtreeIndex = i;
						break;
					} else if (_compare < -2) {
						node.Release(this);
						return _compare;
					}
				}
				break;
			case DELMODE_DELETE_MINIMUM:
				subtreeIndex = 0;
				break;
			case DELMODE_DELETE_MAXIMUM:
				subtreeIndex = node.keyCount;
				break;
			default:
				/* UnknownMode */
				node.Release(this);
				return -3;
				break;
			}

			BTNode child;
			ret = node.GetChild(this, subtreeIndex, degree, child);
			if (ret <= 0) {
				/* IntegrityError */
				node.Release(this);
				return -3;
			}

			if (child.keyCount > MIN_RECORDS) {
				ret = _BTreeDelete0(rootOffset, cmp, userdata, degree, key,
						child.node, subst, mode);
				node.Release(this);
				child.Release(this);
				return ret;
			} else {
				BTNode sibR;
				ret = node.GetChild(this, subtreeIndex + 1, degree, sibR);
				if (ret > 0 && sibR.keyCount > MIN_RECORDS) {
					/* Case 3a (i): child will underflow upon deletion, take a key from rightSibling */
					wuint rightKey = 0;
					wuint leftmostRightSiblingKey = 0;
					this->GetChunkInt(node.chunk, node.node, subtreeIndex * 4,
							&rightKey);
					this->GetChunkInt(sibR.chunk, sibR.node, 0 * 4,
							&leftmostRightSiblingKey);
					r = 0;
					this->GetChunkInt(sibR.chunk, sibR.node,
							(MAX_RECORDS + 0) * 4, &r);
					/* append*/
					this->PutChunkInt(child.chunk, child.node,
							child.keyCount * 4, rightKey);
					this->PutChunkInt(child.chunk, child.node,
							(MAX_RECORDS + node.keyCount + 1) * 4, r);
					_BTreeNodeContentDelete(degree, &sibR, 0, 1);
					this->PutChunkInt(node.chunk, node.node, subtreeIndex * 4,
							leftmostRightSiblingKey);
					ret = _BTreeDelete0(rootOffset, cmp, userdata, degree, key,
							child.node, subst, mode);
					node.Release(this);
					child.Release(this);
					sibR.Release(this);
					return ret;
				}

				BTNode sibL;
				ret = node.GetChild(this, subtreeIndex - 1, degree, sibL);
				if (ret > 0 && sibL.keyCount > MIN_RECORDS) {
					/* Case 3a (ii): child will underflow upon deletion, take a key from leftSibling */
					wuint leftKey = 0;
					this->GetChunkInt(node.chunk, node.node,
							(subtreeIndex - 1) * 4, &leftKey);
					r = 0;
					this->GetChunkInt(sibL.chunk, sibL.node,
							(MAX_RECORDS + sibL.keyCount) * 4, &r);
					/* prepend */
					_BTreeNodeContentCopy(degree, &child, 0, &child, 1,
							child.keyCount + 1);
					this->PutChunkInt(child.chunk, child.node, 0 * 4, leftKey);
					this->PutChunkInt(child.chunk, child.node,
							(MAX_RECORDS + 0) * 4, r);
					/* end prepend */
					wuint rightmostLeftSiblingKey = 0;
					this->GetChunkInt(sibL.chunk, sibL.node,
							(sibL.keyCount - 1) * 4, &rightmostLeftSiblingKey);
					this->PutChunkInt(sibL.chunk, sibL.node,
							(sibL.keyCount - 1) * 4, 0);
					this->PutChunkInt(sibL.chunk, sibL.node,
							(MAX_RECORDS + sibL.keyCount) * 4, 0);
					this->PutChunkInt(node.chunk, node.node,
							(subtreeIndex - 1) * 4, rightmostLeftSiblingKey);
					ret = _BTreeDelete0(rootOffset, cmp, userdata, degree, key,
							child.node, subst, mode);
					node.Release(this);
					child.Release(this);
					sibR.Release(this);
					sibL.Release(this);
					return ret;
				}

				/* Case 3b (i,ii): leftSibling, child, rightSibling all have minimum number of keys */

				if (sibL.node != 0 || sibR.node != 0) { // merge child into leftSibling
					wuint _nodeRecord;
					if (sibL.node != 0) {
						_nodeRecord = sibL.node;
						ret = _BTreeMergeNodes(rootOffset, degree, &child,
								&node, subtreeIndex - 1, &sibL);
					} else {
						_nodeRecord = child.node;
						ret = _BTreeMergeNodes(rootOffset, degree, &sibR, &node,
								subtreeIndex, &child);
					}
					if (ret > 0)
						ret = _BTreeDelete0(rootOffset, cmp, userdata, degree,
								key, _nodeRecord, subst, mode);
					node.Release(this);
					child.Release(this);
					sibR.Release(this);
					sibL.Release(this);
					return ret;
				}
				/* DeletionOnAbsentKey */
				node.Release(this);
				return -3;
			}
		}
	}
}
int Database::BTreeVisit(wuint rootOffset, BTreeVisitor compare,
		BTreeVisitor visit, void *userdata, int degree) {
	DatabaseChunk chunk;
	int ret = LoadChunk(rootOffset, 4, chunk);
	if (ret < 0)
		return ret;
	wuint root = 0;
	GetChunkInt(chunk, rootOffset, &root);
	ReleaseChunk(chunk);
	return _BTreeVisit0(root, compare, visit, userdata, degree);
}

int Database::_BTreeMergeNodes(wuint rootOffset, int degree, BTNode *src,
		BTNode *keyProvider, int kIndex, BTNode *dst) {
	const int MAX_RECORDS = 2 * degree - 1;
	const int NODE_SIZE = (2 * MAX_RECORDS + 1) * 4;
	int ret;
	_BTreeNodeContentCopy(degree, src, 0, dst, dst->keyCount + 1,
			src->keyCount + 1);
	wuint midKey = 0;
	GetChunkInt(keyProvider->chunk, keyProvider->node, kIndex * 4, &midKey);
	PutChunkInt(dst->chunk, dst->node, dst->keyCount * 4, midKey);
	wuint keySucc = 0;
	if ((kIndex + 1) != MAX_RECORDS) {
		GetChunkInt(keyProvider->chunk, keyProvider->node, (kIndex + 1) * 4,
				&keySucc);
	}
	wuint r = 0;
	GetChunkInt(keyProvider->chunk, keyProvider->node,
			(MAX_RECORDS + kIndex + 1) * 4, &r);
	ret = Free(r, NODE_SIZE);
	_BTreeNodeContentDelete(degree, keyProvider, kIndex + 1, 1);
	PutChunkInt(keyProvider->chunk, keyProvider->node, kIndex * 4, keySucc);
	if (kIndex == 0 && keySucc == 0) {
		/*
		 * The root node is excused from the property that a node must have a least MIN keys
		 * This means we must special case it at the point when its had all of its keys deleted
		 * entirely during merge operations (which push one of its keys down as a pivot)
		 */
		DatabaseChunk chunk;
		wuint rootNode = 0;
		ret = LoadChunk(rootOffset, 4, chunk);
		if (ret > 0) {
			GetChunkInt(chunk, rootOffset, &rootNode);
			if (rootNode == keyProvider->node) {
				PutChunkInt(chunk, rootOffset, dst->node);
				Free(rootNode, NODE_SIZE);
			}
			ReleaseChunk(chunk);
		}
	}
	return ret;
}

int Database::_BTreeNodeContentCopy(int degree, BTNode *src, int srcPos,
		BTNode *dst, int dstPos, int length) {
	const int MAX_RECORDS = 2 * degree - 1;
	for (int i = length - 1; i >= 0; i--) { // this order is important when src == dst!
		int srcIndex = srcPos + i;
		int dstIndex = dstPos + i;

		if (srcIndex < src->keyCount + 1) {
			wuint srcChild = 0;
			GetChunkInt(src->chunk, src->node, (MAX_RECORDS + srcIndex) * 4,
					&srcChild);
			PutChunkInt(dst->chunk, dst->node, (MAX_RECORDS + dstIndex) * 4,
					srcChild);
			if (srcIndex < src->keyCount) {
				wuint srcKey = 0;
				GetChunkInt(src->chunk, src->node, srcIndex * 4, &srcKey);
				PutChunkInt(dst->chunk, dst->node, dstIndex * 4, srcKey);
			}
		}
	}
	return 1;
}

int Database::_BTreeNodeContentDelete(int degree, BTNode *node, int i,
		int length) {
	const int MAX_CHILDREN = 2 * degree;
	const int MAX_RECORDS = 2 * degree - 1;
	for (int index = i; index <= MAX_RECORDS; index++) {
		wuint newKey = 0;
		if ((index + length) < node->keyCount) {
			GetChunkInt(node->chunk, node->node, (index + length) * 4, &newKey);
		}
		wuint newChild = 0;
		if ((index + length) < (node->keyCount + 1)) {
			GetChunkInt(node->chunk, node->node,
					(MAX_RECORDS + index + length) * 4, &newChild);
		}
		if (index < MAX_RECORDS) {
			PutChunkInt(node->chunk, node->node, index * 4, newKey);
		}
		if (index < MAX_CHILDREN) {
			PutChunkInt(node->chunk, node->node, (MAX_RECORDS + index) * 4,
					newChild);
		}
	}
	return 1;
}

int Database::_BTreeVisit0(wuint node, BTreeVisitor compare, BTreeVisitor visit,
		void *userdata, int degree) {
	// If found is false, we are still in search mode.
	// Once found is true visit everything.
	// Return false when ready to quit.

	if (node == 0) {
		return true;
	}
	const int MAX_RECORDS = 2 * degree - 1;
	const int NODE_SIZE = (2 * MAX_RECORDS + 1) * 4;
	DatabaseChunk chunk;
	int ret = LoadChunk(node, 0, NODE_SIZE, chunk);
	if (ret < 0)
		return ret;

	// Binary search to find first record greater or equal.
	int lower = 0;
	int upper = MAX_RECORDS - 1;
	wuint r;
	while (lower < upper) {
		GetChunkInt(chunk, node, (upper - 1) * 4, &r);
		if (r != 0)
			break;
		upper--;
	}
	while (lower < upper) {
		int middle = (lower + upper) >> 1;
		wuint checkRec;
		GetChunkInt(chunk, node, middle * 4, &checkRec);
		if (checkRec == 0) {
			upper = middle;
		} else {
			int _compare = compare(this, checkRec, userdata);
			if (_compare >= 0) {
				upper = middle;
			} else {
				if (_compare < -1) {
					ReleaseChunk(chunk);
					return _compare;
				}
				lower = middle + 1;
			}
		}
	}

	// Start with first record greater or equal, reuse comparison results.
	int i = lower;
	for (; i < MAX_RECORDS; ++i) {
		wuint record;
		GetChunkInt(chunk, node, i * 4, &record);
		if (record == 0)
			break;

		int _compare = compare(this, record, userdata);
		if (_compare > 0) {
			// Start point is to the left.
			GetChunkInt(chunk, node, i * 4, &r);
			ReleaseChunk(chunk);
			return _BTreeVisit0(r, compare, visit, userdata, degree);
		} else if (_compare == 0) {
			GetChunkInt(chunk, node, (MAX_RECORDS + i) * 4, &r);
			int ret = _BTreeVisit0(r, compare, visit, userdata, degree);
			if (ret <= 0)
				return 0;
			ret = visit(this, record, userdata);
			if (ret <= 0)
				return 0;
		} else if (_compare < -1) {
			ReleaseChunk(chunk);
			return _compare;
		}
	}
	GetChunkInt(chunk, node, (MAX_RECORDS + i) * 4, &r);
	ReleaseChunk(chunk);
	return _BTreeVisit0(r, compare, visit, userdata, degree);
}
/* Red-Black Tree*/
class RBTreeStack {
public:
	enum {
		CHANGED = 1 << 0,
	};
	wuint record;
	wuint left;
	wuint right;
	int flags;
	DatabaseChunk chunk;
	inline void SetColor(int color) {
		if (GetColor() == color)
			return;
		if (color) {
			this->left |= 1 << 31;
		} else {
			this->left &= ~(1 << 31);
		}
	}
	inline int GetColor() {
		return this->left >> 31;
	}
	inline void SetLeft(wuint left) {
		if (GetLeft() != left) {
			this->left = (this->left & (1 << 31)) | (left & (~(1 << 31)));
		}
	}
	inline wuint GetLeft() {
		return this->left & ~(1 << 31);
	}
	inline void SetRight(wuint right) {
		if (GetRight() != right) {
			this->right = (this->right & (1 << 31)) | (right & (~(1 << 31)));
		}
	}
	inline wuint GetRight() {
		return this->right & ~(1 << 31);
	}
	int Load(Database *db, wuint record, wuint recordOffset) {
		int ret = db->LoadChunk(record, recordOffset, 8, chunk);
		if (ret < 0)
			return ret;
		this->record = record;
		this->flags = 0;
		db->GetChunkInt(chunk, record, recordOffset + 0, &left);
		db->GetChunkInt(chunk, record, recordOffset + 4, &right);
		return ret;
	}
	void Free(Database *db) {
		db->ReleaseChunk(chunk);
	}
	static void FreeAll(Database *db, RBTreeStack *stack, int size) {
		for (int i = 0; i < size; i++) {
			stack[i].Free(db);
		}
	}
};
enum {
	MASK_COLOR = 1 << 31, MASK_RECORD = ~MASK_COLOR,
};
int Database::RBTreeVisit(wuint rootOffset, wuint recordOffset,
		BTreeVisitor compare, BTreeVisitor visit, void *userdata) {
}
int Database::RBTreeInsert(wuint rootOffset, wuint &record, wuint recordOffset,
		RBTreeComparator cmp, void *userdata, int flags) {
	int ret = 0;
	wuint root = 0, _root;
	DatabaseChunk chunk;
	ret = LoadChunk(rootOffset, 4, chunk);
	if (ret < 0)
		return ret;
	GetChunkInt(chunk, rootOffset, &root);
	ReleaseChunk(chunk);
	wuint stack[32];
	int i = 0;
	_root = root;
	wuint x = root, y = 0;
	int _index, _compare;
	bool _fix_insert = false;
	while (true) {
		if ((x & MASK_RECORD) != 0) {
			if (i >= sizeof(stack) / sizeof(stack[0])) {
				return -4;
			}
			stack[i] = x;
			i++;
			_compare = cmp(this, x & MASK_RECORD, userdata);
			if (_compare < 0) {
				if (_compare < -1) {
					return _compare;
				}
				_index = 0;	// left
			} else if (_compare > 0) {
				_index = 4; // right
			} else {
				if ((flags & RBTREE_INSERT) != 0
						&& (flags & RBTREE_ALLOC) == 0) {
					_index = 4; // right
				} else {
					record = x;
					return 1;
				}
			}
		} else {
			if (flags & RBTREE_INSERT) {
				if (flags & RBTREE_ALLOC) {
					int size = record;
					ret = MallocZero(record, size);
				}
				if (ret < 0 || record == 0) {
					if (record == 0)
						ret = -3;
					return ret;
				}
				if ((y & MASK_RECORD) == 0) {
					ret = LoadChunk(rootOffset, 4, chunk);
					if (ret >= 0) {
						PutChunkInt(chunk, rootOffset, record);
						ReleaseChunk(chunk);
					}
				} else {
					_fix_insert = true;
					ret = LoadChunk(y & MASK_RECORD, recordOffset, 8, chunk);
					if (ret >= 0) {
						x = (x & MASK_COLOR) | (record & MASK_RECORD);
						PutChunkInt(chunk, y & MASK_RECORD,
								recordOffset + _index, x);
						ReleaseChunk(chunk);
						ret = LoadChunk(record & MASK_RECORD, recordOffset, 8,
								chunk);
						if (ret >= 0) {
							PutChunkInt(chunk, record & MASK_RECORD,
									recordOffset, MASK_COLOR); // set color the red
							ReleaseChunk(chunk);
						}
					}
				}
			} else {
				record = 0;
				return 0;
			}
			if (ret < 0)
				return ret;
			break;
		}
		y = x;
		ret = LoadChunk(x & MASK_RECORD, recordOffset, 8, chunk);
		if (ret < 0) {
			return ret;
		}
		GetChunkInt(chunk, x & MASK_RECORD, recordOffset + _index, &x);
		ReleaseChunk(chunk);
	}
	i--;
	if (_fix_insert) {
		/* fix insert */
		DatabaseChunk chunk_parent, chunk_grandparent;
		wuint k = record, parent = stack[i], grandparent = 0, uncle, _k0, _k1;
		i--;
		if (i >= 0) {
			grandparent = stack[i];
			i--;
		}
		while (true) {
			if ((k & MASK_RECORD) == (root & MASK_RECORD))
				break;
			ret = LoadChunk(parent & MASK_RECORD, recordOffset, 8,
					chunk_parent);
			if (ret < 0)
				break;
			GetChunkInt(chunk_parent, parent & MASK_RECORD, recordOffset, &_k0);
			//getColor(parent) == BLACK
			if ((_k0 & MASK_COLOR) == 0) {
				break;
			}
			ret = LoadChunk(grandparent & MASK_RECORD, recordOffset, 8,
					chunk_grandparent);
			if (ret < 0) {
				break;
			}
			//grandparent->left
			GetChunkInt(chunk_grandparent, grandparent & MASK_RECORD,
					recordOffset, &_k0);
			//grandparent->right
			GetChunkInt(chunk_grandparent, grandparent & MASK_RECORD,
					recordOffset + 4, &_k1);
			int _i0, _i1;
			if ((parent & MASK_RECORD) == (_k0 & MASK_RECORD)) {
				_i0 = 0;
				_i1 = 4;
				uncle = _k1;
			} else if ((parent & MASK_RECORD) == (_k1 & MASK_RECORD)) {
				_i0 = 4;
				_i1 = 0;
				uncle = _k0;
			} else {
				ret = -1;
				break;
			}
			_k0 = 0;
			if ((uncle & MASK_RECORD) != 0) {
				ret = LoadChunk(uncle & MASK_RECORD, recordOffset, 8, chunk);
				if (ret < 0) {
					break;
				}
				GetChunkInt(chunk, uncle & MASK_RECORD, recordOffset, &_k0);
				//getColor(uncle) == RED
				if ((_k0 & MASK_COLOR) != 0) {
					// setColor(uncle, BLACK);
					_k0 &= MASK_RECORD;
					PutChunkInt(chunk, uncle & MASK_RECORD, recordOffset, _k0);
					GetChunkInt(chunk_parent, parent & MASK_RECORD,
							recordOffset, &_k0);
					//setColor(parent, BLACK);
					_k0 &= MASK_RECORD;
					PutChunkInt(chunk_parent, parent & MASK_RECORD,
							recordOffset, _k0);
					GetChunkInt(chunk_grandparent, grandparent & MASK_RECORD,
							recordOffset, &_k0);
					//setColor(grandparent, RED);
					_k0 |= MASK_COLOR;
					PutChunkInt(chunk_grandparent, grandparent & MASK_RECORD,
							recordOffset, _k0);
					ReleaseChunk(chunk_parent);
					ReleaseChunk(chunk_grandparent);
					//k = grandparent;
					k = grandparent;
					parent = 0;
					if (i >= 0) {
						parent = stack[i];
						i--;
					}
					grandparent = 0;
					if (i >= 0) {
						grandparent = stack[i];
						i--;
					}
				}
				ReleaseChunk(chunk);
			}
			//getColor(uncle) == BLACK
			if ((_k0 & MASK_COLOR) == 0) {
				GetChunkInt(chunk_parent, parent & MASK_RECORD,
						recordOffset + _i1, &uncle);
				//if (k == parent->right) {
				if ((k & MASK_RECORD) == (uncle & MASK_RECORD)) {
					//rotateLeft(parent);
					//right_child = parent->right;
					// uncle = right_child
					ret = LoadChunk((uncle & MASK_RECORD), recordOffset, 8,
							chunk);
					if (ret < 0) {
						break;
					}
					//parent->right = right_child->left;
					GetChunkInt(chunk, (uncle & MASK_RECORD),
							recordOffset + _i0, &_k1);
					_k0 = (uncle & MASK_COLOR) | (_k1 & MASK_RECORD);
					PutChunkInt(chunk_parent, parent & MASK_RECORD,
							recordOffset + _i1, _k0);
					//right_child->left = parent;
					_k1 = (_k1 & MASK_COLOR) | (parent & MASK_RECORD);
					PutChunkInt(chunk, (uncle & MASK_RECORD),
							recordOffset + _i0, _k1);
					ReleaseChunk(chunk);
					/*
					 *     if (grandparent == 0)
					 *      	root = right_child;
					 *     else if (parent == grandparent->left)
					 *      	grandparent->left = right_child;
					 *     else
					 *      	grandparent->right = right_child;
					 */
					_k0 = (parent & MASK_COLOR) | (uncle & MASK_RECORD);
					PutChunkInt(chunk_grandparent, grandparent & MASK_RECORD,
							recordOffset + _i0, _k0);
					//u->node[_i2] = k_p;
					k = parent;
					parent = uncle;
					ReleaseChunk(chunk_parent);
					LoadChunk(parent & MASK_RECORD, recordOffset, 8,
							chunk_parent);
				}
				//rotateRight(grandparent);
				//left_child = grandparent->left;
				//uncle = left_child
				GetChunkInt(chunk_grandparent, grandparent & MASK_RECORD,
						recordOffset + _i0, &uncle);
				ret = LoadChunk((uncle & MASK_RECORD), recordOffset, 8, chunk);
				if (ret < 0) {
					break;
				}
				//grandparent->left = left_child->right;
				GetChunkInt(chunk, (uncle & MASK_RECORD), recordOffset + _i1,
						&_k1);
				_k0 = (uncle & MASK_COLOR) | (_k1 & MASK_RECORD);
				PutChunkInt(chunk_grandparent, grandparent & MASK_RECORD,
						recordOffset + _i0, _k0);
				//left_child->right = grandparent;
				_k1 = (_k1 & MASK_COLOR) | (grandparent & MASK_RECORD);
				PutChunkInt(chunk, uncle & MASK_RECORD, recordOffset + _i1,
						_k1);
				/*
				 *     if (grandparent->parent == 0)
				 *     		root = left_child;
				 *     else if (grandparent == grandparent->parent->left)
				 *     		grandparent->parent->left = left_child;
				 *     else
				 *     		grandparent->parent->right = left_child;
				 */
				wuint k_ppp = 0;
				if (i >= 0) {
					k_ppp = stack[i];
				}
				if (k_ppp == 0) {
					root = uncle;
				} else {
					ret = LoadChunk(k_ppp, recordOffset, 8, chunk);
					if (ret < 0)
						break;
					wuint t1;
					GetChunkInt(chunk, k_ppp & MASK_RECORD, recordOffset, &t1);
					if ((grandparent & MASK_RECORD) == (t1 & MASK_RECORD)) {
						t1 = (t1 & MASK_COLOR) | (uncle & MASK_RECORD);
						PutChunkInt(chunk, k_ppp & MASK_RECORD, recordOffset,
								t1);
					} else {
						GetChunkInt(chunk, k_ppp & MASK_RECORD,
								recordOffset + 4, &t1);
						if ((grandparent & MASK_RECORD) == (t1 & MASK_RECORD)) {
							t1 = (t1 & MASK_COLOR) | (uncle & MASK_RECORD);
							PutChunkInt(chunk, k_ppp & MASK_RECORD,
									recordOffset + 4, t1);
						} else {
							ret = -4;
							break;
						}
					}
					ReleaseChunk(chunk);
				}
				//swap(parent->color, grandparent->color);
				GetChunkInt(chunk_parent, parent, recordOffset, &_k0);
				GetChunkInt(chunk_grandparent, grandparent, recordOffset, &_k1);
				wuint t = _k0;
				_k0 = (_k1 & MASK_COLOR) | (_k0 & MASK_RECORD);
				_k1 = (t & MASK_COLOR) | (_k1 & MASK_RECORD);
				PutChunkInt(chunk_parent, parent, recordOffset, _k0);
				PutChunkInt(chunk_grandparent, grandparent, recordOffset, _k1);
				// k = parent;
				k = parent;
				parent = grandparent;
				grandparent = uncle;
			}
			ReleaseChunk(chunk_parent);
			ReleaseChunk(chunk_grandparent);
		}
		if (ret > 0) {
			if (root != _root) {
				ReleaseChunk(chunk);
				//root->color = 0;
				ret = LoadChunk(rootOffset, 4, chunk);
				if (ret > 0) {
					root &= MASK_RECORD;
					PutChunkInt(chunk, rootOffset, root);
				}
			}
			ReleaseChunk(chunk);
			ret = LoadChunk(root, recordOffset, 4, chunk);
			if (ret > 0) {
				wuint r;
				GetChunkInt(chunk, root, recordOffset, &r);
				if ((r & MASK_COLOR) != 0) {
					r &= ~MASK_COLOR;
					PutChunkInt(chunk, root, recordOffset, r);
				}
			}
		}
		ReleaseChunk(chunk);
		ReleaseChunk(chunk_parent);
		ReleaseChunk(chunk_grandparent);
	}
	if (ret < 0)
		return ret;
	return 0;
	/*while (1) {
	 if (k_p->color == 0)
	 break;
	 if (k_p == k_pp->node[_i2]) {

	 }
	 u = k_pp->node[_i1];
	 if (u->color == 1) {
	 u->color = 0;
	 k_p->color = 0;
	 k_pp->color = 1;
	 k = k_pp;
	 k_p = k->parent;
	 k_pp = k_p->parent;
	 } else {
	 if (k == k_p->node[_i1]) {
	 u = k_p->node[_i1];
	 k_p->node[_i1] = u->node[_i2];
	 if (u->node[_i2] != 0) {
	 u->node[_i2]->parent = k_p;
	 }
	 u->parent = k_p->parent;
	 k_pp->node[_i2] = u;
	 u->node[_i2] = k_p;
	 k_p->parent = u;
	 k = k_p;
	 k_p = u;
	 }
	 k_p->color = 0;
	 k_pp->color = 1;
	 //leftRotate(k->parent->parent);
	 u = k_pp->node[_i2];
	 k_pp->node[_i2] = u->node[_i1];
	 if (u->node[_i1] != TNULL) {
	 u->node[_i1]->parent = k_pp;
	 }
	 u->parent = k_pp->parent;
	 if (k_pp->parent == nullptr) {
	 this->root = u;
	 } else if (k_pp == k_pp->parent->node[_i1]) {
	 k_pp->parent->node[_i1] = u;
	 } else {
	 k_pp->parent->node[_i2] = u;
	 }
	 u->node[_i1] = k_pp;
	 k_pp->parent = u;
	 }
	 if (k == root) {
	 break;
	 }
	 }*/
}

int Database::RBTreeDelete(wuint rootOffset, wuint record, wuint recordOffset,
		BTreeComparator cmp, void *userdata) {
}

int Database::RBTreePrint(wuint rootOffset, wuint recordOffset) {
	int ret = 0;
	wuint root = 0;
	DatabaseChunk chunk;
	ret = LoadChunk(rootOffset, 4, chunk);
	if (ret < 0)
		return ret;
	GetChunkInt(chunk, rootOffset, &root);
	ReleaseChunk(chunk);
	char path[0x1000];
	return _RBTreePrint0(root, recordOffset, 0, path, true);
}

int Database::_RBTreePrint0(wuint block, wuint recordOffset, int depth,
		char *path, bool is_right) {
	if ((block & MASK_RECORD) == 0)
		return 0;
	const int spaces = 8;
	DatabaseChunk chunk;
	int ret = LoadChunk(block & MASK_RECORD, recordOffset, 8, chunk);
	if (ret < 0)
		return ret;
	wuint left = 0, right = 0;
	GetChunkInt(chunk, block & MASK_RECORD, recordOffset, &left);
	GetChunkInt(chunk, block & MASK_RECORD, recordOffset + 4, &right);
	ReleaseChunk(chunk);
	depth++;
	_RBTreePrint0(right, recordOffset, depth, path, true);
	path[depth - 2] = 0;

	if (is_right)
		path[depth - 2] = 1;

	if (left & MASK_RECORD)
		path[depth - 1] = 1;

	printf("\n");

	for (int i = 0; i < depth - 1; i++) {
		if (i == depth - 2)
			//printf("%c", is_right ? 218 : 192);
			printf("-");
		else if (path[i])
			//printf("%c", 179);
			printf("-");
		else
			printf(" ");

		for (int j = 1; j < spaces; j++)
			if (i < depth - 2)
				printf(" ");
			else
				//printf("%c", 196);
				printf("-");
	}
	const char *_color = (left & MASK_COLOR) != 0 ? "R" : "B";
	printf("%c%d\n", _color[0], block & MASK_RECORD);

	for (int i = 0; i < depth; i++) {
		if (path[i])
			//printf("%c", 179);
			printf("|");
		else
			printf(" ");

		for (int j = 1; j < spaces; j++)
			printf(" ");
	}
	_RBTreePrint0(left, recordOffset, depth, path, false);
	return ret;
}

int Database::RBTreeForAll(wuint rootOffset, wuint recordOffset,
		RBTreeComparator cmp, void *userdata) {
	int ret = 0;
	wuint root = 0;
	DatabaseChunk chunk;
	ret = LoadChunk(rootOffset, 4, chunk);
	if (ret < 0)
		return ret;
	GetChunkInt(chunk, rootOffset, &root);
	ReleaseChunk(chunk);
	int depth = 0;
	return _RBTreeForAll0(root, recordOffset, depth, cmp, userdata);
}

int Database::_RBTreeForAll0(wuint block, wuint recordOffset, int depth,
		RBTreeComparator cmp, void *userdata) {
	if ((block & MASK_RECORD) == 0)
		return 0;
	cmp(this, block & MASK_RECORD, userdata);
	if (depth > 32)
		return -4;
	DatabaseChunk chunk;
	int ret = LoadChunk(block & MASK_RECORD, recordOffset, 8, chunk);
	if (ret < 0)
		return ret;
	wuint left = 0, right = 0;
	GetChunkInt(chunk, block & MASK_RECORD, recordOffset, &left);
	GetChunkInt(chunk, block & MASK_RECORD, recordOffset + 4, &right);
	ReleaseChunk(chunk);
	_RBTreeForAll0(left, recordOffset, depth + 1, cmp, userdata);
	_RBTreeForAll0(right, recordOffset, depth + 1, cmp, userdata);
	return 1;
}
