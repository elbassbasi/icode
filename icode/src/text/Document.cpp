/*
 * Document.cpp
 *
 *  Created on: 18 mars 2020
 *      Author: azeddine
 */

#include "Document.h"
IDocument* IDocument::CreateDocument() {
	return new Document();
}

Document::Document() {
	this->ref = 0;
	this->current_index = 0;
	this->current_line = 0;
	this->line_padding = 0;
}
Document::~Document() {
}
IObject* Document::QueryInterface(const IID interfaceId) {
	return 0;
}
int Document::IncRef() {
	return IObject::IncRef(this->ref);
}
int Document::DecRef() {
	return IObject::DecRef(this->ref);
}
DocumentLine* Document::AddLine(int index, size_t size) {
	return AddLine(GetDocumentLine(index), size);
}
DocumentLine* Document::AddLine(DocumentLine *prev, size_t size) {
	DocumentLine *line = (DocumentLine*) malloc(sizeof(DocumentLine) + size);
	if (line == 0)
		return 0;
	this->lines.LinkAfter(line, prev);
	line->alloc = size;
	line->length = 0;
	this->current_index = 0;
	this->current_line = 0;
	return line;
}

IDocumentLine* Document::GetLine(int index) {
	return (IDocumentLine*) GetDocumentLine(index);
}

IDocumentLine* Document::GetNextLine(IDocumentLine *line) {
	return (IDocumentLine*) ((DocumentLine*) line)->next;
}

IDocumentLine* Document::GetPrevLine(IDocumentLine *line) {
	return (IDocumentLine*) ((DocumentLine*) line)->prev;
}

const char* Document::GetLineChars(IDocumentLine *line) {
	return ((DocumentLine*) line)->data;
}

int Document::RegistreLineIdData(size_t size) {
}

void* Document::GetLineIdData(int id, IDocumentLine *line) {
}

DocumentLine* Document::GetDocumentLine(int index) {
	DocumentLine *line;
	int dCurrent, dLast, dFirst, d;
	dCurrent = index - this->current_index;
	dFirst = index;
	dLast = index - this->lines.size;
	if (abs(dCurrent) < abs(dFirst)) {
		if (abs(dCurrent) < abs(dLast)) {
			d = dCurrent;
			line = this->current_line;
		} else {
			d = dLast;
			line = (DocumentLine*) this->lines.last;
		}
	} else {
		if (abs(dFirst) < abs(dLast)) {
			d = dFirst;
			line = (DocumentLine*) this->lines.first;
		} else {
			d = dLast;
			line = (DocumentLine*) this->lines.last;
		}
	}

	if (d >= 0) {
		for (int i = 0; i < d; i++) {
			if (line == 0)
				return 0;
			line = (DocumentLine*) line->next;
		}
		if (line != 0) {
			this->current_index = index;
			this->current_line = line;
		}
		return line;
	} else {
		for (int i = d; i < 0; i--) {
			if (line == 0)
				return 0;
			line = (DocumentLine*) line->prev;
		}
		if (line != 0) {
			this->current_index = index;
			this->current_line = line;
		}
		return line;
	}
}

void Document::InsertText(const char *text, size_t size, int pos) {
	if (size == 0)
		return;
	if (size < 0)
		size = strlen(text);

}
int Document::GetLineCount() {
	return this->lines.size;
}
const char* Document::GetLine(int index, int *size) {
	DocumentLine *line = GetDocumentLine(index);
	if (line == 0) {
		if (size != 0) {
			size = 0;
		}
		return 0;
	}
	if (size != 0) {
		*size = line->length;
	}
	return line->data;
}
