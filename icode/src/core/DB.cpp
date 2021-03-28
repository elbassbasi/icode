/*
 * DB.cpp
 *
 *  Created on: 12 nov. 2019
 *      Author: azeddine
 */

#include "DB.h"
#define DATABASE_SIGNATURE "DB"
#define DATABASE_VERSION "1.1"

struct DataBaseHeader {
	char signature[4];
	char version[2];
	unsigned short blockSize;
	DBPage rm_list;
	DBPage end_pos;
	DBPage root;
};
struct DataBaseRmBlock {
	DBPage nbBlocks;
	DBPage next;
};
void DB::Init() {
	this->pageSize = 0;
	this->end_pos = 0;
	this->root = 0;
	this->file_pos = 0;
	this->file_size = 0;
	this->rm_list = 0;
}
DB::DB() {
	Init();
}
DB::~DB() {
}
int DB::OpenFile(const char *filename, const char *mode) {
	this->file.Open(filename, mode);
	if (this->file.IsOpened())
		return 1;
	else
		return -1;
}
int DB::OpenFile(const char *filename) {
	int ret = this->OpenFile(filename, "r+");
	if (ret > 0) {
		ret = CheckFile();
	}
	return ret;
}

int DB::OpenFileRO(const char *filename) {
	int ret = this->OpenFile(filename, "r");
	if (ret > 0) {
		ret = CheckFile();
	}
	return ret;
}

int DB::CreateFile(const char *filename, int blockSize) {
	int ret = this->OpenFile(filename, "w+");
	if (ret > 0) {
		ret = InitFile(blockSize);
	}
	return ret;
}

bool DB::IsOpened() {
	return this->file.IsOpened();
}

int DB::InitFile(int blockSize) {
	DataBaseHeader h;
	memset(&h, 0, sizeof(h));
	memcpy(h.signature, DATABASE_SIGNATURE, sizeof(DATABASE_SIGNATURE));
	h.rm_list = 0;
	if (blockSize < sizeof(DataBaseRmBlock))
		blockSize = sizeof(DataBaseRmBlock);
	int nb_blocks = sizeof(h) / blockSize;
	if (sizeof(h) % blockSize != 0)
		nb_blocks++;
	h.end_pos = nb_blocks;
	h.blockSize = blockSize;
	this->pageSize = blockSize;
	h.version[0] = 1;
	h.version[1] = 0;
	int ret = Write(&h, sizeof(h));
	if (ret >= 0) {
		this->end_pos = h.end_pos;
		this->rm_list = 0;
		this->file_size = ret;
		this->file_pos = ret;
		return 1;
	}
	return -1;
}

int DB::CheckFile() {
	DataBaseHeader h;
	int ret = Read(&h, sizeof(h));
	if (ret >= 0) {
		if (!strcmp(h.signature, DATABASE_SIGNATURE) && h.blockSize != 0) {
			this->pageSize = h.blockSize;
			this->end_pos = h.end_pos;
			this->rm_list = h.rm_list;
			this->root = h.root;
			fseek(this->file.file, 0, SEEK_END);
			this->file_size = ftell(this->file.file);
			this->file_pos = this->file_size;
			return 1;
		}
	}
	return -1;
}

int DB::NewPagesFromEnd(DBPage *block, int nbBlocks) {
	int ret;
	DBPage _end_pos = this->end_pos + nbBlocks;
	*block = this->end_pos;
	ret = SeekWrite(0, 12, &_end_pos, sizeof(DBPage));
	if (ret < 0)
		return ret;
	this->end_pos = _end_pos;
	return ret;
}

int DB::FindPagesFromRmList(DBPage *block, int nbBlocks) {
	struct DataBaseRmBlock rm_blocks, prev_rm_blocks;
	int stop = 0, ret;
	DBPage current_block = 0, prev_block = 0;
	size_t min_block_rest = sizeof(struct DataBaseRmBlock) / this->pageSize;
	if (sizeof(struct DataBaseRmBlock) % this->pageSize != 0)
		min_block_rest++;
	rm_blocks.next = this->rm_list;
	rm_blocks.nbBlocks = 0;
	while (stop == 0) {
		prev_block = current_block;
		current_block = rm_blocks.next;
		ret = SeekToPage(rm_blocks.next, 0);
		if (ret < 0)
			return ret;
		memcpy(&prev_rm_blocks, &rm_blocks, sizeof(rm_blocks));
		ret = this->Read(&rm_blocks, sizeof(rm_blocks));
		if (ret < 0)
			return ret;
		if ((rm_blocks.nbBlocks > (uint) nbBlocks
				&& (rm_blocks.nbBlocks - nbBlocks) >= min_block_rest)
				|| rm_blocks.nbBlocks == (uint) nbBlocks) {
			if (current_block == this->rm_list) {
				ret = SeekWrite(0, 8, &rm_blocks.next, sizeof(DBPage));
				if (ret < 0)
					return ret;
				this->rm_list = rm_blocks.next;
			} else {
				ret = SeekToPage(prev_block, 0);
				if (ret < 0)
					return ret;
				prev_rm_blocks.next = rm_blocks.next;
				ret = Write(&prev_rm_blocks, sizeof(prev_rm_blocks));
				if (ret < 0)
					return ret;
			}
			if (rm_blocks.nbBlocks != (uint) nbBlocks) {
				ret = DeletePages(current_block + nbBlocks,
						rm_blocks.nbBlocks - nbBlocks);
				if (ret < 0)
					return ret;
			}
			*block = current_block;
		}
		if (rm_blocks.next == 0) {
			stop = 1;
		}
	}
	return 1;
}

