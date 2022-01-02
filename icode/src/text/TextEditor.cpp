/*
 * TextEditor.cpp
 *
 *  Created on: 9 oct. 2019
 *      Author: azeddine
 */

#include "../icode.h"
#include <new>
#include <cmath>
TextEditor::TextEditor() {
	InitPrivate();
}
TextEditor::~TextEditor() {
	if (priv.boldFont != 0) {
		priv.boldFont->Dispose();
		priv.boldFont = 0;
	}
	if (priv.italicFont != 0) {
		priv.italicFont->Dispose();
		priv.italicFont = 0;
	}
	if (priv.boldItalicFont != 0) {
		priv.boldItalicFont->Dispose();
		priv.boldItalicFont = 0;
	}
}
void TextEditor::InitPrivate() {
	memset(&this->priv, 0, sizeof(this->priv));
}
bool TextEditor::Create(WComposite *parent, wuint64 style) {
	bool ret = WCanvas::Create(parent, style);
	if (ret) {
		InitializeEditor();
		if (priv.doc == 0) {
			SetDocument(0);
		}
	}
	return ret;
}
IObject* TextEditor::QueryInterface(IID interfaceId) {
	return IObject::QueryInterface(interfaceId);
}
IDocument* TextEditor::GetDocument() {
	return priv.doc;
}
IDocument* TextEditor::SetDocument(IDocument *document) {
	IDocument *last = priv.doc;
	if (document == 0)
		document = IDocument::CreateDefaultDocument();
	priv.doc = document;
	priv.lineDataIndex = document->AddLineDataID("TextEditorLine");
	document->AddDocumentListener(this);
	document->AddRef();
	if (last != 0)
		last->Release();
	return last;
}
void TextEditor::OnDispose(WEvent &e) {
	this->defaultCaret.Close();
}

bool TextEditor::OnKeyUp(WKeyEvent &e) {
	return false;
}

bool TextEditor::OnKeyDown(WKeyEvent &e) {
	int action = 0;
	/*caretAlignment = PREVIOUS_OFFSET_TRAILING;
	 if (event.keyCode != 0) {
	 // special key pressed (e.g., F1)
	 action = getKeyBinding(event.keyCode | event.stateMask);
	 } else {
	 // character key pressed
	 action = getKeyBinding(event.character | event.stateMask);
	 if (action == SWT.NULL) {
	 // see if we have a control character
	 if ((event.stateMask & SWT.CTRL) != 0 && event.character <= 31) {
	 // get the character from the CTRL+char sequence, the control
	 // key subtracts 64 from the value of the key that it modifies
	 int c = event.character + 64;
	 action = getKeyBinding(c | event.stateMask);
	 }
	 }
	 }*/
	if (action == 0) {
		int ignore = false;
#if __APPLE__
			// Ignore accelerator key combinations (we do not want to
			// insert a character in the text in this instance).
			ignore = (e.detail & (W_COMMAND | W_CTRL)) != 0;
#else
		// Ignore accelerator key combinations (we do not want to
		// insert a character in the text in this instance). Don't
		// ignore CTRL+ALT combinations since that is the Alt Gr
		// key on some keyboards.  See bug 20953.
		ignore = (e.detail ^ W_ALT) == 0 || (e.detail ^ W_CTRL) == 0
				|| (e.detail ^ (W_ALT | W_SHIFT)) == 0
				|| (e.detail ^ (W_CTRL | W_SHIFT)) == 0;
#endif
		// -ignore anything below SPACE except for line delimiter keys and tab.
		// -ignore DEL
		if ((!ignore && e.character > 31 && e.character != W_DEL)
				|| e.character == W_CR || e.character == W_LF
				|| e.character == TAB) {
			DoContent(e.character, true);
			//this->Update();
			this->Redraw();
		}
	} else {
		//invokeAction(action);
	}
	return false;
}

bool TextEditor::OnMenuDetect(WMenuDetectEvent &e) {
	return false;
}
bool TextEditor::OnMouseMove(WMouseEvent &e) {
	return false;
}
bool TextEditor::OnMouseDown(WMouseEvent &e) {
	this->ForceFocus();
	return false;
}

