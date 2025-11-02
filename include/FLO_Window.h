#ifndef	__FLO_WINDOW_H__
#define	__FLO_WINDOW_H__
/*-----------------------------------------------------------------
|	|FILE|:    FLO_Window.h
|
|	FL Library
|	version 2.11
|------------------------------------------------------------------
	|NOTES|:

	For convenience the font handle is stored as
	global once so all classes can use them directly.

	DON'T forget to call SetFontHandle()
	at the start up of the program.

-----------------------------------------------------------------*/
#include <string>
#include <map>
#include <OgreOverlay.h>
#include <OgreBorderPanelOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>

#include <FL_OISInput.h>
#include <FLO_Font.h>


using namespace Ogre;


/*
	These MUST be set, or the some dialog classes won't work.
	SetOverlayHandle is a legacy code. Now Overlay is
	created PER dialog.

	SetFontHandle still needs to be set by all apps.
*/
void SetOverlayHandle( Ogre::Overlay *handle ); // legacy code.
void SetFontHandle( FL_OgreFont *handle );




// FL Window Result
//struct FLW_RESULT
struct FLW_RESULT
{
	int i, i2, i3;
	float f;
	string s, s2, s3;
	string message;
	string id;					// sender name
	//std::string actionID;		// what happened
	FLW_RESULT()					{ Init(); }
	FLW_RESULT( const string& sid )		{ Init(); id = sid; }
	void Init()						{ i=0; f=0.0f; }
};

const string FLW_RESULT_MSG[] = {
	"LETTEROUT",	//---- persistent msg
	"IDLE",			//---- non-persistent msg
	"CHOICE",
	"CANCEL",
	"DESTROYME",
	"CHILDDESTROYED"
};

/*=================================================================================

								Dialog Base Class

	----------------------------------------------------------------------------
	@remarks
	FLW_Dialog represents a dialog window, which contain FLWE_Interface
	based elements. All dialogs should be derived from this class
	and implement its own creation routine as all dialogs look
	different.
	@par
	FLW_Dialog can be nested thru SetNextDialog to create
	a sequence of dialogs on user input.
	@par
	Intended to be used as a base class to more
	specialized/custom dialog classes.

*/
class FLWE_Interface;	// Forward declaration

class FLW_Dialog
{
public:
	FLW_Dialog( const std::string& sID, Ogre::Overlay *overlayHandle );
	virtual ~FLW_Dialog();
	virtual void SetPosition( Real left, Real top );
	virtual void SetDimension( Real width, Real height );
	virtual void SetZOrder( unsigned short z )				{ m_Overlay->setZOrder(z); }
	virtual unsigned short GetZOrder()						{ return m_Overlay->getZOrder(); }
	//virtual void SetNextDialog( FLW_Dialog *nd )			{ m_ChildDialogs[nd->m_sID] = nd; }
	virtual void AddElement( FLWE_Interface *handle );
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	virtual FLW_RESULT Loop( Real timeSinceLastFrame );
	virtual void AlignWindowCenter();
	virtual const OverlayContainer* GetOverlayContainer()	{ return m_OverlayContainer; }
	virtual Real GetLeft()									{ return m_OverlayContainer->getLeft(); }
	virtual Real GetTop()									{ return m_OverlayContainer->getTop(); }
	virtual Real GetWidth()									{ return m_OverlayContainer->getWidth(); }
	virtual Real GetHeight()								{ return m_OverlayContainer->getHeight(); }
	string	GetID() const									{ return m_sID; }
	bool	IsShown();
	void	Show( bool b =true );
	bool	IsInFocus()										{ return m_InFocus; }
	void	InFocus( bool inf =true )						{ m_InFocus = inf; }

protected:
	string								m_sID;
	Overlay								*m_Overlay;
	OverlayContainer					*m_OverlayContainer;// OverlayContainer on which Elements are made
	std::map<string,FLWE_Interface*>	m_Elements;			// Dialog elements
	std::map<string,FLW_Dialog*>		m_ChildDialogs;		// Next dialog in this chain
	FLW_Dialog							*m_ChildDialog;		// Next dialog in this chain
	std::map<string,FLW_RESULT>	m_ResultStack;
	bool								m_InFocus;			// true if this dialog is in focus
};