int DB::NewPages(DBPage *block, uint nbBlocks) {
	int ret;
	if (this->rm_list == 0) {
		return NewPagesFromEnd(block, nbBlocks);
	} else {
		ret = FindPagesFromRmList(block, nbBlocks);
		if (ret < 0)
			return ret;
		return NewPagesFromEnd(block, nbBlocks);
	}
}

int DB::DeletePages(DBPage blocks, uint nbBlocks) {
	int ret;
	struct DataBaseRmBlock rm_blocks;
	if ((nbBlocks * this->pageSize) < sizeof(struct DataBaseRmBlock))
		return -1;
	ret = SeekToPage(blocks, 0);
	if (ret < 0)
		return ret;
	rm_blocks.nbBlocks = nbBlocks;
	rm_blocks.next = this->rm_list;
	ret = this->Write(&rm_blocks, sizeof(rm_blocks));
	if (ret < 0)
		return ret;
	ret = SeekWrite(0, 8, &blocks, sizeof(DBPage));
	if (ret < 0)
		return ret;
	this->rm_list = blocks;
	return 1;
}
int DB::WriteEnd(unsigned int sz) {
	unsigned int i = 0, _w;
	int ret;
	char c[0x100];
	memset(c, 0, sizeof(c));
	while (i < sz) {
		_w = sz - i;
		if (_w > sizeof(c))
			_w = sizeof(c);
		ret = Write(c, _w);
		if (ret < 0)
			return ret;
		i += _w;
	}
	return 1;
}
int DB::SeekToPage(DBPage block, int pos) {
	wint64 newpos;
	newpos = (block * pageSize) + pos;
	if (newpos > this->file_size) {
		if (fseek(this->file.file, this->file_size, SEEK_SET) != 0)
			return -1;
		else {
			this->file_pos = this->file_size;
			return WriteEnd(newpos - this->file_size);
		}
	} else {
		if (fseek(this->file.file, newpos, SEEK_SET) != 0)
			return -1;
		this->file_pos = newpos;
		return 1;
	}
	return 1;
}

int DB::SeekRead(DBPage block, int pos, void *data, uint size) {
	int ret = SeekToPage(block, pos);
	if (ret < 0)
		return ret;
	ret = this->Read(data, size);
	return ret;
}

int DB::SeekWrite(DBPage block, int pos, const void *data, uint size) {
	int ret = SeekToPage(block, pos);
	if (ret < 0)
		return ret;
	ret = this->Write(data, size);
	return ret;
}

int DB::Read(void *data, uint size) {
	int ret = fread(data, 1, size, this->file.file);
	if (ret >= 0) {
		this->file_pos += ret;
		if (this->file_pos > this->file_size)
			this->file_size = this->file_pos;
	}
	if (ret < (int) size)
		return -1;

	return ret;
}

int DB::GetRoot(DBPage *root) {
	*root = this->root;
	return 1;
}

int DB::SetRoot(DBPage root) {
	int ret = this->SeekWrite(0, sizeof(DataBaseHeader) - sizeof(DBPage), &root,
			sizeof(DBPage));
	if (ret > 0) {
		this->root = root;
	}
	return ret;
}

int DB::Write(const void *data, uint size) {
	int ret = fwrite(data, 1, size, this->file.file);
	if (ret >= 0) {
		this->file_pos += ret;
		if (this->file_pos > this->file_size)
			this->file_size = this->file_pos;
	}
	if (ret < (int) size)
		return -1;
	return ret;
}
int DB::Alloc(DBPage *block, uint size, uint *size_allocated) {
	int nb_blocks = size / this->pageSize;
	if (size % this->pageSize != 0)
		nb_blocks++;
	if (size_allocated != 0) {
		*size_allocated = nb_blocks * this->pageSize;
	}
	int ret = NewPages(block, nb_blocks);
	return ret;
}

int DB::Free(DBPage blocks, uint size) {
	int nb_blocks = size / this->pageSize;
	if (size % this->pageSize != 0)
		nb_blocks++;
	return DeletePages(blocks, nb_blocks);
}


int DB::Realloc(DBPage *pages, uint newsize, uint lastsize,
		uint *size_allocated) {
	int ret;
	size_t nb_blocks_1 = newsize / this->pageSize;
	if (newsize % this->pageSize != 0)
		nb_blocks_1++;
	size_t nb_blocks_2 = lastsize / this->pageSize;
	if (lastsize % this->pageSize != 0)
		nb_blocks_2++;
	if (nb_blocks_2 >= nb_blocks_1) {
		size_t min_block_rest = sizeof(struct DataBaseRmBlock) / this->pageSize;
		if (nb_blocks_1 != nb_blocks_2) {
			if ((nb_blocks_2 - nb_blocks_1) <= min_block_rest) {
				if (size_allocated != 0) {
					*size_allocated = nb_blocks_2 * this->pageSize;
				}
				return 1;
			} else
				ret = DeletePages(*pages + nb_blocks_1,
						nb_blocks_2 - nb_blocks_1);
		}
	} else {
		ret = DeletePages(*pages, nb_blocks_2);
		if (ret < 0)
			return ret;
		ret = NewPages(pages, nb_blocks_1);
	}
	if (ret < 0)
		return ret;
	if (size_allocated != 0) {
		*size_allocated = nb_blocks_1 * this->pageSize;
	}
	return 1;
}