bool TextEditor::OnMouseUp(WMouseEvent &e) {
	return false;
}
int TextEditor::OnDrawLine(int lineIndex, IDocumentLine *line, int paintX,
		int paintY, WGraphics &gc, w_color widgetBackground,
		w_color widgetForeground) {
	int line_length = priv.doc->GetLineUtf8Length(line);
	const char *s = priv.doc->GetLineUtf8Chars(line);
	TextEditorLineInfoBase *info =
			(TextEditorLineInfoBase*) priv.doc->GetLineData(line,
					priv.lineDataIndex);
	if (info != 0) {
		WTextLayout tmp, *ret;
		ret = GetTextLayout(tmp, lineIndex);
		ret->Draw(gc, paintX, paintY);
	} else {
		gc.DrawText(s, line_length, paintX, paintY, W_DRAW_TRANSPARENT);
	}
	return GetLineHeight();
}
bool TextEditor::OnPaint(WPaintEvent &e) {
	if (e.bounds.width == 0 || e.bounds.height == 0)
		return false;
	if (priv.clientAreaWidth == 0 || priv.clientAreaHeight == 0)
		return false;

	int startLine = GetLineIndex(e.bounds.y);
	int y = GetLinePixel(startLine);
	int endY = e.bounds.y + e.bounds.height;
	w_color background = GetBackground();
	w_color foreground = GetForeground();
	if (endY > 0) {
		int lineCount = IsSingleLine() ? 1 : priv.doc->GetLinesCount();
		int x = priv.leftMargin - priv.horizontalScrollOffset;
		IDocumentLine *line = priv.doc->GetLine(startLine);
		int paintX = x;
		int paintY = y;
		int lineIndex = startLine;
		while (line != 0 && y < endY) {
			paintY = y;
			y += OnDrawLine(lineIndex, line, paintX, paintY, *e.gc, background,
					foreground);
			lineIndex++;
			line = priv.doc->NextLine(line);
		}
		if (y < endY) {
			e->SetBackground(background);
			//DrawBackground(gc, 0, y, clientAreaWidth, endY - y);
		}
	}
	/*if (blockSelection && blockXLocation != -1) {
	 gc.setBackground(getSelectionBackground());
	 Rectangle rect = getBlockSelectionRectangle();
	 gc.drawRectangle(rect.x, rect.y, Math.max(1, rect.width - 1), Math.max(1, rect.height - 1));
	 gc.setAdvanced(true);
	 if (gc.getAdvanced()) {
	 gc.setAlpha(100);
	 gc.fillRectangle(rect);
	 gc.setAdvanced(false);
	 }
	 }*/

	// fill the margin background
	e->SetBackground(priv.marginColor != 0 ? priv.marginColor : background);
	/*if (topMargin > 0) {
	 DrawBackground(gc, 0, 0, clientAreaWidth, topMargin);
	 }
	 if (bottomMargin > 0) {
	 DrawBackground(gc, 0, clientAreaHeight - bottomMargin,
	 clientAreaWidth, bottomMargin);
	 }
	 if (leftMargin - alignmentMargin > 0) {
	 DrawBackground(gc, 0, 0, leftMargin - alignmentMargin,
	 clientAreaHeight);
	 }
	 if (rightMargin > 0) {
	 DrawBackground(gc, clientAreaWidth - rightMargin, 0,
	 rightMargin, clientAreaHeight);
	 }*/
	return true;
}

bool TextEditor::OnResize(WEvent &e) {
	int oldHeight = priv.clientAreaHeight;
	int oldWidth = priv.clientAreaWidth;
	WRect clientArea;
	GetClientArea(clientArea);
	priv.clientAreaHeight = clientArea.height;
	priv.clientAreaWidth = clientArea.width;
	if (priv.notalwaysShowScroll && priv.ignoreResize)
		return false;

	RedrawMargins(oldHeight, oldWidth);
	if (priv.wordWrap) {
		if (oldWidth != priv.clientAreaWidth) {
			//renderer.reset(0, content.getLineCount());
			priv.verticalScrollOffset = -1;
			//renderer.calculateIdle();
			WCanvas::Redraw();
		}
		if (oldHeight != priv.clientAreaHeight) {
			if (oldHeight == 0)
				priv.topIndexY = 0;
			SetScrollBars(true);
		}
		//SetCaretLocation();
	} else {
		//renderer.calculateClientArea();
		SetScrollBars(true);
		ClaimRightFreeSpace();
		// StyledText allows any value for horizontalScrollOffset when clientArea is zero
		// in setHorizontalPixel() and setHorisontalOffset(). Fixes bug 168429.
		if (priv.clientAreaWidth != 0) {
			WScrollBar horizontalBar;
			GetHorizontalBar(horizontalBar);
			if (horizontalBar.IsOk() && horizontalBar.GetVisible()) {
				if (priv.horizontalScrollOffset
						!= horizontalBar.GetSelection()) {
					horizontalBar.SetSelection(priv.horizontalScrollOffset);
					priv.horizontalScrollOffset = horizontalBar.GetSelection();
				}
			}
		}
	}
	UpdateCaretVisibility();
	/* claimBottomFreeSpace();
	 setAlignment();*/
	//TODO FIX TOP INDEX DURING RESIZE
//	if (oldHeight != clientAreaHeight || wordWrap) {
//		calculateTopIndex(0);
//	}
	return false;
}

bool TextEditor::OnTraverse(WKeyEvent &e) {
	return false;
}

