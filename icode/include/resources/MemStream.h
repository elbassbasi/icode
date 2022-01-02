/*
 * MemStream.h
 *
 *  Created on: 3 nov. 2020
 *      Author: azeddine
 */

#ifndef ICODE_RESOURCES_MEMSTREAM_H_
#define ICODE_RESOURCES_MEMSTREAM_H_
#include "IStream.h"
class MemStream: public IStream {
protected:
	ObjectRef ref;
	int flags;
	char *memory;
	size_t allocated;
	size_t size;
	size_t pos;
public:
	enum {
		READ = 1, WRITE = 2, FREE_ON_CLOSE = 4,
	};
	MemStream();
	~MemStream();
	IObject* QueryInterface(IID Id);
	ObjectRef* GetRef(int *tmp);
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

#endif /* ICODE_RESOURCES_MEMSTREAM_H_ */
