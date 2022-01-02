/*
 * TextEditor.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_TEXT_TEXTEDITOR_H_
#define ICODE_INCLUDE_TEXT_TEXTEDITOR_H_
#include "IDocument.h"
class _TextEditorPriv;
class TextEditor;
class TextEditorLineInfoBase: public IDocumentLineData {
public:
	TextEditorLineInfoBase();
	~TextEditorLineInfoBase();
	ObjectRef* GetRef(int *tmp);
	int lineWidth;
	int lineHeight;
public:
	void RemoveLine(IDocumentLine *line);
};
class TextEditorLineInfo: public TextEditorLineInfoBase {
public:
	void *styles;
};
class ICODE_PUBLIC TextEditor: public WCanvas, public IDocumentListener {
private:
	ObjectRef ref;
	enum {
		TAB = '\t', BIDI_CARET_WIDTH = 3, CACHE_SIZE = 128,
	};
	struct PrivateData {
		IDocument *doc;
		WFont *regularFont;
		WFont *boldFont;
		WFont *italicFont;
		WFont *boldItalicFont;
		int selection_line;
		int selection_offset;
		int selection_length;
		int verticalScrollOffset;
		int horizontalScrollOffset;
		int topIndex;
		int topIndexY;
		int clientAreaHeight; // the client area height. Needed to calculate content width for new visible lines during Resize callback
		int clientAreaWidth; // the client area width. Needed during Resize callback to determine if line wrap needs to be recalculated
		int leftMargin;
		int topMargin;
		int rightMargin;
		int bottomMargin;
		w_color marginColor;
		int alignmentMargin;
		int averageCharWidth;
		int ascent;
		int descent;
		int tabLength;					// number of characters in a tab
		int tabWidth;
		int maxWidth;
		int maxWidthLineIndex;
		bool overwrite :1;
		bool fixedLineHeight :1;
		bool fixedPitch :1;
		bool ignoreResize :1;
		bool notalwaysShowScroll :1;
		bool wordWrap :1;
		int lineDataIndex;
		int caretLine;
		int caretOffset;
		int layouts_start;
	};
	WCaret defaultCaret;
	union {
		int data[0x50];
		PrivateData priv;
	};
	WTextLayout layouts[CACHE_SIZE];
public:
	TextEditor();
	~TextEditor();
	bool Create(WComposite *parent, wuint64 style);
	IObject* QueryInterface(IID interfaceId);
	ObjectRef* GetRef(int *tmp);
	void LineUpdated(const DocumentEvent &event, int op);
	void DocumentChanged(const DocumentEvent &event);
	void OnLineInserted(const DocumentEvent &event);
	/**
	 * Appends a string to the text at the end of the widget.
	 *
	 * @param string the string to be appended
	 * @see #replaceTextRange(int,int,String)
	 */
	void Append(const char *text, int length) {
		int lastChar = WMAX(GetCharCount(), 0);
		//ReplaceTextRange(lastChar, 0, text, length);
	}
	/**
	 * Appends a string to the text at the end of the widget.
	 *
	 * @param string the string to be appended
	 * @see #replaceTextRange(int,int,String)
	 */
	void Append(const char *text) {
		Append(text, -1);
	}
	/**
	 * Copies the selected text to the specified clipboard.  The text will be put in the
	 * clipboard in plain text format and RTF format.
	 * <p>
	 * The clipboardType is  one of the clipboard constants defined in class
	 * <code>DND</code>.  The <code>DND_CLIPBOARD</code>  clipboard is
	 * used for data that is transferred by keyboard accelerator (such as Ctrl+C/Ctrl+V)
	 * or by menu action.  The <code>DND_SELECTION_CLIPBOARD</code>
	 * clipboard is used for data that is transferred by selecting text and pasting
	 * with the middle mouse button.
	 * </p>
	 *
	 * @param clipboardType indicates the type of clipboard
	 *
	 */
	void Copy(int clipboardType) {
		CopySelection(clipboardType);
	}
	/**
	 * Copies the selected text to the <code>DND_CLIPBOARD</code> clipboard.
	 * <p>
	 * The text will be put on the clipboard in plain text format and RTF format.
	 * The <code>DND_CLIPBOARD</code> clipboard is used for data that is
	 * transferred by keyboard accelerator (such as Ctrl+C/Ctrl+V) or
	 * by menu action.
	 * </p>
	 */
	void Copy() {
		Copy(W_CLIPBOARD);
	}
	/**
	 * Returns the alignment of the widget.
	 *
	 * @return the alignment
	 *
	 * @see #getLineAlignment(int)
	 */
	int GetAlignment();
	/**
	 * Returns the Always Show Scrollbars flag.  True if the scrollbars are
	 * always shown even if they are not required.  False if the scrollbars are only
	 * visible when some part of the content needs to be scrolled to be seen.
	 * The H_SCROLL and V_SCROLL style bits are also required to enable scrollbars in the
	 * horizontal and vertical directions.
	 *
	 * @return the Always Show Scrollbars flag value
	 */
	bool GetAlwaysShowScrollBars();
	/**
	 * Returns the color of the margins.
	 *
	 * @return the color of the margins.
	 */
	WColor GetMarginColor();
	/**
	 * Moves the selected text to the clipboard.  The text will be put in the
	 * clipboard in plain text format and RTF format.
	 */
	void Cut();
	/**
	 * Returns the baseline, in pixels.
	 *
	 * Note: this API should not be used if a StyleRange attribute causes lines to
	 * have different heights (i.e. different fonts, rise, etc).
	 *
	 * @return baseline the baseline
	 *
	 * @see #getBaseline(int)
	 */
	int GetBaseline();
	/**
	 * Returns the baseline at the given offset, in pixels.
	 *
	 * @param offset the offset
	 *
	 * @return baseline the baseline
	 */
	int GetBaseline(int offset);
	/**
	 * Returns whether the widget is in block selection mode.
	 *
	 * @return true if widget is in block selection mode, false otherwise
	 */
	bool GetBlockSelection();
	/**
	 * Returns the block selection bounds. The bounds is
	 * relative to the upper left corner of the document.
	 *
	 * @return the block selection bounds
	 */
	bool GetBlockSelectionBounds(WRect &bounds);
	/**
	 * Returns the bottom margin.
	 *
	 * @return the bottom margin.
	 */
	int GetBottomMargin();
	/**
	 * Returns the caret position relative to the start of the text.
	 *
	 * @return the caret position relative to the start of the text.
	 */
	bool GetCaretOffset(int &line, int &offset);
	/**
	 * Returns whether the widget implements double click mouse behavior.
	 *
	 * @return true if double clicking a word selects the word, false if double clicks
	 * have the same effect as regular mouse clicks
	 */
	bool GetDoubleClickEnabled();
	/**
	 * Returns whether the widget content can be edited.
	 *
	 * @return true if content can be edited, false otherwise
	 */
	bool GetEditable() {
		return (GetStyle() & W_READ_ONLY) == 0;
	}
	/**
	 * Returns the horizontal scroll offset relative to the start of the line.
	 *
	 * @return horizontal scroll offset relative to the start of the line,
	 * measured in character increments starting at 0, if > 0 the content is scrolled
	 */
	int GetHorizontalIndex();
	/**
	 * Returns the horizontal scroll offset relative to the start of the line.
	 *
	 * @return the horizontal scroll offset relative to the start of the line,
	 * measured in pixel starting at 0, if > 0 the content is scrolled.
	 */
	int GetHorizontalPixel();
	/**
	 * Returns the line indentation of the widget.
	 *
	 * @return the line indentation
	 *
	 * @see #getLineIndent(int)
	 */
	int GetIndent();
	/**
	 * Returns whether the widget justifies lines.
	 *
	 * @return whether lines are justified
	 *
	 * @see #getLineJustify(int)
	 */
	bool GetJustify();
	/**
	 * Returns the action assigned to the key.
	 * Returns SWT.NULL if there is no action associated with the key.
	 *
	 * @param key a key code defined in const.h or a character.
	 * 	Optionally ORd with a state mask.  Preferred state masks are one or more of
	 *  W_MOD1, W_MOD2, W_MOD3, since these masks account for modifier platform
	 *  differences.  However, there may be cases where using the specific state masks
	 *  (i.e., W_CTRL, W_SHIFT, W_ALT, W_COMMAND) makes sense.
	 * @return one of the predefined actions defined in TextEditor or 0
	 * 	if there is no action associated with the key.
	 */
	int GetKeyBinding(int key);
	/**
	 * Gets the number of characters.
	 *
	 * @return number of characters in the widget
	 */
	int GetCharCount();
	/**
	 * Returns the line at the given line index without delimiters.
	 * Index 0 is the first line of the content. When there are not
	 * any lines, getLine(0) is a valid call that answers an empty string.
	 * <p>
	 *
	 * @param lineIndex index of the line to return.
	 * @return the line text without delimiters
	 */
	const char* GetLine(int lineIndex, int &length);
	/**
	 * Returns the alignment of the line at the given index.
	 *
	 * @param index the index of the line
	 *
	 * @return the line alignment
	 *
	 * @see #getAlignment()
	 */
	int GetLineAlignment(int index);
	/**
	 * Returns the line at the specified offset in the text
	 * where 0 &lt; offset &lt; getCharCount() so that getLineAtOffset(getCharCount())
	 * returns the line of the insert location.
	 *
	 * @param offset offset relative to the start of the content.
	 * 	0 <= offset <= getCharCount()
	 * @return line at the specified offset in the text
	 */
	int GetLineAtOffset(int offset);
	void SetText(const char *text, int length);
	void SetText(const char *text) {
		SetText(text, -1);
	}
	IDocument* GetDocument();
	IDocument* SetDocument(IDocument *document);
	/**
	 * Returns the line index for a y, relative to the client area.
	 * The line index returned is always in the range 0..lineCount - 1.
	 *
	 * @param y the y-coordinate pixel
	 *
	 * @return the line index for a given y-coordinate pixel
	 */
	int GetLineIndex(int y);
	/**
	 * Returns the top pixel, relative to the client area, of a given line.
	 * Clamps out of ranges index.
	 *
	 * @param lineIndex the line index, the max value is lineCount. If
	 * lineIndex == lineCount it returns the bottom pixel of the last line.
	 * It means this function can be used to retrieve the bottom pixel of any line.
	 *
	 * @return the top pixel of a given line index
	 */
	int GetLinePixel(int lineIndex);
	/**
	 * Returns whether the widget can have only one line.
	 *
	 * @return true if widget can have only one line, false if widget can have
	 * 	multiple lines
	 */
	bool IsSingleLine() {
		return (GetStyle() & W_SINGLE) != 0;
	}
