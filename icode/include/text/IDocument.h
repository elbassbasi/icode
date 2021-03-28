/*
 * IDocument.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_TEXT_IDOCUMENT_H_
#define ICODE_INCLUDE_TEXT_IDOCUMENT_H_
#include "../core/core.h"
class IDocumentLine;
class ICODE_PUBLIC IDocumentListener: public IObject {
public:

};
class ICODE_PUBLIC IDocument: public IObject {
public:
	static IDocument* CreateDocument();
public:
	virtual int RegistreLineIdData(size_t size) = 0;
	/*
	 * return internal memory reference for specific id and line
	 */
	virtual void* GetLineIdData(int id, IDocumentLine *line)=0;
	void* GetLineData(int id, IDocumentLine *line) {
		return *((void**) GetLineIdData(id, line));
	}
	void SetLineData(int id, IDocumentLine *line, void *data) {
		*((void**) GetLineIdData(id, line)) = data;
	}
	virtual void InsertText(const char *text, size_t size, int pos)=0;
	virtual int GetLineCount()=0;
	virtual const char* GetLine(int index, int *size = 0)=0;
	virtual IDocumentLine* GetLine(int index)=0;
	virtual IDocumentLine* GetNextLine(IDocumentLine *line)=0;
	virtual IDocumentLine* GetPrevLine(IDocumentLine *line)=0;
	virtual const char* GetLineChars(IDocumentLine *line)=0;
};
#endif /* ICODE_INCLUDE_TEXT_IDOCUMENT_H_ */