void TextEditor::SetText(const char *text, int length) {
}

bool TextEditor::OnHScroll(WScrollBarEvent &e) {
	int selection = e.scrollbar->GetSelection();
	int scrollPixel = selection - priv.horizontalScrollOffset;
	ScrollHorizontal(scrollPixel, false);
	return false;
}

bool TextEditor::OnVScroll(WScrollBarEvent &e) {
	int selection = e.scrollbar->GetSelection();
	int scrollPixel = selection - GetVerticalScrollOffset();
	ScrollVertical(scrollPixel, false);
	return false;
}
bool TextEditor::OnImeComposition(WImeEvent &e) {
	return WCanvas::OnImeComposition(e);
}
bool TextEditor::OnComputeSize(w_event_compute_size *e) {
	return WCanvas::OnComputeSize(e);
}
void TextEditor::InitializeEditor() {
	WFont *font = GetFont();
	SetFont0(font, priv.tabLength);
	defaultCaret.Create(this);
	CreateCaretBitmaps();
	defaultCaret.SetLocation(0, 0);
	SetCaret(&defaultCaret);
}

void TextEditor::LineUpdated(const DocumentEvent &event, int op) {
}

void TextEditor::DocumentChanged(const DocumentEvent &event) {
}

void TextEditor::DoContent(int key, bool updateCaret) {
	char text_tmp[10];
	const char *text;
	int text_length;
	/*if (blockSelection && blockXLocation != -1) {
	 insertBlockSelectionText(key, SWT.NULL);
	 return;
	 }

	 Event event = new Event();
	 event.start = selection.x;
	 event.end = selection.y;*/
	// replace a CR line break with the widget line break
	// CR does not make sense on Windows since most (all?) applications
	// don't recognize CR as a line break.
	if (key == W_CR || key == W_LF) {
		if ((this->GetStyle() & W_SINGLE) == 0) {
#if defined (__WIN32__)
			text = "\r\n";
			text_length = 2;
#else
			text = "\n";
			text_length = 1;
#endif

		} else {
			text = 0;
			text_length = 0;
		}
	} else if (priv.selection_length == 0 && priv.overwrite && key != TAB) {
		// no selection and overwrite mode is on and the typed key is not a
		// tab character (tabs are always inserted without overwriting)?
		/*int lineIndex = content.getLineAtOffset(event.end);
		 int lineOffset = content.getOffsetAtLine(lineIndex);
		 String line = content.getLine(lineIndex);
		 // replace character at caret offset if the caret is not at the
		 // end of the line
		 if (event.end < lineOffset + line.length()) {
		 event.end++;
		 }*/

		text_length = w_utf8_from_ucs4((wunichar*) &key, 1, text_tmp,
				sizeof(text_tmp));
		text = text_tmp;
	} else {
		text_length = w_utf8_from_ucs4((wunichar*) &key, 1, text_tmp,
				sizeof(text_tmp));
		text = text_tmp;
	}
	if (text != 0) {
		if (updateCaret) {
			//Fix advancing flag for delete/backspace key on direction boundary
			/*if (event.text.length() == 0) {
			 int lineIndex = content.getLineAtOffset(event.start);
			 int lineOffset = content.getOffsetAtLine(lineIndex);
			 TextLayout layout = renderer.getTextLayout(lineIndex);
			 int levelStart = layout.getLevel(event.start - lineOffset);
			 int lineIndexEnd = content.getLineAtOffset(event.end);
			 if (lineIndex != lineIndexEnd) {
			 renderer.disposeTextLayout(layout);
			 lineOffset = content.getOffsetAtLine(lineIndexEnd);
			 layout = renderer.getTextLayout(lineIndexEnd);
			 }
			 int levelEnd = layout.getLevel(event.end - lineOffset);
			 renderer.disposeTextLayout(layout);
			 if (levelStart != levelEnd) {
			 caretAlignment = PREVIOUS_OFFSET_TRAILING;
			 } else {
			 caretAlignment = OFFSET_LEADING;
			 }
			 }*/
		}
		IDocument *doc = priv.doc;
		int line = priv.selection_line;
		int offset = priv.selection_offset;
		doc->Replace(line, offset, priv.selection_length, text, text_length);
		for (int i = priv.selection_line; i <= line; i++) {
			ClearTextLayout(i);
		}
		priv.selection_line = line;
		priv.selection_length = 0;
		priv.selection_offset = offset;
		// set the caret position prior to sending the modify event.
		// fixes 1GBB8NJ
		if (updateCaret /*&& !(blockSelection && blockXLocation != -1)*/) {
			// always update the caret location. fixes 1G8FODP
			//setSelection(event.start + event.text.length(), 0, true, false);
			//showCaret();
		}
		SetScrollBars(true);
	}
}

