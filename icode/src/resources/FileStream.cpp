/*
 * FileStream.cpp
 *
 *  Created on: 3 nov. 2020
 *      Author: azeddine
 */

#include "../icode.h"
FileStream::FileStream() {
	this->file = 0;
	this->ref = 0;
}

FileStream::~FileStream() {
}

IObject* FileStream::QueryInterface(IID Id) {
	switch (Id) {
	case iid_IStream:
		return Cast<IStream>();
		break;
	}
	return IObject::QueryInterface(Id);
}

int FileStream::IncRef() {
	return IObject::IncRef(ref);
}

int FileStream::DecRef() {
	return IObject::DecRef(ref);
}

bool FileStream::GetProperty(IID Id,WValue& value) {
	return 0;
}

bool FileStream::SetProperty(IID Id,WValue& value) {
	return false;
}
bool FileStream::Open(const char *file, const char *mode) {
	if (this->file != 0) {
		fclose(this->file);
	}
	this->file = w_fopen(file, mode);
	return this->file != 0;
}
WResult FileStream::Close() {
	if (this->file != 0) {
		fclose(this->file);
		return true;
	} else
		return false;
}

WResult FileStream::Read(void *pv, size_t cb, size_t *pcbRead) {
	if (this->file != 0) {
		*pcbRead = fread(pv, 1, cb, this->file);
		return true;
	} else
		return false;
}

WResult FileStream::Write(const void *pv, size_t cb, size_t *pcbWritten) {
	if (this->file != 0) {
		*pcbWritten = fwrite(pv, 1, cb, this->file);
		return true;
	} else
		return false;
}

WResult FileStream::Seek(wuint64 dlibMove, int dwOrigin) {
	if (this->file != 0) {
		fseek(file, dlibMove, dwOrigin);
		return true;
	} else
		return false;
}

WResult FileStream::Tell(wuint64 *pos) {
	if (this->file != 0) {
		*pos = ftell(file);
		return true;
	} else
		return false;
}

WResult FileStream::Rewind() {
	if (this->file != 0) {
		rewind(file);
		return true;
	} else
		return false;
}

WResult FileStream::Flush() {
	if (this->file != 0) {
		fflush(file);
		return true;
	} else
		return false;
}
WResult FileStream::SetSize(wuint64 libNewSize) {
	return false;
}

WResult FileStream::GetSize(wuint64 *libNewSize) {
	if (this->file != 0) {
		return true;
	} else
		return false;
}
