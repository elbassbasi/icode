/*
 * Document.cpp
 *
 *  Created on: 19 sept. 2021
 *      Author: azeddine
 */

#include "Document.h"

IDocument* IDocument::CreateDefaultDocument() {
	return new Document();
}

Document::Document() {
	this->lines = 0;
	this->linesCount = 0;
	this->utf32length = 0;
	this->utf8length = 0;
	InsertLine(0);
}

Document::~Document() {
}

IObject* Document::QueryInterface(const IID Id) {
	if (Id == IID_IDocument) {
		return static_cast<IDocument*>(this);
	}
	return IObject::QueryInterface(Id);
}
bool Document::GetProperty(IID Id, WValue &value) {
	return IObject::GetProperty(Id, value);
}

bool Document::SetProperty(IID Id, WValue &value) {
	return IObject::SetProperty(Id, value);
}
ObjectRef* Document::GetRef(int *tmp) {
	return &this->ref;
}
int Document::GetLength() {
	return this->utf32length;
}

int Document::Get(char *text, int length) {
	if (text == 0 || length <= 0) {
		return this->utf8length;
	} else {
		return Get(0, 0, this->utf8length, text, length);
	}
}

int Document::Get(IDocumentLine *line, int offset, int length, char *text,
		int text_length) {
}

int Document::Set(const char *text, int length) {
	int line = 0;
	int offset = 0;
	return Replace(line, offset, this->utf32length, text, length);
}
int Document::ReplaceLineText(DocumentLine *line, int utf8offset, int offset,
		int length, const char *text, int text_length) {
	int utf8_length = -1;
	int utf8_offset = utf8offset;
	if (offset > line->utf32length)
		return 0;
	if ((offset + length) >= line->utf32length) {
		length = line->utf32length - offset;
		utf8_length = line->utf8length - utf8_offset;
	}
	char *s = line->text;
	if (line->utf8length == line->utf32length) {
		if (utf8_length < 0)
			utf8_length = length;
	} else if (utf8_length < 0) {
		char *t = s, c;
		int _i = utf8_offset;
		int _length = 0;
		while (t != 0) {
			if (_i == _length) {
				utf8_length = (t - s) - utf8_offset;
				break;
			}
			_i++;
			c = t[0];
			t += w_utf8_skip_data[c];
		}
	}
	int new_size = line->utf8length + text_length - utf8_length;
	bool ret = line->Resize(new_size);
	if (!ret)
		return -1;
	s = line->text;
	if (text_length > utf8_length) {
		int l = line->utf8length - 1;
		int dep = text_length - utf8_length;
		int start = utf8_offset + utf8_length;
		while (l >= start) {
			s[l + dep] = s[l];
			l--;
		}
	}
	if (text_length < utf8_length) {
		int l = utf8_offset + text_length;
		int dep = utf8_length - text_length;
		int end = line->utf8length;
		while (l < end) {
			s[l] = s[l + dep];
			l++;
		}
	}
	int tLength = 0;
	if (text != 0) {
		char *t = &s[utf8_offset], c;
		int i = 0, j;
		while (i < text_length) {
			c = text[i];
			t[i] = c;
			int n = w_utf8_skip_data[c];
			int _end = n + i;
			for (j = i + 1; j < _end; j++) {
				t[j] = text[j];
			}
			tLength++;
			i += n;
		}
	}
	int _utf32length = tLength - length;
	if (_utf32length != 0) {
		int _utf8length = text_length - utf8_length;
		line->utf8length += _utf8length;
		line->AddLength(_utf32length);
	}
	return tLength;
}
int Document::Replace(int &line, int &offset, int length, const char *text,
		int text_length) {
	if (offset < 0)
		return 0;
	DocumentLine *_line;
	int lineIndex = line;
	_line =(DocumentLine *) this->lines->GetAt(lineIndex);
	if (_line == 0)
		return 0;
	int _utf8off;
	int _off = offset;
	int _len = length;
	if (_line->utf32length == _line->utf8length) {
		_utf8off = _off;
	} else {

	}
	if (text != 0 && text_length != 0) {
		const char *s = text, *prevs = s;
		const char *s_end = s;
		if (text_length >= 0) {
			s_end = text + text_length;
		}
		int _utf32len = 0;
		bool stop = false;
		while (1) {
			char c;
			if (text_length >= 0) {
				if (s >= s_end)
					stop = true;
				else {
					c = s[0];
				}
			} else {
				c = s[0];
				if (c == 0)
					stop = true;
			}
			if (stop) {
				wintptr l = s - prevs;
				int ret = ReplaceLineText(_line, _utf8off, _off, _len, prevs,
						l);
				if (ret >= 0) {
					line = lineIndex;
					offset = _off + ret;
					_off = 0;
					_len -= ret;
					_utf8off = 0;
					_off = 0;
				}
				break;
			}
			if (c == '\n' || c == '\r') {
				wintptr l = s - prevs;
				l = WMAX(l, 1);
				int ret = ReplaceLineText(_line, _utf8off, _off, _len, prevs,
						l);
				if (ret) {
					_off = 0;
					_len -= ret;
					_utf8off = 0;
					_off = 0;
					lineIndex++;
					if (_len <= 0) {
						_line = InsertLine(lineIndex);
					} else {
						_line =(DocumentLine *) _line->GetNext();
						if (_line == 0)
							_line = InsertLine(lineIndex);
					}
				}
				prevs = s + 1;
			}
			s++;
		}
	} else {
		int length_rest = length;
		while (length_rest > 0) {
		}

	}
}

