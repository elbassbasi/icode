/*
 * IStream.cpp
 *
 *  Created on: 3 nov. 2020
 *      Author: azeddine
 */

#include "../icode.h"
wresult IStream_close(w_stream *stream) {
	return ((IStream*)stream)->Close();
}
wresult IStream_read(w_stream *stream, void *pv, size_t cb, size_t *pcbRead) {
	return ((IStream*)stream)->Read(pv,cb,pcbRead);
}
wresult IStream_write(w_stream *stream, const void *pv, size_t cb,
		size_t *pcbWritten) {
	return ((IStream*)stream)->Write(pv,cb,pcbWritten);
}
wresult IStream_seek(w_stream *stream, wuint64 dlibMove, int dwOrigin) {
	return ((IStream*)stream)->Seek(dlibMove,dwOrigin);
}
wresult IStream_tell(w_stream *stream, wuint64 *pos) {
	return ((IStream*)stream)->Tell(pos);
}
wresult IStream_rewind(w_stream *stream) {
	return ((IStream*)stream)->Rewind();
}
wresult IStream_set_size(w_stream *stream, wuint64 libNewSize) {
	return ((IStream*)stream)->SetSize(libNewSize);
}
wresult IStream_get_size(w_stream *stream, wuint64 *libNewSize) {
	return ((IStream*)stream)->GetSize(libNewSize);
}
struct _w_stream_class IStream_clazz = {//
		IStream_close,//
		IStream_read,//
		IStream_write,//
		IStream_seek,//
		IStream_tell,//
		IStream_rewind,//
		IStream_set_size,//
		IStream_get_size,//
};
IStream::IStream() {
	this->clazz = &IStream_clazz;
}
IStream::~IStream() {
	this->clazz->close((w_stream*)this);
}