/*---------------------------------------------------------------------------

			Dialog derivatives - typical dialogs for convenience.
					To be used as is or as base class.

---------------------------------------------------------------------------*/
class FLW_WallpaperDialog : public FLW_Dialog
{
public:
	FLW_WallpaperDialog( const string& sID, Ogre::Overlay *overlayHandle );
	~FLW_WallpaperDialog();
	void SetDimension( Real width, Real height );
	void SetWallpaper( const String& materialName );
protected:
	string		m_WallpaperElementName;
};

class FLW_BorderedDialog : public FLW_Dialog
{
public:
	FLW_BorderedDialog( const string& sID, Ogre::Overlay *overlayHandle, Real borderSize =0.02f );
	~FLW_BorderedDialog();
	void SetDimension( Real width, Real height );
	Real GetBorderSize_Sides();
	Real GetBorderSize_TopAndBottom();
protected:
	string		m_BorderElementName;
private:
	Real		m_BorderSize;			// border size of the sides!! Size of top and bottom are calculated according to screen ratio.
};

/*
	Border + single line text. Border size is adjusted to text length.
*/
class FLW_BorderedSingleTextDialog : public FLW_BorderedDialog
{
public:
	FLW_BorderedSingleTextDialog( const string& sID, Ogre::Overlay *overlayHandle, FL_OgreFont *fontHandle,
									Real borderSize =0.02f, Real leftPad =0.03f, Real rightPad =0.03f,
															Real topPad =0.0f, Real bottomPad =0.0f );
	void SetText( const wstring& txt );
	FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
protected:
	Real	m_LeftPadding, m_RightPadding, m_TopPadding, m_BottomPadding;
	string	m_SingleTextAreaName;
};

/*
	Ready made 'Chat' dialog, DQ/FF style,
	but with a wallpaper and WITHOUT borders.
*/
class FLW_WallpaperedTextDialog : public FLW_WallpaperDialog
{
public:
	FLW_WallpaperedTextDialog( const string& sID, Ogre::Overlay *overlayHandle, FL_OgreFont *fontHandle,
								Real paddingAroundText =0 );
	~FLW_WallpaperedTextDialog();
	//virtual void Loop( Real timeSinceLastFrame );
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	virtual void SetDimension( Real width, Real height );
	//virtual void Input_PressedOnce( eJOYPAD_BUTTON button );
	void SetText( const wstring& txt );
	void SetTimeInterval( Real ti );
protected:
	Real		m_PaddingAroundText;
	string		m_TextAreaName;
};

/*
	Ready made 'Chat' dialog, DQ/FF style.
*/
class FLW_BorderedTextDialog : public FLW_BorderedDialog
{
public:
	FLW_BorderedTextDialog( const string& sID, Ogre::Overlay *overlayHandle, FL_OgreFont *fontHandle,
							Real borderSize =0.02f, Real leftPad =0.03f, Real rightPad =0.03f );
	~FLW_BorderedTextDialog();
	//virtual void Loop( Real timeSinceLastFrame );
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	virtual void SetDimension( Real width, Real height );
	//virtual void Input_PressedOnce( eJOYPAD_BUTTON button );
	void SetText( const wstring& txt );
	void SetTimeInterval( Real ti );
	Real GetTimeInterval();
	bool HasTextPushEnded();
	bool IsPaused();
protected:
	string		m_TextAreaName;
	Real		m_LeftPadding, m_RightPadding;
};


