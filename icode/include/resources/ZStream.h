/*
 * ZStream.h
 *
 *  Created on: 3 nov. 2020
 *      Author: azeddine
 */

#ifndef ICODE_RESOURCES_ZSTREAM_H_
#define ICODE_RESOURCES_ZSTREAM_H_
#include "MemStream.h"
#include "FileStream.h"
#include "zlib.h"
class ZStream: public IStream {
protected:
	int ref;
	int flags;
	IStream *stream;
	z_stream strm;
    wuchar in[0x100];
public:
	ZStream();
	~ZStream();
	bool OpenForWrite(IStream *stream,int level, int flags);
	bool OpenForRead(IStream *stream,int flags);
	IObject* QueryInterface(IID Id);
	int IncRef();
	int DecRef();
	bool GetProperty(IID Id,WValue& value);
	bool SetProperty(IID Id,WValue& value);
	WResult Close();
	WResult Read(void *pv, size_t cb, size_t *pcbRead);
	WResult Write(const void *pv, size_t cb, size_t *pcbWritten);
	WResult Seek(wuint64 dlibMove, int dwOrigin);
	WResult Tell(wuint64 *pos);
	WResult Rewind();
	WResult Flush();
	WResult SetSize(wuint64 libNewSize);
	WResult GetSize(wuint64 *libNewSize);
};

class ZMemStream: public ZStream {
public:
	MemStream memstream;
};

class ZFileStream: public ZStream {
public:
	FileStream filestream;
};

#endif /* ICODE_RESOURCES_ZSTREAM_H_ */
