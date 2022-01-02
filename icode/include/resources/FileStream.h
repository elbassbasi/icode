/*
 * FileStream.h
 *
 *  Created on: 3 nov. 2020
 *      Author: azeddine
 */

#ifndef ICODE_RESOURCES_FILESTREAM_H_
#define ICODE_RESOURCES_FILESTREAM_H_
#include "IStream.h"
class FileStream: public IStream {
protected:
	ObjectRef ref;
	FILE* file;
public:
	FileStream();
	~FileStream();
	bool Open(const char* file,const char* mode);
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

#endif /* ICODESRC_RESOURCES_FILESTREAM_H_ */
