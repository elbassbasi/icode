/*
 * IDocument.h
 *
 *  Created on: 15 mai 2020
 *      Author: Azeddine El Bassbasi
 */

#ifndef ICODE_INCLUDE_TEXT_IDOCUMENT_H_
#define ICODE_INCLUDE_TEXT_IDOCUMENT_H_
#include "../core/core.h"
class IDocumentListener;
class IDocumentLineData;
class IDocumentData;
class IDocumentLine;
class ICODE_PUBLIC IDocument: public IObject {
public:
	static IDocument* CreateDefaultDocument();
public:
	/* --------------- text access and manipulation --------------------------- */
	/**
	 * Returns the number of characters in this document.
	 *
	 * @return the number of characters in this document
	 */
	virtual int GetLength()=0;

	/**
	 * Returns this document's complete text.
	 *
	 * @return the document's complete text
	 */
	virtual int Get(char *text, int length)=0;

	/**
	 * Returns this document's text for the specified range.
	 *
	 * @param offset the document offset
	 * @param length the length of the specified range
	 * @param text
	 * @param text_length the length of the text
	 * @return the text size for the specified range
	 */
	virtual int Get(IDocumentLine *line, int offset, int length, char *text,
			int text_length)=0;
	/**
	 * Replaces the content of the document with the given text.
	 *
	 * @param text the new content of the document
	 * @param length the length of the text
	 */
	virtual int Set(const char *text, int length)=0;

	/**
	 * Substitutes the given text for the specified document range.
	 *
	 * @param iter the document offset
	 * @param length the length of the specified range
	 * @param text the substitution text
	 * @param text_length the length of the text
	 * @return return the number off character added
	 */
	virtual int Replace(int &line, int &offset, int length, const char *text,
			int text_length)=0;

	virtual wunichar GetChar(IDocumentLine *line, int offset, int utf8offset)=0;
	/* ---------------------- line information -------------------------------- */
	/**
	 * Returns the number of lines in this document.
	 * <p>
	 * Note that a document always has at least one line.
	 * </p>
	 *
	 * @return the number of lines in this document.
	 */
	virtual int GetLinesCount()=0;
	/**
	 * Returns the line at which the character of the specified position is located.
	 * The first line has the line number 0. A new line starts directly after a line
	 * delimiter. <code>(offset == document length)</code> is a valid argument although there is no
	 * corresponding character.
	 *
	 * @param offset the document offset
	 * @return the number of the line
	 */
	virtual IDocumentLine* GetLineOfOffset(int offset)=0;
	/**
	 * Determines the offset of the first character of the given line.
	 *
	 * @param line the line of interest
	 * @return the document offset
	 */
	virtual int GetLineOffset(IDocumentLine *line)=0;
	virtual IDocumentLine* GetLine(int index)=0;
	/**
	 * Returns the number of lines which are occupied by a given text range.
	 *
	 * @param offset the offset of the specified text range
	 * @param length the length of the specified text range
	 * @return the number of lines occupied by the specified range
	 */
	virtual int GetLinesCount(IDocumentLine *line, int offset, int utf8offset,
			int length)=0;
	/**
	 * Returns the number of the line at which the character of the specified position is located.
	 * The first line has the line number 0. A new line starts directly after a line
	 * delimiter. <code>(offset == document length)</code> is a valid argument although there is no
	 * corresponding character.
	 *
	 * @param offset the document offset
	 * @return the number of the line
	 */
	virtual int GetLineIndex(IDocumentLine *line)=0;
	/**
	 * Returns the length of the given line including the line's delimiter.
	 *
	 * @param line the line of interest
	 * @return the length of the line
	 */
	virtual int GetLineLength(IDocumentLine *line)=0;
	virtual const char* GetLineUtf8Chars(IDocumentLine *line)=0;
	virtual int GetLineUtf8Length(IDocumentLine *line)=0;
	virtual IDocumentLine* NextLine(IDocumentLine *line)=0;
	virtual IDocumentLine* PrevLine(IDocumentLine *line)=0;
	/* line data*/

	virtual int AddLineDataID(StringId *id) = 0;
	int AddLineDataID(const char *id) {
		return AddLineDataID(ManagersGet()->RegistreString(id));
	}
	virtual IDocumentLineData* GetLineData(IDocumentLine *line, int id)=0;
	virtual bool SetLineData(IDocumentLine *line, int id,
			IDocumentLineData *data)=0;
public:
	/* document data */
	virtual int AddDataID(StringId *id)=0;
	int AddDataID(const char *id) {
		return AddDataID(ManagersGet()->RegistreString(id));
	}
	virtual IDocumentData* GetData(int id)=0;
	virtual bool SetData(int id, IDocumentData *data)=0;
	/**
	 * Registers the document listener with the document. After registration
	 * the IDocumentListener is informed about each change of this document.
	 * If the listener is already registered nothing happens.<p>
	 * An <code>IDocumentListener</code> may call back to this method
	 * when being inside a document notification.
	 *
	 * @param listener the listener to be registered
	 */
	virtual void AddDocumentListener(IDocumentListener *listener)=0;

	/**
	 * Removes the listener from the document's list of document listeners.
	 * If the listener is not registered with the document nothing happens.<p>
	 * An <code>IDocumentListener</code> may call back to this method
	 * when being inside a document notification.
	 *
	 * @param listener the listener to be removed
	 */
	virtual void RemoveDocumentListener(IDocumentListener *listener)=0;
};
template<>
inline const IID __IID<IDocument>() {
	return IID_IDocument;
}
class ICODE_PUBLIC IDocumentLineData: public IObject {
public:
	virtual void RemoveLine(IDocumentLine *line)=0;

};
class ICODE_PUBLIC IDocumentData: public IObject {
public:

};
class DocumentEvent {
public:
	/** The changed document */
	IDocument *fDocument;
	IDocumentLine *fLinePtr;
	/** The start line changed */
	int fLine;
	/** Text inserted into the document */
	const char *fText;
	/** The line offset */
	int fLineOffset;
	/** Length of the replaced document text */
	int fLength;
	int fTextLength;
};
class ICODE_PUBLIC IDocumentListener: public IObject {
public:
	virtual void OnLineInserted(const DocumentEvent &event) = 0;
	virtual void OnLineDeleted(const DocumentEvent &event) = 0;
	virtual void OnLineUpdated(const DocumentEvent &event) = 0;
	/**
	 * The manipulation described by the document event has been performed.
	 *
	 * @param event the document event describing the document change
	 */
	virtual void DocumentChanged(const DocumentEvent &event)=0;
};
#endif /* ICODE_INCLUDE_TEXT_IDOCUMENT_H_ */
