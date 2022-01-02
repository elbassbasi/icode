/*
 * ZStream.cpp
 *
 *  Created on: 3 nov. 2020
 *      Author: azeddine
 */

#include "../icode.h"
ZStream::ZStream() {
	this->flags = 0;
	this->stream = 0;
}

ZStream::~ZStream() {
}
bool ZStream::OpenForWrite(IStream *stream, int level, int flags) {
	this->flags = flags;
	this->stream = stream;
	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	int ret = deflateInit(&strm, level);
	if (ret != Z_OK)
		return false;
	return true;
}
bool ZStream::OpenForRead(IStream *stream, int flags) {
	this->flags = flags;
	this->stream = stream;
	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	int ret = inflateInit(&strm);
	if (ret != Z_OK)
		return false;
	return true;
}
IObject* ZStream::QueryInterface(IID Id) {
	switch (Id) {
	case IID_IStream:
		return static_cast<IStream*>(this);
		break;
	}
	return IObject::QueryInterface(Id);
}
bool ZStream::GetProperty(IID Id, WValue &value) {
	return false;
}
bool ZStream::SetProperty(IID Id, WValue &value) {
	return false;
}
WResult ZStream::Close() {
	/* clean up and return */
	(void) deflateEnd(&strm);
	return true;
}

WResult ZStream::Read(void *pv, size_t cb, size_t *pcbRead) {
	WResult ret;
	strm.next_out = (Bytef*) pv;
	strm.avail_out = cb;
	/* run inflate() on input until output buffer not full */
	do {
		if (strm.avail_in == 0) {
			strm.next_in = this->in;
			size_t pcbRead = 0;
			ret = stream->Read(this->in, sizeof(this->in), &pcbRead);
			if (ret < 0) {
				inflateEnd(&strm);
				return ret;
			}
			strm.avail_in = pcbRead;
		}
		ret = inflate(&strm, Z_NO_FLUSH);
		switch (ret) {
		case Z_NEED_DICT:
			ret = Z_DATA_ERROR;
			inflateEnd(&strm);
			return ret;
			break;
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			inflateEnd(&strm);
			return ret;
		}
	} while (strm.avail_out == 0);
	return true;
}

WResult ZStream::Write(const void *pv, size_t cb, size_t *pcbWritten) {
	WResult ret;
	strm.next_in = (Bytef*) pv;
	strm.avail_in = cb;
	/* run deflate() on input until output buffer not full, finish
	 compression if all of source has been read in */
	do {
		strm.next_out = (Bytef*) this->in;
		strm.avail_out = sizeof(this->in);
		ret = deflate(&strm, Z_NO_FLUSH); /* no bad return value */
		int have = sizeof(this->in) - strm.avail_out;
		size_t pcbWritten;
		ret = stream->Write(strm.next_out, have, &pcbWritten);
		if (ret < 0) {
			deflateEnd(&strm);
			return ret;
		}
	} while (strm.avail_in == 0);
	return true;
}

WResult ZStream::Seek(wuint64 dlibMove, int dwOrigin) {
	return false;
}

WResult ZStream::Tell(wuint64 *pos) {
	return false;
}

WResult ZStream::Rewind() {
	return false;
}

WResult ZStream::SetSize(wuint64 libNewSize) {
	return false;
}

WResult ZStream::GetSize(wuint64 *libNewSize) {
	return false;
}

WResult ZStream::Flush() {
	return stream->Flush();
}

ObjectRef* ZStream::GetRef(int *tmp) {
	return &this->ref;
}