int TextEditor::GetVerticalScrollOffset() {
	if (priv.verticalScrollOffset == -1) {
		/*renderer.calculate(0, topIndex);
		 int height = 0;
		 for (int i = 0; i < topIndex; i++) {
		 height += renderer.getLineHeight(i);
		 }*/
		int height = priv.topIndex * GetLineHeight();
		height -= priv.topIndexY;
		priv.verticalScrollOffset = height;
	}
	return priv.verticalScrollOffset;
}

int TextEditor::GetLineHeight() {
	return priv.ascent + priv.descent;
}
int TextEditor::GetLineHeight(IDocumentLine *line) {
	return 0;
}

bool TextEditor::ScrollVertical(int pixels, bool adjustScrollBar) {
	if (pixels == 0) {
		return false;
	}
	if (priv.verticalScrollOffset != -1) {
		WScrollBar verticalBar;
		GetVerticalBar(verticalBar);
		if (verticalBar.IsOk() && adjustScrollBar) {
			verticalBar.SetSelection(priv.verticalScrollOffset + pixels);
		}
		int scrollWidth = priv.clientAreaWidth - priv.leftMargin
				- priv.rightMargin;
		if (pixels > 0) {
			int sourceY = priv.topMargin + pixels;
			int scrollHeight = priv.clientAreaHeight - sourceY
					- priv.bottomMargin;
			if (scrollHeight > 0) {
				Scroll(priv.leftMargin, priv.topMargin, priv.leftMargin,
						sourceY, scrollWidth, scrollHeight, true);
			}
			if (sourceY > scrollHeight) {
				int redrawY = WMAX(0, priv.topMargin + scrollHeight);
				int redrawHeight = WMIN(priv.clientAreaHeight,
						pixels - scrollHeight);
				Redraw(priv.leftMargin, redrawY, scrollWidth, redrawHeight,
						true);
			}
		} else {
			int destinationY = priv.topMargin - pixels;
			int scrollHeight = priv.clientAreaHeight - destinationY
					- priv.bottomMargin;
			if (scrollHeight > 0) {
				Scroll(priv.leftMargin, destinationY, priv.leftMargin,
						priv.topMargin, scrollWidth, scrollHeight, true);
			}
			if (destinationY > scrollHeight) {
				int redrawY = WMAX(0, priv.topMargin + scrollHeight);
				int redrawHeight = WMIN(priv.clientAreaHeight,
						-pixels - scrollHeight);
				Redraw(priv.leftMargin, redrawY, scrollWidth, redrawHeight,
						true);
			}
		}
		priv.verticalScrollOffset += pixels;
		CalculateTopIndex(pixels);
	} else {
		CalculateTopIndex(pixels);
		Redraw();
	}
	//setCaretLocation();
	return true;
}
bool TextEditor::ScrollHorizontal(int pixels, bool adjustScrollBar) {
	if (pixels == 0)
		return false;
	if (priv.wordWrap)
		return false;
	WScrollBar horizontalBar;
	GetHorizontalBar(horizontalBar);
	if (horizontalBar.IsOk() && adjustScrollBar) {
		horizontalBar.SetSelection(priv.horizontalScrollOffset + pixels);
	}
	int scrollHeight = priv.clientAreaHeight - priv.topMargin
			- priv.bottomMargin;
	if (pixels > 0) {
		int sourceX = priv.leftMargin + pixels;
		int scrollWidth = priv.clientAreaWidth - sourceX - priv.rightMargin;
		if (scrollWidth > 0) {
			Scroll(priv.leftMargin, priv.topMargin, sourceX, priv.topMargin,
					scrollWidth, scrollHeight, true);
		}
		if (sourceX > scrollWidth) {
			WCanvas::Redraw(priv.leftMargin + scrollWidth, priv.topMargin,
					pixels - scrollWidth, scrollHeight, true);
		}
	} else {
		int destinationX = priv.leftMargin - pixels;
		int scrollWidth = priv.clientAreaWidth - destinationX
				- priv.rightMargin;
		if (scrollWidth > 0) {
			Scroll(destinationX, priv.topMargin, priv.leftMargin,
					priv.topMargin, scrollWidth, scrollHeight, true);
		}
		if (destinationX > scrollWidth) {
			WCanvas::Redraw(priv.leftMargin + scrollWidth, priv.topMargin,
					-pixels - scrollWidth, scrollHeight, true);
		}
	}
	priv.horizontalScrollOffset += pixels;
	//setCaretLocation();
	return true;
}
void TextEditor::CalculateScrollBars() {
	WScrollBar horizontalBar;
	WScrollBar verticalBar;
	GetHorizontalBar(horizontalBar);
	GetVerticalBar(verticalBar);
	SetScrollBars(true);
	if (verticalBar.IsOk()) {
		verticalBar.SetIncrement(GetLineHeight());
	}
	if (horizontalBar.IsOk()) {
		horizontalBar.SetIncrement(priv.averageCharWidth);
	}
}
void TextEditor::CalculateTopIndex(int delta) {
	int oldDelta = delta;
	int oldTopIndex = priv.topIndex;
	int oldTopIndexY = priv.topIndexY;
	if (IsFixedLineHeight()) {
		int verticalIncrement = GetLineHeight();
		if (verticalIncrement == 0) {
			return;
		}
		int verticalScrollOffset = GetVerticalScrollOffset();
		float _div = (float) verticalScrollOffset / (float) verticalIncrement;
		priv.topIndex = ceil(_div);
		// Set top index to partially visible top line if no line is fully
		// visible but at least some of the widget client area is visible.
		// Fixes bug 15088.
		if (priv.topIndex > 0) {
			if (priv.clientAreaHeight > 0) {
				int bottomPixel = GetVerticalScrollOffset()
						+ priv.clientAreaHeight;
				int fullLineTopPixel = priv.topIndex * verticalIncrement;
				int fullLineVisibleHeight = bottomPixel - fullLineTopPixel;
				// set top index to partially visible line if no line fully fits in
				// client area or if space is available but not used (the latter should
				// never happen because we use claimBottomFreeSpace)
				if (fullLineVisibleHeight < verticalIncrement) {
					priv.topIndex = GetVerticalScrollOffset()
							/ verticalIncrement;
				}
			} else if (priv.topIndex >= priv.doc->GetLinesCount()) {
				priv.topIndex = priv.doc->GetLinesCount() - 1;
			}
		}
	} else {
		/*if (delta >= 0) {
		 delta -= topIndexY;
		 int lineIndex = topIndex;
		 int lineCount = content.getLineCount();
		 while (lineIndex < lineCount) {
		 if (delta <= 0) break;
		 delta -= renderer.getLineHeight(lineIndex++);
		 }
		 if (lineIndex < lineCount && -delta + renderer.getLineHeight(lineIndex) <= clientAreaHeight - topMargin - bottomMargin) {
		 topIndex = lineIndex;
		 topIndexY = -delta;
		 } else {
		 topIndex = lineIndex - 1;
		 topIndexY = -renderer.getLineHeight(topIndex) - delta;
		 }
		 } else {
		 delta -= topIndexY;
		 int lineIndex = topIndex;
		 while (lineIndex > 0) {
		 int lineHeight = renderer.getLineHeight(lineIndex - 1);
		 if (delta + lineHeight > 0) break;
		 delta += lineHeight;
		 lineIndex--;
		 }
		 if (lineIndex == 0 || -delta + renderer.getLineHeight(lineIndex) <= clientAreaHeight - topMargin - bottomMargin) {
		 topIndex = lineIndex;
		 topIndexY = - delta;
		 } else {
		 topIndex = lineIndex - 1;
		 topIndexY = - renderer.getLineHeight(topIndex) - delta;
		 }
		 }*/
	}
	if (priv.topIndex < 0) {
		// bugs
		priv.topIndex = 0;
	}
	if (priv.topIndex != oldTopIndex || oldTopIndexY != priv.topIndexY) {
		int width = GetWidth();
		//renderer.calculateClientArea();
		if (width != GetWidth()) {
			SetScrollBars(false);
		}
	}
}