/*
	Ready made 'Menu' dialog, DQ/FF style.
*/
class FLW_BorderedMenuDialog : public FLW_BorderedDialog
{
public:
	FLW_BorderedMenuDialog( const string& sID, Ogre::Overlay *overlayHandle, FL_OgreFont *fontHandle,
							int rows, int columns,
							Real borderSize =0.02f, Real leftPad =0.019f, Real rightPad =0.015f );
	~FLW_BorderedMenuDialog();
	/* @returns FLW_RESULT::i contains the cursor index. */
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	/* Ignores all the dialog elements to set the dimension.
		Usually it is recommended to call SetMatrixTextWidth(). */
	virtual void SetDimension( Real width, Real height );
	/* Force the text area width of a given column to 'width'.
		Usually it is recommended to call SetText() and let width adjust automatically.
		Call FitBorderToTextArea_() methods to snap border to the newly sized text area. */
	void SetMatrixTextWidth( Real width, int index =-1 );
	/* Auto resize overlay container & border to fit matrix text area. */
	void FitBorderToTextArea();
	/* Auto resize overlay container & border width to fit matrix text area.
		Call ResizeColumnsToWidestText() in advance if auto-adjusting text area as well. */
	void FitBorderToTextAreaH();
	/* Auto resize overlay container & border height to fit matrix text area. */
	void FitBorderToTextAreaV();
	/* Resizes the text area to widest text present, WITHOUT resizing border.
		@remark Proper way to control dialog size is to call SetMatrixTextWidth() but for quick
		experiment auto sizing can help. This method is an automated version of SetMatrixTextWidth(),
		where it finds the longest text and resizes the text area width.	*/
	void ResizeColumnsToWidestText();
	/* Convenience method to call ResizeColumnsToWidestText() followed by FitBorderToTextAreaH(). */
	void FitBorderToWidestTextH();
	/* Sets text at the specified index. @note Call FitBorderToTextAreaH() to adjust border to the new width. */
	void SetText( const wstring& txt, int idx );
	wstring GetText( int idx ) const;
	Real GetLeft_CursorPanel();
	Real GetTop_CursorPanel();
	int GetCursorPosition();
	void SetCursorPosition( int idx );
	void ShowCursor( bool show =true );
	bool IsCursorShown();
	FL_OgreFont* GetFontHandle();
	void AllowEmptySelection( bool b =true );

protected:
	string		m_MatrixTextAreaName;
	Real		m_LeftPadding;	// padding to the left of text area, before the border
	Real		m_RightPadding;	// padding to the right of text area, before the border
};

/*
	Fixed-column Vertical List

	e.g. Item shop list DQ/FF style.
*/
class FLW_BorderedVerticalListDialog : public FLW_BorderedDialog
{
public:
	FLW_BorderedVerticalListDialog( const string& sID, Ogre::Overlay *overlayHandle, FL_OgreFont *fontHandle,
									int visibleRows, int visibleColumns,
									Real borderSize =0.02f, Real leftPad =0.04f, Real rightPad =0.0f );
	~FLW_BorderedVerticalListDialog();
	void LockCursorH( bool lock =true );
	/* @returns FLW_RESULT::i contains the cursor index in data table coordinate. */
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	/* columnIndex: column index i.e. | 0th | 1st | 2nd | 3rd | 
		@note Remember to call UpdateData() after calling this method.
		Also ResizeWindowToShowAll() will override this call with longest text width. */
	void SetCellWidth( Real width, int columnIndex );
	/* Auto-resizes cell widths to longest text length, then resizes the whole window to show all.
		@note If you intend to use SetCellWidth() to force certain width, don't call this! */
	void ResizeWindowToShowAll();
	/* Redraws data in the view. */
	void UpdateData();
	/* Auto resize overlay container & border to fit matrix text area. */
	//void FitBorderToTextArea();
	/* Auto resize overlay container & border width to fit matrix text area. */
	//void FitBorderToTextAreaH();
	/* Auto resize overlay container & border height to fit matrix text area. */
	//void FitBorderToTextAreaV();
	/* Push a row of data into the back of data table. # of data read is always equal to # of columns.
		@note Call UpdateData() once pushing is done so view is updated. */
	void PushBackData( std::deque<wstring> *rowData );
	void ClearList();
	wstring GetDataTableText( int idx );
	/* @returns the index of data table the cursor is pointing at. */
	int GetCursorDataIndex();
	void ShowCursor( bool show =true );
	void AllowEmptySelection( bool b =true );
protected:
	string		m_ListTextAreaName;
	Real		m_LeftPadding;
	Real		m_RightPadding;
};


/*
*/

/*================================================================================================

									FL Window Elements

							Basic elements contained in a dialog

------------------------------------------------------------------------------------------------*/

