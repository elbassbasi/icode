/*
 * MemStream.cpp
 *
 *  Created on: 3 nov. 2020
 *      Author: azeddine
 */

#include "../icode.h"

MemStream::MemStream() {
	this->flags = 0;
	this->memory = 0;
	this->allocated = 0;
	this->pos = 0;
	this->size = 0;
}
MemStream::~MemStream() {
}
IObject* MemStream::QueryInterface(IID Id) {
	switch (Id) {
	case IID_IStream:
		return static_cast<IStream*>(this);
		break;
	}
	return IObject::QueryInterface(Id);
}
bool MemStream::GetProperty(IID Id, WValue &value) {
	return false;
}

bool MemStream::SetProperty(IID Id, WValue &value) {
	return false;
}

WResult MemStream::Close() {
	if (memory != 0 && (flags & FREE_ON_CLOSE)) {
		free(memory);
	}
	return true;
}

WResult MemStream::Read(void *pv, size_t cb, size_t *pcbRead) {
	if (this->flags & READ) {
		if (this->pos < this->size) {
			size_t r = std::min(cb, this->size - this->pos);
			memcpy(pv, &this->memory[this->pos], r);
			this->pos += r;
			*pcbRead = r;
			return true;
		} else
			return -1;
	} else
		return false;
}

WResult MemStream::Write(const void *pv, size_t cb, size_t *pcbWritten) {
}

WResult MemStream::Seek(wuint64 dlibMove, int dwOrigin) {
}

WResult MemStream::Tell(wuint64 *pos) {
	*pos = this->pos;
	return true;
}

WResult MemStream::Rewind() {
	this->pos = 0;
	return true;
}

WResult MemStream::SetSize(wuint64 libNewSize) {
}

WResult MemStream::GetSize(wuint64 *libNewSize) {
	*libNewSize = this->allocated;
	return true;
}

WResult MemStream::Flush() {
	return true;
}

ObjectRef* MemStream::GetRef(int *tmp) {
	return &this->ref;
}
