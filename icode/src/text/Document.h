/*
 * Document.h
 *
 *  Created on: 18 mars 2020
 *      Author: azeddine
 */

#ifndef SRC_ICODE_TEXT_DOCUMENT_H_
#define SRC_ICODE_TEXT_DOCUMENT_H_
#include "../icode.h"
class DocumentLine: public BasicLink {
public:
	friend class Document;
	ushort alloc;
	ushort length;
	ushort unicode_length;
	char *data;
	void *user_data[0];
};
class Document: public IDocument {
private:
	int ref;
	BasicList lines;
	int current_index;
	DocumentLine *current_line;
	int line_padding;
public:
	Document();
	~Document();
	IObject* QueryInterface(const IID interfaceId);
	int IncRef();
	int DecRef();
public:
	int RegistreLineIdData(size_t size);
	/*
	 * return internal memory reference for specific id and line
	 */
	void* GetLineIdData(int id, IDocumentLine *line);
	void InsertText(const char *text, size_t size, int pos);
	int GetLineCount();
	const char* GetLine(int index, int *size = 0);
	IDocumentLine* GetLine(int index);
	IDocumentLine* GetNextLine(IDocumentLine *line);
	IDocumentLine* GetPrevLine(IDocumentLine *line);
	const char* GetLineChars(IDocumentLine *line);
private:
	DocumentLine* AddLine(DocumentLine *prev, size_t size);
	DocumentLine* AddLine(int index, size_t size);
	DocumentLine* GetDocumentLine(int index);
};

#endif /* SRC_ICODE_TEXT_DOCUMENT_H_ */