/*	FL Window Element Interface + derivatives
	@remarks
	FLWE_Interface is the base class of all dialog elements,
	the so-called "controls". This includes backdrop panel,
	bordered panel, text areas etc i.e. elements which
	reside inside a dialog.
*/
class FLWE_Interface
{
public:
	FLWE_Interface( const std::string& sid, Ogre::OverlayContainer *oc )
						: m_sID(sid), m_AcceptInput(true)	{}
	virtual ~FLWE_Interface();
	/* Updating content & returning result if necessary. */
	virtual FLW_RESULT Loop( std::map<string,FLW_RESULT> *resultStack, Real timeSinceLastFrame );
	/* For now the only way to inject input into any window. */
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	virtual void SetPosition( Real left, Real top )			{}
	virtual void SetDimension( Real width, Real height )	{}
	virtual void SetMaterial( const String& materialName )	{}
	//virtual void SetZOrder( unsigned short z )				{}
public:
	std::string								m_sID;
	bool									m_AcceptInput;	// allows focus & input channeling
};


/*
	Panel with single backdrop texture
*/
class FLWE_Panel : public FLWE_Interface
{
public:
	FLWE_Panel( const std::string& sID, Ogre::OverlayContainer *overlayHandle );
	~FLWE_Panel();
	//virtual void Loop( std::map<string,FLW_RESULT> *resultStack, Real timeSinceLastFrame );
	virtual void SetPosition( Real left, Real top )			{	m_PanelElement->setPosition(left,top); }
	/* Set width or height to -1 to retain the same size. */
	virtual void SetDimension( Real width, Real height )
													{	if( width>0 ) m_PanelElement->setWidth(width);
														if( height>0 ) m_PanelElement->setHeight(height);
													}
	virtual void SetMaterial( const String& materialName )	{	m_PanelElement->setMaterialName(materialName); }

protected:
	Ogre::PanelOverlayElement	*m_PanelElement;
};


/*
	Bordered panel
*/
class FLWE_Border : public FLWE_Interface
{
public:
	FLWE_Border( const string& sID, Ogre::OverlayContainer *oc, Real borderSize_topAndBottom =0.01f );
	~FLWE_Border();
	virtual void SetPosition( Real left, Real top )			{	m_BorderElement->setPosition(left,top); }
	/* Set width or height to -1 to retain the same size. */
	virtual void SetDimension( Real width, Real height )
													{	if( width>0 ) m_BorderElement->setWidth(width);
														if( height>0 ) m_BorderElement->setHeight(height);
													}
	Real GetBorderSize_Sides()						{ return m_BorderElement->getRightBorderSize(); }
	Real GetBorderSize_TopAndBottom()				{ return m_BorderElement->getTopBorderSize(); }
	// for border u need centre and border materials. virtual void SetMaterial( String materialName )			{	m_BorderElement->setMaterialName(materialName); }

protected:
	Ogre::BorderPanelOverlayElement	*m_BorderElement;
};

/*
	Instant, single line of text string.
*/
class FLWE_TextArea : public FLWE_Interface
{
public:
	FLWE_TextArea( const std::string& sID, Ogre::OverlayContainer *overlayHandle, FL_OgreFont *fontHandle );
	~FLWE_TextArea();
	//virtual FLW_RESULT Loop( std::map<string,FLW_RESULT> *resultStack, Real timeSinceLastFrame );
	virtual void SetPosition( Real left, Real top )			{	m_TextArea->setPosition(left,top); }
	virtual void SetDimension( Real width, Real height )
													{	if( width>0 ) m_TextArea->setWidth(width);
														if( height>0 ) m_TextArea->setHeight(height);
													}
	void SetText( const wstring& txt )				{	m_TextArea->setCaption( txt );
														m_TextArea->setWidth(m_FontHandle->TextWidth(txt));
													}
	Real GetWidth()									{	return m_TextArea->getWidth(); }
	Real GetHeight()								{	return m_TextArea->getHeight(); }
	FL_OgreFont*	GetFontHandle()					{	return m_FontHandle; }
protected:
	FL_OgreFont					*m_FontHandle;
	TextAreaOverlayElement		*m_TextArea;
};