bool TextEditor::IsFixedLineHeight() {
	return priv.fixedLineHeight;
}
void TextEditor::SetScrollBar(WScrollBar &bar, int clientArea, int maximum,
		int margin) {
	int inactive = 1;
	if (clientArea < maximum) {
		bar.SetMaximum(maximum - margin);
		bar.SetThumb(clientArea - margin);
		bar.SetPageIncrement(clientArea - margin);
		if (priv.notalwaysShowScroll)
			bar.SetVisible(true);
	} else if (bar.GetThumb() != inactive || bar.GetMaximum() != inactive) {
		bar.SetValues(bar.GetSelection(), bar.GetMinimum(), inactive, inactive,
				bar.GetIncrement(), inactive);
	}
}

void TextEditor::SetScrollBars(bool vertical) {
	priv.ignoreResize = W_TRUE;
	if (!IsFixedLineHeight() || priv.notalwaysShowScroll)
		vertical = true;
	WScrollBar verticalBar;
	WScrollBar horizontalBar;
	if (vertical) {
		GetVerticalBar(verticalBar);
	}
	GetHorizontalBar(horizontalBar);
	int oldHeight = priv.clientAreaHeight;
	int oldWidth = priv.clientAreaWidth;
	if (priv.notalwaysShowScroll) {
		if (verticalBar.IsOk())
			verticalBar.SetVisible(false);
		if (horizontalBar.IsOk())
			horizontalBar.SetVisible(false);
	}
	if (verticalBar.IsOk()) {
		SetScrollBar(verticalBar, priv.clientAreaHeight, GetHeight(),
				priv.topMargin + priv.bottomMargin);
	}
	if (horizontalBar.IsOk() && !priv.wordWrap) {
		SetScrollBar(horizontalBar, priv.clientAreaWidth, GetWidth(),
				priv.leftMargin + priv.rightMargin);
		if (priv.notalwaysShowScroll && horizontalBar.GetVisible()
				&& verticalBar.IsOk()) {
			SetScrollBar(verticalBar, priv.clientAreaHeight, GetHeight(),
					priv.topMargin + priv.bottomMargin);
			if (verticalBar.GetVisible()) {
				SetScrollBar(horizontalBar, priv.clientAreaWidth, GetWidth(),
						priv.leftMargin + priv.rightMargin);
			}
		}
	}
	if (priv.notalwaysShowScroll) {
		RedrawMargins(oldHeight, oldWidth);
	}
	priv.ignoreResize = W_FALSE;
}

