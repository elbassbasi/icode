/*
 * TextEditor.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_TEXT_TEXTEDITOR_H_
#define ICODE_INCLUDE_TEXT_TEXTEDITOR_H_
#include "IDocument.h"
class ICODE_PUBLIC TextEditor: public WCanvas, public IDocumentListener {
private:
	int ref;
	IDocument *doc;
public:
	TextEditor();
	~TextEditor();
	bool Create(WComposite *parent);
	IObject* QueryInterface(const IID interfaceId);
	int IncRef();
	int DecRef();
protected:
	void OnDispose(WEvent &e);
	bool OnKeyUp(WKeyEvent &e);
	bool OnKeyDown(WKeyEvent &e);
	bool OnMenuDetect(WMenuDetectEvent &e);
	bool OnMouseMove(WMouseEvent &e);
	bool OnPaint(WPaintEvent &e);
	bool OnResize(WEvent &e);
	bool OnTraverse(WKeyEvent &e);
};
#endif /* ICODE_INCLUDE_TEXT_TEXTEDITOR_H_ */
