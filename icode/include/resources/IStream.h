/*
 * IInputStream.h
 *
 *  Created on: 18 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_RESOURCES_IINPUTSTREAM_H_
#define ICODE_INCLUDE_RESOURCES_IINPUTSTREAM_H_
#include "../core/core.h"
class ICODE_PUBLIC IStream: public IObject {
public:
	IStream();
	~IStream();
	virtual WResult Close()=0;
	virtual WResult Read(void *pv, size_t cb, size_t *pcbRead)=0;
	virtual WResult Write(const void *pv, size_t cb, size_t *pcbWritten)=0;
	virtual WResult Seek(wuint64 dlibMove, int dwOrigin)=0;
	virtual WResult Tell(wuint64 *pos)=0;
	virtual WResult Rewind()=0;
	virtual WResult Flush()=0;
	virtual WResult SetSize(wuint64 libNewSize)=0;
	virtual WResult GetSize(wuint64 *libNewSize)=0;
protected:
	struct _w_stream_class *clazz;
public:
	size_t Read(void *pv, size_t cb) {
		size_t pcbRead = 0;
		this->Read(pv, cb, &pcbRead);
		return pcbRead;
	}
	size_t Write(const void *pv, size_t cb) {
		size_t pcbWritten = 0;
		this->Write(pv, cb, &pcbWritten);
		return pcbWritten;
	}
};
template<>
inline const IID __IID<IStream>() {
	return IID_IStream;
}

#endif /* ICODE_INCLUDE_RESOURCES_IINPUTSTREAM_H_ */