void TextEditor::SetFont0(WFont *font, int tabs) {
	WGraphics gc;
	WFontMetrics metrics;
	WTextLayout layout;
	WRect rect;
	layout.Create();
	layout.SetFont(priv.regularFont);
	if (font != 0) {
		if (priv.boldFont != 0)
			priv.boldFont->Dispose();
		if (priv.italicFont != 0)
			priv.italicFont->Dispose();
		if (priv.boldItalicFont != 0)
			priv.boldItalicFont->Dispose();
		priv.boldFont = priv.italicFont = priv.boldItalicFont = 0;
		priv.regularFont = font;
		layout.SetText("    ");
		layout.SetFont(font);
		WTextStyle style;
		int _style[4] = { W_NORMAL, W_BOLD, W_ITALIC, W_BOLD | W_ITALIC };
		for (int i = 0; i < 4; i++) {
			style.Init();
			style.SetFont(GetFont0(_style[i]));
			layout.SetStyle(style, i, i);
		}
		layout.GetLineMetrics(0, metrics);
		priv.ascent = metrics.getAscent() + metrics.getLeading();
		priv.descent = metrics.getDescent();
		if (priv.boldFont != 0)
			priv.boldFont->Dispose();
		if (priv.italicFont != 0)
			priv.italicFont->Dispose();
		if (priv.boldItalicFont != 0)
			priv.boldItalicFont->Dispose();
		priv.boldFont = priv.italicFont = priv.boldItalicFont = 0;
	}
	layout.Dispose();
	layout.Create();
	layout.SetFont(priv.regularFont);
	char *tabBuffer;
	char tabBufferTmp[0x100];
	if (tabs >= sizeof(tabBufferTmp))
		tabBuffer = (char*) malloc((tabs + 1) * sizeof(char));
	else
		tabBuffer = tabBufferTmp;
	memset(tabBuffer, ' ', tabs);
	/*for (int i = 0; i < tabs; i++) {
	 tabBuffer[i] = ' ';
	 }*/
	tabBuffer[tabs] = 0;
	layout.SetText(tabBuffer);
	if (tabs >= sizeof(tabBufferTmp))
		free(tabBuffer);
	layout.GetBounds(rect);
	priv.tabWidth = rect.width;
	layout.Dispose();
	if (GetGraphics(gc)) {
		//gc.SetFont(font);
		gc.GetFontMetrics(metrics);
		priv.averageCharWidth = metrics.getAverageCharWidth();
		priv.fixedPitch = gc.StringExtent("l").width
				== gc.StringExtent("W").width; //$NON-NLS-1$ //$NON-NLS-2$
	}
}
void TextEditor::OnLineInserted(const DocumentEvent &event) {
	IDocumentLine *line = event.fDocument->GetLine(event.fLine);
	if (line != 0) {
		IDocumentLineData *info = priv.doc->GetLineData(line,
				priv.lineDataIndex);
		if (info == 0 && priv.lineDataIndex != 0) {
			info = new TextEditorLineInfoBase();
			priv.doc->SetLineData(line, priv.lineDataIndex, info);
		}
	}
}

WFont* TextEditor::GetFont0(int style) {
	WFont **_f = 0;
	switch (style) {
	case W_BOLD:
		_f = &priv.boldFont;
		break;
	case W_ITALIC:
		_f = &priv.italicFont;
		break;
	case W_BOLD | W_ITALIC:
		_f = &priv.boldItalicFont;
		break;
	default:
		_f = &priv.regularFont;
		break;
	}
	if (*_f == 0) {
		WFontData data;
		priv.regularFont->GetFontData(data);
		data.SetStyle(style);
		*_f = WFont::Create(data);
	}
	if (*_f == 0)
		return priv.regularFont;
	return *_f;
}
TextEditorLineInfoBase::TextEditorLineInfoBase() {
	this->lineWidth = -1;
	this->lineHeight = -1;
}