wunichar Document::GetChar(IDocumentLine *line, int offset, int utf8offset) {
}

int Document::GetLinesCount() {
	return this->linesCount;
}

IDocumentLine* Document::GetLineOfOffset(int offset) {
	if (offset >= this->utf32length)
		return 0;
	return (IDocumentLine*) this->lines->LineOfOffset(offset);
}

int Document::GetLineOffset(IDocumentLine *line) {
	DocumentLine *l = (DocumentLine*) line;
	return l->GetOffset();
}

IDocumentLine* Document::GetLine(int index) {
	if ((wuint) index >= this->linesCount)
		return 0;
	return (IDocumentLine*) this->lines->GetAt(index);
}

int Document::GetLinesCount(IDocumentLine *line, int offset, int utf8offset,
		int length) {
}

int Document::GetLineIndex(IDocumentLine *line) {
	DocumentLine *l = (DocumentLine*) line;
	return l->GetIndex();
}

int Document::GetLineLength(IDocumentLine *line) {
	DocumentLine *l = (DocumentLine*) line;
	return l->utf32length;
}

const char* Document::GetLineUtf8Chars(IDocumentLine *line) {
	DocumentLine *l = (DocumentLine*) line;
	if (l->text == 0)
		return "";
	return l->text;
}

int Document::GetLineUtf8Length(IDocumentLine *line) {
	DocumentLine *l = (DocumentLine*) line;
	return l->utf8length;
}

IDocumentLine* Document::NextLine(IDocumentLine *line) {
	DocumentLine *l = (DocumentLine*) line;
	return (IDocumentLine*) l->GetNext();
}

IDocumentLine* Document::PrevLine(IDocumentLine *line) {
	DocumentLine *l = (DocumentLine*) line;
	return (IDocumentLine*) l->GetPrev();
}

int Document::AddLineDataID(StringId *id) {
	StringId **_ids;
	int count = linesData.GetCount(_ids);
	if (id != 0) {
		for (int i = 0; i < count; i++) {
			if (_ids[i] == id)
				return i + 1;
		}
	}
	bool ret = false;
	if (lines != 0) {
		DocumentLine *parent = (DocumentLine*) realloc(lines,
				sizeof(DocumentLine) + count * sizeof(void*));
		if (parent != 0) {
			parent->data[count - 1] = 0;
			this->lines = parent;
			ret = GrowLineData(lines, count);
		}
	} else
		ret = true;
	if (ret) {
		_ids = linesData.Add0(&count);
		*_ids = id;
		return count + 1;
	} else {
		return 0;
	}
}
IDocumentLineData* Document::GetLineData(IDocumentLine *line, int id) {
	if (id > 0 && id <= this->linesData.GetCount()) {
		DocumentLine *_line = (DocumentLine*) line;
		return _line->data[id - 1];
	}
	return 0;
}

bool Document::SetLineData(IDocumentLine *line, int id,
		IDocumentLineData *data) {
	if (id > 0 && id <= this->linesData.GetCount()) {
		DocumentLine *_line = (DocumentLine*) line;
		_line->data[id - 1] = data;
		return true;
	}
	return false;
}

