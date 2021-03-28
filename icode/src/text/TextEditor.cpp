/*
 * TextEditor.cpp
 *
 *  Created on: 9 oct. 2019
 *      Author: azeddine
 */

#include "../icode.h"

TextEditor::TextEditor() {
	this->doc = 0;
	this->ref = 0;
}
bool TextEditor::Create(WComposite *parent) {
	bool ret = WCanvas::Create(parent, W_NONE);
	this->doc = IDocument::CreateDocument();
	if (this->doc == 0) {
		this->Dispose();
		return false;
	}
	return ret;
}
IObject* TextEditor::QueryInterface(const IID interfaceId) {
	return 0;
}

int TextEditor::IncRef() {
	IObject::IncRef(this->ref);
}

int TextEditor::DecRef() {
	IObject::DecRef(this->ref);
}

TextEditor::~TextEditor() {
}

void TextEditor::OnDispose(WEvent &e) {
}

bool TextEditor::OnKeyUp(WKeyEvent &e) {
	return false;
}

bool TextEditor::OnKeyDown(WKeyEvent &e) {
	return false;
}

bool TextEditor::OnMenuDetect(WMenuDetectEvent &e) {
	return false;
}
bool TextEditor::OnMouseMove(WMouseEvent &e) {
	return false;
}

bool TextEditor::OnPaint(WPaintEvent &e) {
	return false;
}

bool TextEditor::OnResize(WEvent &e) {
	return false;
}

bool TextEditor::OnTraverse(WKeyEvent &e) {
	return false;
}