protected:
	virtual void InitializeEditor();
	void OnDispose(WEvent &e);
	bool OnKeyUp(WKeyEvent &e);
	bool OnKeyDown(WKeyEvent &e);
	bool OnMenuDetect(WMenuDetectEvent &e);
	bool OnMouseMove(WMouseEvent &e);
	bool OnMouseDown(WMouseEvent &e);
	bool OnMouseUp(WMouseEvent &e);
	bool OnPaint(WPaintEvent &e);
	bool OnResize(WEvent &e);
	bool OnTraverse(WKeyEvent &e);
	bool OnHScroll(WScrollBarEvent &e);
	bool OnVScroll(WScrollBarEvent &e);
	bool OnImeComposition(WImeEvent &e);
	bool OnComputeSize(w_event_compute_size *e);
	virtual int OnDrawLine(int lineIndex, IDocumentLine *line, int paintX,
			int paintY, WGraphics &gc, w_color widgetBackground,
			w_color widgetForeground);
	virtual void OnLineStyled(int lineIndex, IDocumentLine *line,
			WTextLayout *layout);
	void OnLineDeleted(const DocumentEvent &event);
	void OnLineUpdated(const DocumentEvent &event);
private:
	void InitPrivate();
	void DoContent(int key, bool updateCaret);
	int GetVerticalScrollOffset();
	bool ScrollVertical(int pixels, bool adjustScrollBar);
	bool ScrollHorizontal(int pixels, bool adjustScrollBar);
	int GetLineHeight();
	int GetLineHeight(IDocumentLine *line);
	void CalculateScrollBars();
	void CalculateTopIndex(int delta);
	bool IsFixedLineHeight();
	void SetScrollBar(WScrollBar &bar, int clientArea, int maximum, int margin);
	void SetScrollBars(bool vertical);
	void SetFont0(WFont *font, int tabs);
	WFont* GetFont0(int style);
	int GetHeight();
	int GetWidth();
	void RedrawMargins(int oldHeight, int oldWidth);
	void UpdateCaretVisibility();
	/**
	 * Scrolls text to the right to use new space made available by a resize.
	 */
	void ClaimRightFreeSpace();
	void CreateCaretBitmaps();
	bool CopySelection(int type);
	/*
	 *
	 */
	WTextLayout* GetTextLayout(WTextLayout &tmp, int lineIndex);
	void ClearTextLayout(int lineIndex);
};
#endif /* ICODE_INCLUDE_TEXT_TEXTEDITOR_H_ */