int Document::AddDataID(StringId *id) {
	DocumentDataID *_ids;
	int count = datas.GetCount(_ids);
	if (id != 0) {
		for (int i = 0; i < count; i++) {
			if (_ids[i].id == id)
				return i + 1;
		}
	}
	_ids = datas.Add0(count, &count);
	if (_ids == 0)
		return 0;
	_ids->id = id;
	_ids->data = 0;
	return count + 1;
}

IDocumentData* Document::GetData(int id) {
	if (id > 0) {
		DocumentDataID *ptr = datas.Get0(id - 1);
		if (ptr != 0)
			return ptr->data;
		else
			return 0;
	} else
		return 0;
}

bool Document::SetData(int id, IDocumentData *data) {
	if (id > 0) {
		DocumentDataID *ptr = datas.Get0(id - 1);
		if (ptr != 0)
			return ptr->data = data;
		else
			return false;
	} else
		return false;
}

void Document::AddDocumentListener(IDocumentListener *listener) {
	IDocumentListener **arr;
	int count = listeners.GetCount(&arr);
	for (int i = 0; i < count; i++) {
		if (arr[i] == listener) {
			return;
		}
	}
	arr = listeners.Add0();
	if (arr != 0) {
		*arr = listener;
	}
}

void Document::RemoveDocumentListener(IDocumentListener *listener) {
	IDocumentListener **arr;
	int count = listeners.GetCount(&arr);
	for (int i = 0; i < count; i++) {
		if (arr[i] == listener) {
			listeners.Remove(i);
			return;
		}
	}
}
bool Document::GrowLineData(DocumentLine *parent, int newLength) {
	if (parent->left != 0) {
		DocumentLine *newline = (DocumentLine*) realloc(parent->left,
				sizeof(DocumentLine) + newLength * sizeof(void*));
		if (newline == 0)
			return false;
		newline->data[newLength - 1] = 0;
		parent->left = newline;
		bool ret = GrowLineData(parent->left->Cast<DocumentLine>(), newLength);
		if (!ret)
			return ret;
	}
	if (parent->right != 0) {
		DocumentLine *newline = (DocumentLine*) realloc(parent->right,
				sizeof(DocumentLine) + newLength * sizeof(void*));
		if (newline == 0)
			return false;
		newline->data[newLength - 1] = 0;
		parent->right = newline;
		bool ret = GrowLineData(parent->right->Cast<DocumentLine>(), newLength);
		if (!ret)
			return ret;
	}
	return true;
}
DocumentLine* Document::InsertLine(int index) {
	int count = linesData.GetCount();
	DocumentLine *line = (DocumentLine*) calloc(1,
			sizeof(DocumentLine) + count * sizeof(void*));
	if (line == 0)
		return 0;
	this->lines = this->lines->InsertAt(line, index)->Cast<DocumentLine>();
	this->linesCount++;
	IDocumentListener **arr;
	count = listeners.GetCount(&arr);
	if (count > 0) {
		DocumentEvent event;
		event.fDocument = this;
		event.fLine = index;
		event.fLineOffset = 0;
		event.fLength = 0;
		event.fText = 0;
		event.fTextLength = 0;
		for (int i = 0; i < count; i++) {
			arr[i]->OnLineInserted(event);
		}
	}
	return line;
}

void Document::DeleteLine(int index) {
	if (this->linesCount > 1) {
		IDocumentListener **arr;
		int count = listeners.GetCount(&arr);
		if (count > 0) {
			DocumentEvent event;
			event.fDocument = this;
			event.fLine = index;
			event.fLineOffset = 0;
			event.fLength = 0;
			event.fText = 0;
			event.fTextLength = 0;
			for (int i = 0; i < count; i++) {
				arr[i]->OnLineDeleted(event);
			}
		}
		DocumentLine *line;
		this->lines = (DocumentLine*) this->lines->RemoveAt(index,
				(AVLList**) &line);
		line->Resize(0);
		this->linesCount--;
		free(line);
	} else {
		this->lines->Resize(0);
	}
}
/* DocumentLine */
bool DocumentLine::Resize(int newsize) {
	if (newsize == 0) {
		if (this->text != 0)
			free(this->text);
		this->alloc = 0;
		this->text = 0;
		return true;
	} else {
		int _newsize = newsize + 1;
		size_t _sz = (_newsize / LINE_GROW) * LINE_GROW;
		if (_newsize % LINE_GROW != 0) {
			_sz += LINE_GROW;
		}
		if (this->alloc == _sz)
			return true;
		char *newText;
		if (this->text == 0) {
			newText = (char*) malloc(_sz);
		} else {
			newText = (char*) realloc(this->text, _sz);
		}
		if (newText == 0)
			return false;
		this->text = newText;
		this->alloc = _sz;
		return true;
	}
}