TextEditorLineInfoBase::~TextEditorLineInfoBase() {
}

void TextEditorLineInfoBase::RemoveLine(IDocumentLine *line) {
	delete this;
}

int TextEditor::GetHeight() {
	int defaultLineHeight = GetLineHeight();
	if (IsFixedLineHeight()) {
		int lineCount = priv.doc->GetLinesCount();
		return lineCount * defaultLineHeight + priv.topMargin
				+ priv.bottomMargin;
	}
	int totalHeight = 0;
	/*int width = styledText.getWrapWidth();
	 for (int i = 0; i < lineCount; i++) {
	 int height = lineHeight[i];
	 if (height == -1) {
	 if (width > 0) {
	 int length = content.getLine(i).length();
	 height = ((length * averageCharWidth / width) + 1) * defaultLineHeight;
	 } else {
	 height = defaultLineHeight;
	 }
	 }
	 totalHeight += height;
	 }*/
	return totalHeight + priv.topMargin + priv.bottomMargin;
}

int TextEditor::GetWidth() {
	return priv.maxWidth;
}

void TextEditor::RedrawMargins(int oldHeight, int oldWidth) {
	/* Redraw the old or new right/bottom margin if needed */
	if (oldWidth != priv.clientAreaWidth) {
		if (priv.rightMargin > 0) {
			int x = (
					oldWidth < priv.clientAreaWidth ?
							oldWidth : priv.clientAreaWidth) - priv.rightMargin;
			WCanvas::Redraw(x, 0, priv.rightMargin, oldHeight, false);
		}
	}
	if (oldHeight != priv.clientAreaHeight) {
		if (priv.bottomMargin > 0) {
			int y = (
					oldHeight < priv.clientAreaHeight ?
							oldHeight : priv.clientAreaHeight)
					- priv.bottomMargin;
			WCanvas::Redraw(0, y, oldWidth, priv.bottomMargin, false);
		}
	}
}

int TextEditor::GetLineIndex(int y) {
	y -= priv.topMargin;
	if (IsFixedLineHeight()) {
		int lineHeight = GetLineHeight();
		int lineIndex = (y + GetVerticalScrollOffset()) / lineHeight;
		int lineCount = priv.doc->GetLinesCount();
		lineIndex = WMAX(0, WMIN(lineCount - 1, lineIndex));
		return lineIndex;
	}
	if (y == priv.topIndexY)
		return priv.topIndex;
	IDocument *doc = priv.doc;
	IDocumentLine *line = doc->GetLine(priv.topIndex);
	if (y < priv.topIndexY) {
		while (y < priv.topIndexY) {
			line = doc->PrevLine(line);
			if (line == 0)
				break;
			y += GetLineHeight(line);
		}
	} else {
		int lineCount = priv.doc->GetLinesCount();
		int lineHeight = GetLineHeight(line);
		while (y - lineHeight >= priv.topIndexY) {
			y -= lineHeight;
			line = priv.doc->NextLine(line);
			if (line == 0)
				break;
			lineHeight = GetLineHeight(line);
		}
	}
	return doc->GetLineIndex(line);
}
int TextEditor::GetLinePixel(int lineIndex) {
	int lineCount = priv.doc->GetLinesCount();
	lineIndex = WMAX(0, WMIN(lineCount, lineIndex));
	if (IsFixedLineHeight()) {
		int lineHeight = GetLineHeight();
		return lineIndex * lineHeight - GetVerticalScrollOffset()
				+ priv.topMargin;
	}
	if (lineIndex == priv.topIndex)
		return priv.topIndexY + priv.topMargin;
	int height = priv.topIndexY;
	IDocument *doc = priv.doc;
	IDocumentLine *line = doc->GetLine(priv.topIndex);
	int _line = priv.topIndex;
	if (lineIndex > priv.topIndex) {
		while (_line < lineIndex) {
			height += GetLineHeight(line);
			line = doc->NextLine(line);
			if (line == 0)
				break;
			_line++;
		}
	} else {
		while (_line >= lineIndex) {
			height -= GetLineHeight(line);
			line = doc->PrevLine(line);
			if (line == 0)
				break;
			_line--;
		}
	}
	return height + priv.topMargin;
}

void TextEditor::ClaimRightFreeSpace() {
	int newHorizontalOffset = WMAX(0, GetWidth() - priv.clientAreaWidth);
	if (newHorizontalOffset < priv.horizontalScrollOffset) {
		// item is no longer drawn past the right border of the client area
		// align the right end of the item with the right border of the
		// client area (window is scrolled right).
		ScrollHorizontal(newHorizontalOffset - priv.horizontalScrollOffset,
				true);
	}
}