/*
	Most common text area. Allows multiple lines of text in a set region.
				_________________________________
		^		|								|
		|		|			txt line 0			|
		|		| ----------------------------- |
		|		|			txt line 1			|
4 LINES	|		| ----------------------------- |
		|		|			txt line 2			|
		|		| ----------------------------- |
		|		|			txt line 3			|
		-		|_______________________________|

*/
class FLWE_MultiLineTextArea : public FLWE_Interface
{
public:
	/* UP TO 'maxLines' # of text lines will be used in this area.	*/
	FLWE_MultiLineTextArea( const std::string& sID, Ogre::OverlayContainer *overlayHandle, FL_OgreFont *fontHandle,
							int maxLines =1, Real lineSpacing =0.0f );
	~FLWE_MultiLineTextArea();
	virtual FLW_RESULT Loop( std::map<string,FLW_RESULT> *resultStack, Real timeSinceLastFrame );
	/* @returns FLW_RESULT::i contains the cursor index. */
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	virtual void SetPosition( Real left, Real top );
	virtual void SetDimension( Real width, Real height );
	void SetText( const wstring& txt );
	void ClearText();
	void SetTimeInterval( Real ti )			{	m_Interval=ti; }
	Real GetTimeInterval()					{	return m_Interval; }
	/* @returns true if text output process is finished. */
	bool HasTextPushEnded()					{	return m_Word.empty() && !m_WordStack->HasMoreElements(); }
	bool IsPaused()							{	return m_PauseText; }
	Real GetWidth()							{	return m_OverlayContainer->getWidth(); }
	Real GetHeight()						{	return m_OverlayContainer->getHeight(); }
protected:
	int	_GetSizeVisibleRows();
	void _MoveToNextLine();
	void _PauseText();
	void _UnpauseText()		{	m_PauseText = false;
								m_TimeCounter = m_Interval;
								m_BlinkerText->hide();
							}
protected:
	FL_OgreFont								*m_FontHandle;
	OverlayContainer						*m_OverlayContainer;
	TextAreaOverlayElement					*m_BlinkerText;
	std::deque<TextAreaOverlayElement*>		m_TextLines;
	Real									m_LineSpacing;
	FL::FL_LexerW							*m_WordStack;	// stores what needs to be displayed on screen
	wstring									m_Word;			// current word being processed
	int										m_TextLineRow;	// current row in the text line area
	Real									m_Interval;		// time interval (sec) at which a letter is displayed
	Real									m_TimeCounter;	// counter in seconds, used in Loop
	bool									m_PauseText;	// pauses printing out
	//std::deque<DisplayString>				m_WordStack;
	//std::deque<DisplayString>::iterator		m_WordStackIterator;
};

