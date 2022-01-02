/*
 * Document.h
 *
 *  Created on: 19 sept. 2021
 *      Author: azeddine
 */

#ifndef ICODE_SRC_TEXT_DOCUMENT_H_
#define ICODE_SRC_TEXT_DOCUMENT_H_
#include "../icode.h"

class DocumentLine: public AVLList {
public:
	enum {
		LINE_GROW = 32, //
	};
	wuint alloc;
	wuint utf8length;
	wuint utf32length;
	wuint utf32sum;
	char *text;
	IDocumentLineData *data[0];
	static void Recalculate(DocumentLine *node, const intptr_t diff);
public:
	bool Resize(int newsize);
	int GetOffset();
	DocumentLine* LineOfOffset(int offset);
	void SetLength(int length);
	void AddLength(int length);
};
struct DocumentDataID {
	StringId *id;
	IDocumentData *data;
};
class Document: public IDocument {
private:
	ObjectRef ref;
	DocumentLine *lines;
	wuint linesCount;
	wuint utf8length;
	wuint utf32length;
	WArray<StringId*> linesData;
	WArray<DocumentDataID> datas;
	WArray<IDocumentListener*> listeners;
public:
	Document();
	~Document();
	IObject* QueryInterface(const IID Id);
	ObjectRef* GetRef(int *tmp);
	bool GetProperty(IID Id, WValue &value);
	bool SetProperty(IID Id, WValue &value);
public:
	/* --------------- text access and manipulation --------------------------- */
	int GetLength();
	int Get(char *text, int length);
	int Get(IDocumentLine *line, int offset, int length, char *text,
			int text_length);
	int Set(const char *text, int length);
	int ReplaceLineText(DocumentLine *line, int utf8offset, int offset,
			int length, const char *text, int text_length);
	int Replace(int &line, int &offset, int length, const char *text,
			int text_length);

	wunichar GetChar(IDocumentLine *line, int offset, int utf8offset);
public:
	/* ---------------------- line information -------------------------------- */
	int GetLinesCount();
	IDocumentLine* GetLineOfOffset(int offset);
	int GetLineOffset(IDocumentLine *line);
	IDocumentLine* GetLine(int index);
	int GetLinesCount(IDocumentLine *line, int offset, int utf8offset,
			int length);
	int GetLineIndex(IDocumentLine *line);
	int GetLineLength(IDocumentLine *line);
	const char* GetLineUtf8Chars(IDocumentLine *line);
	int GetLineUtf8Length(IDocumentLine *line);
	IDocumentLine* NextLine(IDocumentLine *line);
	IDocumentLine* PrevLine(IDocumentLine *line);
	int AddLineDataID(StringId *id);
	IDocumentLineData* GetLineData(IDocumentLine *line, int id);
	bool SetLineData(IDocumentLine *line, int id, IDocumentLineData *data);
public:
	int AddDataID(StringId *id);
	IDocumentData* GetData(int id);
	bool SetData(int id, IDocumentData *data);
	void AddDocumentListener(IDocumentListener *listener);
	void RemoveDocumentListener(IDocumentListener *listener);
private:
	bool GrowLineData(DocumentLine *parent, int newLength);
	DocumentLine* InsertLine(int index);
	void DeleteLine(int index);
};
#endif /* ICODE_SRC_TEXT_DOCUMENT_H_ */