void TextEditor::CreateCaretBitmaps() {
	int caretWidth = BIDI_CARET_WIDTH;
	/*if (leftCaretSurface != null) {
	 if (defaultCaret != null && leftCaretSurface.equals(defaultCaret.getImage())) {
	 defaultCaret.setImage(null);
	 }
	 leftCaretSurface.dispose();
	 }*/
	int lineHeight = GetLineHeight();
	WGraphics gc;
	WSurface leftCaretSurface;
	WImage leftCaretImage;
	leftCaretSurface.Create(caretWidth, lineHeight);
	gc.Create(leftCaretSurface);
	gc.SetBackground(W_COLOR_BLACK);
	gc.FillRectangle(0, 0, caretWidth, lineHeight);
	gc.SetForeground(W_COLOR_WHITE);
	gc.DrawLine(0, 0, 0, lineHeight);
	gc.DrawLine(0, 0, caretWidth - 1, 0);
	gc.DrawLine(0, 1, 1, 1);
	gc.Dispose();

	leftCaretSurface.GetImage(leftCaretImage);
	defaultCaret.SetImage(leftCaretImage);

	/*if (rightCaretBitmap != null) {
	 if (defaultCaret != null && rightCaretBitmap.equals(defaultCaret.getImage())) {
	 defaultCaret.setImage(null);
	 }
	 rightCaretBitmap.dispose();
	 }
	 rightCaretBitmap = new Image(display, caretWidth, lineHeight);
	 gc = new GC (rightCaretBitmap);
	 gc.setBackground(display.getSystemColor(SWT.COLOR_BLACK));
	 gc.fillRectangle(0, 0, caretWidth, lineHeight);
	 gc.setForeground(display.getSystemColor(SWT.COLOR_WHITE));
	 gc.drawLine(caretWidth-1,0,caretWidth-1,lineHeight);
	 gc.drawLine(0,0,caretWidth-1,0);
	 gc.drawLine(caretWidth-1,1,1,1);
	 gc.dispose();*/
}

void TextEditor::UpdateCaretVisibility() {
	//WCaret caret = getCaret();
	if (this->defaultCaret.IsOk()) {
		/*if (blockSelection && blockXLocation != -1) {
		 caret.setVisible(false);
		 } else {*/
		WPoint location;
		WSize size;
		defaultCaret.GetLocation(location);
		defaultCaret.GetSize(size);
		int visible = priv.topMargin <= location.y + size.height
				&& location.y <= priv.clientAreaHeight - priv.bottomMargin
				&& priv.leftMargin <= location.x + size.width
				&& location.x <= priv.clientAreaWidth - priv.rightMargin;
		defaultCaret.SetVisible(visible);
		//}
	}
}

WTextLayout* TextEditor::GetTextLayout(WTextLayout &tmp, int lineIndex) {
	WTextLayout *ret;
	int diff = lineIndex - priv.topIndex;
	if (diff >= 0 && diff < CACHE_SIZE) {
		int i = diff + priv.layouts_start;
		if (i >= CACHE_SIZE)
			i -= CACHE_SIZE;
		ret = &layouts[i];
	} else {
		ret = &tmp;
	}
	if (!ret->IsOk()) {
		IDocumentLine *line = priv.doc->GetLine(lineIndex);
		if (line != 0) {
			OnLineStyled(lineIndex, line, ret);
		}
	}
	return ret;
}

void TextEditor::ClearTextLayout(int lineIndex) {
	int diff = lineIndex - priv.topIndex;
	if (diff >= 0 && diff < CACHE_SIZE) {
		int i = diff + priv.layouts_start;
		layouts[i].Dispose();
	}
}
void TextEditor::OnLineStyled(int lineIndex, IDocumentLine *line,
		WTextLayout *layout) {
	const char *text = priv.doc->GetLineUtf8Chars(line);
	int length = priv.doc->GetLineUtf8Length(line);
	layout->Create();
	layout->SetText(text, length);
	const char *label = "int";
	WTextStyle style;
	int label_length = strlen(label);
	for (int i = 0; i < length - label_length; i++) {
		if (!strncmp(&text[i], label, label_length)) {
			if ((i == 0 || text[i - 1] == ' ')
					&& ((i + label_length) == length
							|| text[i + label_length] == ' ')) {
				style.Init();
				style.SetUnderlineSingle(W_COLOR_BLUE);
				style.SetForeground(W_COLOR_BLUE);
				style.SetFont(GetFont0(W_BOLD));
				style.SetStrikeout(W_COLOR_BLACK);
				layout->SetStyle(style, i, i + label_length);
				i += label_length;
			}
		}
	}
}

ObjectRef* TextEditor::GetRef(int *tmp) {
	return &this->ref;
}

ObjectRef* TextEditorLineInfoBase::GetRef(int *tmp) {
	return new (tmp) ObjectRef();
}

void TextEditor::OnLineDeleted(const DocumentEvent &event) {
}

void TextEditor::OnLineUpdated(const DocumentEvent &event) {
}