/*
	Matrix consisting of multiple rows and columns of single-line text areas.
	Think of spreadsheet or DQ's main menu.
	Also allows cursor display & movement, freely or limited to certain rows/columns.

	|-----> X
	|
	|		-------		-------		-------		-------		-------
	/		index 0		index 1		index 2		index 3		index 4
	Y		-------		-------		-------		-------		-------
			index 5		index 6		index 7		index 8		index 9
			-------		-------		-------		-------		-------
			index 10	index 11	index 12	index 13	index 14
			-------		-------		-------		-------		-------
*/
class FLWE_MatrixTextArea : public FLWE_Interface
{
	//const int _PaddingLeft = 
public:
	FLWE_MatrixTextArea( const std::string& sID, Ogre::OverlayContainer *overlayHandle, FL_OgreFont *fontHandle,
							int rows =1, int columns =1, Real lineSpacingH =0.04f, Real lineSpacingV =0.0f );
	~FLWE_MatrixTextArea();
	//virtual FLW_RESULT Loop( std::map<string,FLW_RESULT> *resultStack, Real timeSinceLastFrame );
	/* @returns FLW_RESULT::i contains the cursor index when message is "CHOICE". */
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	virtual void SetPosition( Real left, Real top );
	/* Usually, use SetMatrixTextWidth() to set the size of a specific column.
		Internally m_OverlayContainer is resized to ensure the correct size, so this method shouldn't be necessary. */
	virtual void SetDimension( Real width, Real height )	{	m_OverlayContainer->setDimensions( width, height );}
	/* Sets text for the specified index. Also adjusts cell width internally. */
	void SetText( const wstring& txt, int index );
	wstring GetText( int index ) const;
	/* index: if index is below 0 (such as default value), it recalculate cursor position so that it is placed correctly. */
	void SetCursorPosition( int index =-1 );
	int GetCursorPosition()								{	return m_CursorPosition; }
	Real GetLeft()										{	return m_OverlayContainer->getLeft(); }
	Real GetTop()										{	return m_OverlayContainer->getTop(); }
	/* @return Left coordinate of the cursor panel in MatrixTextArea coordinate. */
	Real GetLeft_CursorPanel()							{	return m_CursorPanel->getLeft(); }
	/* @return Top coordinate of the cursor panel in MatrixTextArea coordinate. */
	Real GetTop_CursorPanel()							{	return m_CursorPanel->getTop(); }
	void ShowCursor( bool b )							{	if(b)m_CursorPanel->show(); else m_CursorPanel->hide(); }
	bool IsCursorShown()								{	return m_CursorPanel->isVisible(); }
	/* Lock cursor movement along the X axis. */
	void LockCursorH( bool b =true )					{	m_LockCursorH = b; }
	/* Lock cursor movement along the Y axis. */
	void LockCursorV( bool b =true )					{	m_LockCursorV = b; }
	/* Returns the total width of the 1st row. */
	Real GetWidth();
	/* Returns # of rows X font height. */
	Real GetHeight();
	/* Resizes each column to the widest text width.
		@returns The new total width of the whole area. 
		@remark SetText() only adjusts the width of the specified cell rather than the entire column
				to which it belongs. This method provides a convenient way to remedy this by automatically
				resizing all cells to the longest text in each column. */
	virtual Real ResizeColumnsToWidestText();
	/* Adjust the width of the specified column.
		@note CalculateLocalPositions() is called automatically and parent overlay resized to fit. */
	void SetMatrixTextWidth( Real width, int index =-1 );
	FL_OgreFont* GetFontHandle()		{ return m_FontHandle; }
	void AllowEmptySelection( bool b =true )		{	m_bAllowEmptySlotSelection = b; }
protected:
	/* Re-positions all matrix text & cursor correctly within m_OverlayContainer coordinate.
		Call this after altering text width etc. */
	void CalculateLocalPositions();
	/* Move cursor right (+) or left (-). */
	virtual void _MoveCursorBy1H( bool negative );
	virtual void _MoveCursorBy1V( bool negative );
	
protected:
	FL_OgreFont										*m_FontHandle;
	OverlayContainer								*m_OverlayContainer;
	PanelOverlayElement								*m_CursorPanel;
	int												m_CursorPosition;
	bool											m_LockCursorH;
	bool											m_LockCursorV;
	std::deque<TextAreaOverlayElement*>				m_MatrixText;
	int												m_RowSize, m_ColumnSize;
	Real											m_LineSpacingV;		// vertical
	Real											m_LineSpacingH;		// horizontal
	bool											m_bAllowEmptySlotSelection;
protected:
	Real											m_LeftPadding, m_RightPadding;
	Real											m_TopPadding, m_BottomPadding;
};