void DocumentLine::Recalculate(DocumentLine *node, const intptr_t diff) {
	int left_height, right_height;
	int size = 1;
	int sum = node->utf32length;
	DocumentLine *n =(DocumentLine *) node->left;
	if (n == 0) {
		left_height = 1;
	} else {
		left_height = n->height + 1;
		size += n->size;
		sum += n->utf32sum;
	}
	n =(DocumentLine *) node->right;
	if (n == 0) {
		right_height = 1;
	} else {
		right_height = n->height + 1;
		size += n->size;
		sum += n->utf32sum;
	}
	node->height = left_height > right_height ? left_height : right_height;
	node->size = size;
	node->utf32sum = sum;
}
void DocumentLine::AddLength(int length) {
	this->utf32length += length;
	DocumentLine *p = this;
	while (p) {
		p->utf32sum += length;
		p =(DocumentLine *) p->parent;
	}
}
void DocumentLine::SetLength(int length) {
	int diff = length - this->utf32length;
	AddLength(diff);
}
int DocumentLine::GetOffset() {
	DocumentLine *left = this->left->Cast<DocumentLine>();
	int offset = (left ? left->utf32sum : 0);
	DocumentLine *c = this;
	DocumentLine *p =(DocumentLine *) this->parent;
	while (p) {
		if (c == p->right) {
			offset += p->utf32sum - c->utf32sum;
		}
		c = p;
		p =(DocumentLine *) p->parent;
	}
	return offset;
}

DocumentLine* DocumentLine::LineOfOffset(int offset) {
	DocumentLine *n = this;
	while (1) {
		int leftsum;
		if (n->left == 0)
			leftsum = 0;
		else {
			DocumentLine *left = n->left->Cast<DocumentLine>();
			leftsum = left->utf32sum;
		}
		if (offset < leftsum) {
			n =(DocumentLine *) n->left;
		} else if (offset >= (leftsum + n->utf32length)) {
			offset -= leftsum + n->utf32length;
			n =(DocumentLine *) n->right;
		} else {
			return n;
		}
	}
	return 0;
}
void lines_print(DocumentLine *root, int tabs) {
	int i;
	if (root != 0) {
		lines_print(root->right->Cast<DocumentLine>(), tabs + 1);
		for (i = 0; i < tabs; i++)
			printf("\t");
		int p_index = -1;
		if (root->parent != 0) {
			p_index = root->parent->Cast<DocumentLine>()->alloc;
		}
		printf("%2i|%2i|%2i\n", root->alloc, root->utf32sum, p_index);
		lines_print(root->left->Cast<DocumentLine>(), tabs + 1);
	}
}
void lines_test() {
	Document *doc = new Document();
	DocumentLine *root = 0;
	DocumentLine *line;
	const int length = 16;
	for (int i = 0; i < length; i++) {
		line = new DocumentLine();
		memset(line, 0, sizeof(DocumentLine));
		line->alloc = i;
		root =(DocumentLine *) root->InsertAt(line, i);
		line->SetLength(i);
	}
	/*for (int i = 0; i < length; i++) {
	 line = root->Get(i);
	 line->SetLength(i);
	 }*/
	line =(DocumentLine *) root->GetAt(0);
	while (line != 0) {
		int index = line->GetIndex();
		printf("%d %d\n", index, line->alloc);
		line =(DocumentLine *) line->GetNext();
	}
	for (int i = 0; i < length; i++) {
		line =(DocumentLine *) root->GetAt(i);
		int index = line->GetIndex();
		printf("%d %d\n", index, line->GetOffset());
	}
	printf("Offset\n");
	for (int i = 0; i < length; i++) {
		int offset = i * 2;
		line = root->LineOfOffset(offset);
		int index = -1;
		if (line != 0)
			index = line->alloc;
		printf("%d %d\n", offset, index);
	}
	lines_print(root, 0);
}