/*
	A viewport of row x column showing a part of a bigger spreadsheet.

	ENTIRE DATA TABLE
	|-----> X				VISIBLE DATA
	|				 ||=====================||
	|		-------	 ||	-------		-------	||	-------		-------
	/		index 0	 ||	index 1		index 2	||	index 3		index 4
	Y		-------	 ||	-------		-------	||	-------		-------
			index 5	 ||	index 6		index 7	||	index 8		index 9
			-------	 ||=====================||	-------		-------
			index 10	index 11	index 12	index 13	index 14
			-------		-------		-------		-------		-------

	NOTE:	Unlike FLWE_MatrixTextArea, text in rows/columns are NOT set thru SetText().
			Fill out the m_DataTable and it will be displayed according to m_DataPosition.
			Also, unlike FLWE_MatrixTextArea, FLWE_ScrollableMatrixTextArea uses
			a fixed column/row size regardless of the length of the text residing in each cell.

	NOTE2:	Don't forget to call UpdateDataRowSize() after changing data table size!
			i.e. after adding or removing data w/ GetDataTableHandle() or PushData().
*/
class FLWE_ScrollableMatrixTextArea : public FLWE_MatrixTextArea
{
public:
	FLWE_ScrollableMatrixTextArea( const std::string& sID, Ogre::OverlayContainer *overlayHandle, FL_OgreFont *fontHandle,
							int maxRows =1, int maxColumns =1, Real lineSpacingH =0.06f, Real lineSpacingV =0.0f,
							Real rightPad =0.1f, Real bottomPad =0.05f );
	~FLWE_ScrollableMatrixTextArea();
	/* @returns FLW_RESULT::i contains the cursor index in data table coordinate when message is "CHOICE". */
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	/* @returns the handle to the data table. Call UpdateDataRowSize() after altering data size. */
	std::deque<wstring>* GetDataTableHandle()			{ return &m_DataTable; }
	wstring GetDataTableText( int idx ) const			{ if(idx<(int)m_DataTable.size())return m_DataTable[idx]; return wstring(); }
	/* Pushes 1 string into data table. UpdateDataRowSize() must be called to update the # of rows. */
	void PushData( const wstring& txt )					{
		m_DataTable.push_back(txt);
		//_UpdateArrowsLocation(); // maybe not the most efficient.
	}
	void ClearDataTable()								{ m_DataTable.clear(); UpdateDataRowSize(); PushDataTableToMatrix(); }
	/* Calculates & updates row size based on the DataTable size and column size. */
	void UpdateDataRowSize()							{ m_DataRowSize = m_DataTable.size() / m_DataColumnSize; }
	void SetDataColumnSize( int col )					{ m_DataColumnSize = col; }
	//void SetDataRowSize( int row )						{ m_DataColumnSize = row; }
	int GetDataRowSize()								{ return m_DataRowSize; }
	int GetDataColumnSize()								{ return m_DataColumnSize; }
	/* @returns the index of data table the cursor is pointing at. */
	int GetCursorDataIndex();
	/* Sets top left coordinate within data table. @returns the new index which should be the same as index param.
		@remark If index was not accepted for some reason (e.g. scrolling off the list),
		return value will not match index. */
	int SetDataPosition( int index );
	/* Updates the visible column/row area with data in data table. */
	void PushDataTableToMatrix();
	void SetMatrixTextWidth( Real width, int index =-1 );
	virtual Real ResizeColumnsToWidestText();
	
protected:
	/* Shows/hides arrows by examining where data position is. */
	void _UpdateArrowsVisibility();
	/* Recalculates arrows locations. */
	void _UpdateArrowsLocation();
	/* Move cursor right (+) or left (-). */
	void _MoveCursorBy1V( bool negative );
	void _MoveCursorBy1H( bool negative )		{ FLWE_MatrixTextArea::_MoveCursorBy1H( negative ); }

protected:
	PanelOverlayElement			*m_UpArrowPanel, *m_DownArrowPanel;
	PanelOverlayElement			*m_LeftArrowPanel, *m_RightArrowPanel;
	std::deque<wstring>			m_DataTable;			// data spreadsheet of row x column
	int							m_DataColumnSize;		// # of cells along X axis
	int							m_DataRowSize;			// # of cells along Y axis
	int							m_DataPosition;			// index of top-left cell where current visible data begins.
};

/*


		*** WORK IN PROGRESS ***



	New version of FLWE_ScrollableMatrixTextArea.
	FLWE_ScrollableMatrixTextArea is kept for legacy.

	A viewport of row x column showing a part of a bigger spreadsheet.

	ENTIRE DATA TABLE
	|-----> X				VISIBLE DATA
	|				 ||=====================||
	|		-------	 ||	-------		-------	||	-------		-------
	/		index 0	 ||	index 1		index 2	||	index 3		index 4
	Y		-------	 ||	-------		-------	||	-------		-------
			index 5	 ||	index 6		index 7	||	index 8		index 9
			-------	 ||=====================||	-------		-------
			index 10	index 11	index 12	index 13	index 14
			-------		-------		-------		-------		-------


	--------------------------------------------------
		TODO : Currently not implemented OR used.
	--------------------------------------------------

*/
class FLWE_SpreadSheet : public FLWE_MatrixTextArea
{
public:
	FLWE_SpreadSheet( const std::string& sID, Ogre::OverlayContainer *overlayHandle, FL_OgreFont *fontHandle,
							int maxRows =1, int maxColumns =1, Real lineSpacingH =0.06f, Real lineSpacingV =0.0f,
							Real rightPad =0.1f, Real bottomPad =0.05f );
	~FLWE_SpreadSheet();
	/* @returns FLW_RESULT::i contains the cursor index in data table coordinate when message is "CHOICE". */
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	/* @returns the handle to the data table. Call UpdateDataRowSize() after altering data size. */
	std::deque<wstring>* GetDataTableHandle()			{ return &m_DataTable; }
	wstring GetDataTableText( int idx ) const			{ if(idx<(int)m_DataTable.size())return m_DataTable[idx]; return wstring(); }
	/* Pushes 1 string into data table. UpdateDataRowSize() must be called to update the # of rows. */
	void PushData( const wstring& txt )					{
		m_DataTable.push_back(txt);
		//_UpdateArrowsLocation(); // maybe not the most efficient.
	}
	void ClearDataTable()								{ m_DataTable.clear(); UpdateDataRowSize(); PushDataTableToMatrix(); }
	/* Calculates & updates row size based on the DataTable size and column size. */
	void UpdateDataRowSize()							{ m_DataRowSize = m_DataTable.size() / m_DataColumnSize; }
	void SetDataColumnSize( int col )					{ m_DataColumnSize = col; }
	//void SetDataRowSize( int row )						{ m_DataColumnSize = row; }
	int GetDataRowSize()								{ return m_DataRowSize; }
	int GetDataColumnSize()								{ return m_DataColumnSize; }
	/* @returns the index of data table the cursor is pointing at. */
	int GetCursorDataIndex();
	/* Sets top left coordinate within data table. @returns the new index which should be the same as index param.
		@remark If index was not accepted for some reason (e.g. scrolling off the list),
		return value will not match index. */
	int SetDataPosition( int index );
	/* Updates the visible column/row area with data in data table. */
	void PushDataTableToMatrix();
	void SetMatrixTextWidth( Real width, int index =-1 );
	virtual Real ResizeColumnsToWidestText();
	void AllowEmptySelection( bool b =true )		{	m_bAllowEmptySelection = b; }
	
protected:
	/* Shows/hides arrows by examining where data position is. */
	void _UpdateArrowsVisibility();
	/* Recalculates arrows locations. */
	void _UpdateArrowsLocation();
	/* Move cursor right (+) or left (-). */
	void _MoveCursorBy1V( bool negative );
	void _MoveCursorBy1H( bool negative )		{ FLWE_MatrixTextArea::_MoveCursorBy1H( negative ); }

protected:
	PanelOverlayElement			*m_UpArrowPanel, *m_DownArrowPanel;
	PanelOverlayElement			*m_LeftArrowPanel, *m_RightArrowPanel;
	std::deque<wstring>			m_DataTable;			// data spreadsheet of row x column
	int							m_DataColumnSize;		// # of cells along X axis
	int							m_DataRowSize;			// # of cells along Y axis
	int							m_DataPosition;			// index of top-left cell where current visible data begins.
	bool						m_bAllowEmptySelection;	// true to allow moving cursor to an empty slot.
};



/*=========================================================================
							Dialog Stack
			Minimal manager/container of FLW_Dialog elements
			
			Stack of dialogs. Input automatically goes to the back
			of the stack while loop is called for all dialogs.
-------------------------------------------------------------------------*/
class FL_DialogStack
{
public:
	FL_DialogStack() {}
	virtual ~FL_DialogStack();
	/* Calls Loop() in all registered dialogs. */
	virtual void Loop( Real timeSinceLastFrame );
	/* Passes input to the dialog at the back of the stack. */
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	virtual void Push( FLW_Dialog *handle );
	virtual void PopBack();
	virtual void PopBackAndHide();
	/* Removes the last dialog and frees the object memory. */
	virtual void PopAndDelete();
	/* Return number of registered dialog objects. */
	virtual int DialogStackSize()		{ return (int)m_DialogStack.size(); }

public:
	std::deque<FLW_Dialog*>		m_DialogStack;
	std::deque<FLW_RESULT>		m_ResultStack;
};



class DialogManager : public FL_DialogStack
{
public:
	DialogManager() : m_IsActive(true)	{}
	bool IsActive()							{ if(m_IsActive&&DialogStackSize()) return true; return false; }
	void SetActive( bool b =true )			{ m_IsActive = b; }

protected:
	bool m_IsActive;
};



/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif	// __FLO_WINDOW_H__