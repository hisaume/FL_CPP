#include <FLO_Window.h>
#include <FL_General.h>
#include <FL_GeneralTools.h>



//Ogre::Overlay *gOverlayHandle =NULL;
FL_OgreFont *gFontHandle =NULL;

void SetOverlayHandle( Ogre::Overlay *handle )
{
//	gOverlayHandle = handle;
}

void SetFontHandle( FL_OgreFont *handle )
{
	gFontHandle = handle;
}


/*
		Utilities
*/

void OVERLAY_SetBorderPanelProperties( Ogre::BorderPanelOverlayElement *panel )
{
	Real ONE = 1.0f;
	panel->setTopBorderUV( 0.3333f, 0, 0.6666f, 0.3333f );
	panel->setBottomBorderUV( 0.3333f, 0.6666f, 0.6666f, ONE );
	panel->setLeftBorderUV( 0, 0.3333f, 0.3333f, 0.6666f );
	panel->setRightBorderUV( 0.6666f, 0.3333f, ONE, 0.6666f );
	panel->setTopLeftBorderUV( 0, 0, 0.3333f, 0.3333f );
	panel->setBottomRightBorderUV( 0.6666f, 0.6666f, ONE, ONE );
	panel->setTopRightBorderUV( 0.6666f, 0, ONE, 0.3333f );
	panel->setBottomLeftBorderUV( 0, 0.6666f, 0.3333f, ONE );
	//panel->setUV( 0.3333f, 0.3333f, 0.6666f, 0.6666f );	// Centre texture
}

/*
	FLW_Dialog
*/
FLW_Dialog::FLW_Dialog( std::string sID, Ogre::Overlay *overlayHandle )
: m_InFocus(true), m_sID(sID) ,m_ChildDialog(NULL)
{
	OverlayManager& om = OverlayManager::getSingleton();

	if( NULL == overlayHandle ) {
		//overlayHandle = gOverlayHandle;
		m_Overlay = om.create( m_sID+String("_Overlay") );
		m_Overlay->show();
	}

	m_OverlayContainer = static_cast<OverlayContainer*>( om.createOverlayElement("Panel", String(sID)) );
	m_OverlayContainer->setMetricsMode( Ogre::GMM_RELATIVE );
	m_OverlayContainer->setPosition( 0, 0 );
	m_OverlayContainer->setDimensions( 1.0f, 1.0f );
	m_OverlayContainer->show();
	m_Overlay->add2D( m_OverlayContainer );	// attach it to an overlay
}

FLW_Dialog::~FLW_Dialog()
{
	std::map<string,FLW_Dialog*>::iterator itD;
	for( itD=m_ChildDialogs.begin(); itD!=m_ChildDialogs.end(); ++itD ) {
		delete itD->second;
	}
	if( m_ChildDialog )
		delete m_ChildDialog;

	std::map<string,FLWE_Interface*>::iterator iter;
	for( iter=m_Elements.begin(); iter!=m_Elements.end(); ++iter )
		delete iter->second;

	OverlayManager::getSingleton().destroyOverlayElement( m_OverlayContainer );
	OverlayManager::getSingleton().destroy( m_Overlay );

	LOGHI( L"Dialog destructor ended. " ); LOGHIVAR( m_sID, true );
}

void FLW_Dialog::SetPosition( Real left, Real top )
{
	m_OverlayContainer->setPosition( left, top );
}

void FLW_Dialog::SetDimension( Real width, Real height )
{
	if( width>0 && height>0 )
		m_OverlayContainer->setDimensions( width, height );
	else if( width > 0 )
		m_OverlayContainer->setWidth( width );
	else if( height > 0 )
		m_OverlayContainer->setHeight( height );
}

void FLW_Dialog::AddElement( FLWE_Interface *handle )
{
	m_Elements[handle->m_sID] = handle;
}

bool FLW_Dialog::IsShown()
{
	return m_OverlayContainer->isVisible();
}

void FLW_Dialog::Show( bool b )
{
	if( b )	m_OverlayContainer->show();
	else	m_OverlayContainer->hide();
}

void FLW_Dialog::AlignWindowCenter()
{
	Real left = (1.0f-m_OverlayContainer->getWidth()) / 2.0f;
	Real top = (1.0f-m_OverlayContainer->getHeight()) / 2.0f;
	SetPosition( left, top );
}

FLW_RESULT FLW_Dialog::Loop( Real timeSinceLastFrame )
{
	FLW_RESULT sendRes;

	if( m_ChildDialog ) {
		if( m_ChildDialog->IsShown() && m_ChildDialog->IsInFocus() )
			sendRes = m_ChildDialog->Loop( timeSinceLastFrame );
	}

	// Go thru child ELEMENTS
	m_ResultStack.clear();
	std::map<string,FLWE_Interface*>::iterator iter;
	for( iter=m_Elements.begin(); iter!=m_Elements.end(); ++iter ) {
		if( m_ChildDialog )
			m_ResultStack[iter->first] = FLW_RESULT(iter->first);	// do nothing
		else {
			m_ResultStack[iter->first] = iter->second->Loop( NULL, timeSinceLastFrame );
			if( m_ResultStack[iter->first].message == "LETTEROUT" )
				sendRes.message = "LETTEROUT";
		}
	}
	/*else {
		std::map<string,FLW_Dialog*>::iterator itD;
		for( itD=m_ChildDialogs.begin(); itD!=m_ChildDialogs.end(); ++itD ) {
			delete itD->second;
		}
	}*/

	sendRes.id = m_sID;
	return sendRes;
}

FLW_RESULT FLW_Dialog::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	bool childProcess = false;
	FLW_RESULT childRes(m_sID);

	if( m_ChildDialog ) {
		childProcess = true;
		//if( button == JOYP_B ) {	// Cancel child dialog
		//	SAFE_DELETE( m_ChildDialog );
		//} else {
		//childRes = m_ChildDialog->Input_PressedOnce( button );
		//if( childRes.message = "DESTROYME" ) {
		//	childRes.childprocessed = true;
		//	childRes.message = "CHILDDESTROYED";
		//	SAFE_DELETE( m_ChildDialog );
		//}
		//	if( childRes.message == "LETTEROUT" )
		//		;
		//}
	}
	// This dialog's elements must be seen to be processed
	// EVEN if m_ChildDialog is processed, as m_ResultStack must be good every loop.

	m_ResultStack.clear();
	std::map<string,FLWE_Interface*>::iterator iter;
	for( iter=m_Elements.begin(); iter!=m_Elements.end(); ++iter ) {
		if( childProcess )
			m_ResultStack[iter->first] = FLW_RESULT();	// as child is being processed, this dialog did nothing.
		else
			m_ResultStack[iter->first] = iter->second->Input_PressedOnce( button );
	}

	//childRes.id = m_sID;		Return child ID as this can only be base class and parent need to know.
	return childRes;
}

/*
	Wallpaper Dialog
*/
FLW_WallpaperDialog::FLW_WallpaperDialog( string sID, Ogre::Overlay *overlayHandle )
: FLW_Dialog( sID, overlayHandle )
{
	m_WallpaperElementName = sID + "_WallpaperArea";
	FLWE_Interface *wallpaper = new FLWE_Panel( m_WallpaperElementName, m_OverlayContainer );
	AddElement( wallpaper );
}

FLW_WallpaperDialog::~FLW_WallpaperDialog()
{
}

void FLW_WallpaperDialog::SetWallpaper( String materialName )
{
	m_Elements[m_WallpaperElementName]->SetMaterial( materialName );
}

void FLW_WallpaperDialog::SetDimension( Ogre::Real width, Ogre::Real height )
{
	FLW_Dialog::SetDimension( width, height );
	((FLWE_Panel*)m_Elements[m_WallpaperElementName])->SetDimension( width, height );
}


/*
	Bordered Dialog
*/
FLW_BorderedDialog::FLW_BorderedDialog( string sID, Ogre::Overlay *overlayHandle, Real borderSize )
: FLW_Dialog( sID, overlayHandle ), m_BorderSize(borderSize)
{
	m_BorderElementName = sID + "_BorderArea";
	FLWE_Interface *borderArea = new FLWE_Border( m_BorderElementName, m_OverlayContainer, m_BorderSize );
	AddElement( borderArea );
}

FLW_BorderedDialog::~FLW_BorderedDialog()
{
}

void FLW_BorderedDialog::SetDimension( Ogre::Real width, Ogre::Real height )
{
	FLW_Dialog::SetDimension( width, height );
	((FLWE_Border*)m_Elements[m_BorderElementName])->SetDimension( width, height );
}

Real FLW_BorderedDialog::GetBorderSize_Sides()
{
	return ((FLWE_Border*)m_Elements[m_BorderElementName])->GetBorderSize_Sides();
}

Real FLW_BorderedDialog::GetBorderSize_TopAndBottom()
{
	return ((FLWE_Border*)m_Elements[m_BorderElementName])->GetBorderSize_TopAndBottom();
}


/*
	Bordered SINGLE text dialog
*/
FLW_BorderedSingleTextDialog::FLW_BorderedSingleTextDialog( string sID,
															Ogre::Overlay *overlayHandle, FL_OgreFont *fontHandle,
															Real borderSize,
															Real leftPad, Real rightPad, Real topPad, Real bottomPad )
: FLW_BorderedDialog( sID, overlayHandle, borderSize ), m_LeftPadding(leftPad), m_RightPadding(rightPad),
														m_TopPadding(topPad), m_BottomPadding(bottomPad)
{
	m_SingleTextAreaName = sID + "_SingleTextArea";
	FLWE_Interface *textArea = new FLWE_TextArea( m_SingleTextAreaName, m_OverlayContainer, fontHandle );
	textArea->SetPosition( GetBorderSize_Sides()+m_LeftPadding, GetBorderSize_TopAndBottom()+m_TopPadding );
	AddElement( textArea );

	SetText( L"NO_TXT" );
}

void FLW_BorderedSingleTextDialog::SetText( std::wstring txt )
{
	((FLWE_TextArea*)m_Elements[m_SingleTextAreaName])->SetText( txt );
	FLW_BorderedDialog::SetDimension(	((FLWE_TextArea*)m_Elements[m_SingleTextAreaName])->GetWidth()
											+ m_LeftPadding + m_RightPadding + GetBorderSize_Sides()*2,
										((FLWE_TextArea*)m_Elements[m_SingleTextAreaName])->GetHeight()
											+ m_TopPadding + m_BottomPadding + GetBorderSize_TopAndBottom()*2 );
}

FLW_RESULT FLW_BorderedSingleTextDialog::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	if( button == JOYP_B || button == JOYP_A ) {
		FLW_RESULT sendRes(m_sID);
		sendRes.message = "DESTROYME";
		return sendRes;
	}
	else {
		FLW_BorderedDialog::Input_PressedOnce( button );	// fills m_ResultStack
		FLW_RESULT sendRes = m_ResultStack[m_SingleTextAreaName];
		sendRes.id = m_sID;
		return sendRes;
	}
}

/*
	Wallpaper TEXT dialog
*/
FLW_WallpaperedTextDialog::FLW_WallpaperedTextDialog( std::string sID, Ogre::Overlay *overlayHandle, FL_OgreFont *fontHandle,
													   	Real paddingAroundText )
: FLW_WallpaperDialog(sID, overlayHandle), m_PaddingAroundText(paddingAroundText)
{
	m_TextAreaName = sID + "_WallpaperTextArea";
	FLWE_Interface *txtArea = new FLWE_MultiLineTextArea( m_TextAreaName, m_OverlayContainer, fontHandle, 20, 0.0f );
	txtArea->SetPosition( m_PaddingAroundText, m_PaddingAroundText );
	txtArea->SetDimension( 1.0f-m_PaddingAroundText*2, 1.0f-m_PaddingAroundText*2 );

	AddElement( txtArea );
}

FLW_WallpaperedTextDialog::~FLW_WallpaperedTextDialog()
{
}

/*
void FLW_WallpaperedTextDialog::Loop( Real timeSinceLastFrame )
{
	FLW_Dialog::Loop( timeSinceLastFrame );
}*/

FLW_RESULT FLW_WallpaperedTextDialog:: Input_PressedOnce( eJOYPAD_BUTTON button )
{
	FLW_Dialog::Input_PressedOnce( button );
	FLW_RESULT sendRes( m_sID );
	// Process m_ResultStack[m_TextAreaName] to alter sendRes if necessary
	return sendRes;
}

void FLW_WallpaperedTextDialog::SetDimension( Real width, Real height )
{
	FLW_Dialog::SetDimension( width, height );
	FLW_WallpaperDialog::SetDimension( width, height );
	//m_Elements[m_WallpaperElementName]->SetDimension( width, height );
	m_Elements[m_TextAreaName]->SetDimension( width-m_PaddingAroundText*2, height-m_PaddingAroundText*2 );
}

void FLW_WallpaperedTextDialog::SetText( std::wstring txt )
{
	((FLWE_MultiLineTextArea*)m_Elements[m_TextAreaName])->SetText( txt );
}

void FLW_WallpaperedTextDialog::SetTimeInterval( Real ti )
{
	((FLWE_MultiLineTextArea*)m_Elements[m_TextAreaName])->SetTimeInterval(ti);
}

/*
void FLW_WallpaperedTextDialog::Input_PressedOnce( eJOYPAD_BUTTON button )
{
}*/


/*----------------------------------
		Bordered TEXT dialog
----------------------------------*/
FLW_BorderedTextDialog::FLW_BorderedTextDialog( std::string sID, Ogre::Overlay *overlayHandle, FL_OgreFont *fontHandle,
											   	Real borderSize, Real leftPad, Real rightPad )
: FLW_BorderedDialog( sID, overlayHandle, borderSize ), m_LeftPadding(leftPad), m_RightPadding(rightPad)
{
	m_TextAreaName = sID + "_BorderedTextArea";
	FLWE_Interface *txtArea = new FLWE_MultiLineTextArea( m_TextAreaName, m_OverlayContainer, fontHandle, 20, 0.0f );
	txtArea->SetPosition( GetBorderSize_Sides()+m_LeftPadding, GetBorderSize_TopAndBottom() );
	txtArea->SetDimension( 1.0f-GetBorderSize_Sides()*2-m_LeftPadding-m_RightPadding, 1.0f-GetBorderSize_TopAndBottom()*2 );

	AddElement( txtArea );
}

FLW_BorderedTextDialog::~FLW_BorderedTextDialog()
{
}

/*
void FLW_BorderedTextDialog::Loop( Real timeSinceLastFrame )
{
	FLW_Dialog::Loop( timeSinceLastFrame );
}*/

FLW_RESULT FLW_BorderedTextDialog:: Input_PressedOnce( eJOYPAD_BUTTON button )
{
	FLW_Dialog::Input_PressedOnce( button );
	FLW_RESULT sendRes(m_sID);
	// Process m_ResultStack[m_TextAreaName] to change sendRes, if necessary.
	sendRes.message = m_ResultStack[m_TextAreaName].message;
	return sendRes;
}

void FLW_BorderedTextDialog::SetDimension( Real width, Real height )
{
	//FLW_Dialog::SetDimension( width, height );
	FLW_BorderedDialog::SetDimension( width, height );
	//m_Elements[m_BorderElementName]->SetDimension( width, height );
	m_Elements[m_TextAreaName]->SetDimension( width-GetBorderSize_Sides()*2-m_LeftPadding-m_RightPadding,
												height-GetBorderSize_TopAndBottom()*2 );
}

void FLW_BorderedTextDialog::SetText( std::wstring txt )
{
	((FLWE_MultiLineTextArea*)m_Elements[m_TextAreaName])->SetText( txt );
}

void FLW_BorderedTextDialog::SetTimeInterval( Real ti )
{
	((FLWE_MultiLineTextArea*)m_Elements[m_TextAreaName])->SetTimeInterval(ti);
}

Real FLW_BorderedTextDialog::GetTimeInterval()
{
	return ((FLWE_MultiLineTextArea*)m_Elements[m_TextAreaName])->GetTimeInterval();
}

bool FLW_BorderedTextDialog::HasTextPushEnded()
{
	return ((FLWE_MultiLineTextArea*)m_Elements[m_TextAreaName])->HasTextPushEnded();
}

bool FLW_BorderedTextDialog::IsPaused()
{
	return ((FLWE_MultiLineTextArea*)m_Elements[m_TextAreaName])->IsPaused();
}

/*
void FLW_BorderedTextDialog::Input_PressedOnce( eJOYPAD_BUTTON button )
{
}*/

/*---------------------------------
			Menu Dialog
---------------------------------*/
FLW_BorderedMenuDialog::FLW_BorderedMenuDialog(
							string sID, Ogre::Overlay *overlayHandle, FL_OgreFont *fontHandle,
							int rows, int columns, Real borderSize, Real leftPad, Real rightPad )
: FLW_BorderedDialog( sID, overlayHandle, borderSize ), m_LeftPadding(leftPad), m_RightPadding(rightPad)
{
	m_MatrixTextAreaName = sID + "_MainMatrixTextArea";
	FLWE_Interface *txtArea = new FLWE_MatrixTextArea( m_MatrixTextAreaName, m_OverlayContainer, fontHandle, rows, columns );
	txtArea->SetPosition( GetBorderSize_Sides()+m_LeftPadding, GetBorderSize_TopAndBottom() );
	AddElement( txtArea );

	ResizeColumnsToWidestText();
	FitBorderToTextArea();
}

FLW_BorderedMenuDialog::~FLW_BorderedMenuDialog()
{
}

/*
void FLW_BorderedMenuDialog::Loop( Real timeSinceLastFrame )
{
	FLW_Dialog::Loop( timeSinceLastFrame );
}*/

FLW_RESULT FLW_BorderedMenuDialog::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	if( button == JOYP_B ) {
		FLW_RESULT sendRes(m_sID);
		sendRes.message = "DESTROYME";
		return sendRes;
	}
	else {
		FLW_RESULT res = FLW_BorderedDialog::Input_PressedOnce( button );	// fills m_ResultStack
		//if( res.message != "CHILDDESTROYED" ) {
			res = m_ResultStack[m_MatrixTextAreaName];
			res.id = m_sID;
		//}
		return res;
	}
}

void FLW_BorderedMenuDialog::SetDimension( Real width, Real height )
{
	FLW_Dialog::SetDimension( width, height );
	FLW_BorderedDialog::SetDimension( width, height );
	//m_Elements[m_BorderElementName]->SetDimension( width, height );
	m_Elements[m_MatrixTextAreaName]->SetDimension( width-GetBorderSize_Sides()*2, height-GetBorderSize_TopAndBottom()*2 );
}

void FLW_BorderedMenuDialog::SetMatrixTextWidth( Real width, int index )
{
	// Adjusting matrix text column width changes the overall overlay size.
	((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->SetMatrixTextWidth( width, index );
	FitBorderToTextAreaH();
}

void FLW_BorderedMenuDialog::FitBorderToTextArea()
{
	Real width = ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->GetWidth();
	//Real width = ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->ResizeColumnsToWidestText();
	Real height = ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->GetHeight();
	width += 2*GetBorderSize_Sides()+m_LeftPadding+m_RightPadding;
	height += 2*GetBorderSize_TopAndBottom();
	m_OverlayContainer->setDimensions( width, height );
	m_Elements[m_BorderElementName]->SetDimension( width, height );
}

void FLW_BorderedMenuDialog::FitBorderToTextAreaH()
{
	Real width = ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->GetWidth();
	//Real width = ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->ResizeColumnsToWidestText();
	Real height = -1.0f;
	width += 2*GetBorderSize_Sides()+m_LeftPadding+m_RightPadding;
	m_OverlayContainer->setWidth( width );
	m_Elements[m_BorderElementName]->SetDimension( width, height );
}

void FLW_BorderedMenuDialog::ResizeColumnsToWidestText()
{
	((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->ResizeColumnsToWidestText();
}

void FLW_BorderedMenuDialog::FitBorderToWidestTextH()
{
	ResizeColumnsToWidestText();
	FitBorderToTextAreaH();
}

void FLW_BorderedMenuDialog::FitBorderToTextAreaV()
{
	Real width = -1.0f;
	Real height = ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->GetHeight();
	height += 2*GetBorderSize_TopAndBottom();
	m_OverlayContainer->setHeight( height );
	m_Elements[m_BorderElementName]->SetDimension( width, height );
}

void FLW_BorderedMenuDialog::SetText( wstring txt, int idx )
{
	((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->SetText(txt,idx);
}

wstring FLW_BorderedMenuDialog::GetText( int idx )
{
	return ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->GetText( idx );
}

Real FLW_BorderedMenuDialog::GetLeft_CursorPanel()
{
	return GetLeft() + ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->GetLeft_CursorPanel()
					 + ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->GetLeft();
}

Real FLW_BorderedMenuDialog::GetTop_CursorPanel()
{
	return GetTop() + ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->GetTop_CursorPanel()
					+ ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->GetTop();
}

int FLW_BorderedMenuDialog::GetCursorPosition()
{
	return ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->GetCursorPosition();
}

void FLW_BorderedMenuDialog::SetCursorPosition( int idx )
{
	((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->SetCursorPosition( idx );
}

void FLW_BorderedMenuDialog::ShowCursor( bool show )
{
	((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->ShowCursor( show );
}

bool FLW_BorderedMenuDialog::IsCursorShown()
{
	return ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->IsCursorShown();
}

FL_OgreFont* FLW_BorderedMenuDialog::GetFontHandle()
{
	return ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->GetFontHandle();
}

void FLW_BorderedMenuDialog::AllowEmptySelection( bool b )
{
	return ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->AllowEmptySelection();
}

/*
	FLW_BorderedVerticalListDialog
*/
FLW_BorderedVerticalListDialog::FLW_BorderedVerticalListDialog(
							string sID, Ogre::Overlay *overlayHandle, FL_OgreFont *fontHandle,
							int rows, int columns, Real borderSize, Real leftPad, Real rightPad )
: FLW_BorderedDialog( sID, overlayHandle, borderSize ), m_LeftPadding(leftPad), m_RightPadding(rightPad)
{
	m_ListTextAreaName = sID + "_MainVListTextArea";
	FLWE_Interface *txtArea = new FLWE_ScrollableMatrixTextArea( m_ListTextAreaName, m_OverlayContainer, fontHandle, rows, columns,
																	0.06f, 0.0f, 0.07f, 0.0f );
																		/*	Make room using right padding
																			for up/down arrows.
																		*/
	txtArea->SetPosition( GetBorderSize_Sides()+m_LeftPadding, GetBorderSize_TopAndBottom() );
	AddElement( txtArea );

	((FLWE_ScrollableMatrixTextArea*)txtArea)->SetDataColumnSize( columns );
	((FLWE_ScrollableMatrixTextArea*)txtArea)->LockCursorH();
}

FLW_BorderedVerticalListDialog::~FLW_BorderedVerticalListDialog()
{
}

FLW_RESULT FLW_BorderedVerticalListDialog::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	FLW_BorderedDialog::Input_PressedOnce( button );	// fills m_ResultStack
	// No meaningful return values from FLW_BorderedDialog because it's only a box with a border.
	FLW_RESULT sendRes = m_ResultStack[m_ListTextAreaName];
	sendRes.id = m_sID;
	// process m_ResultStack[m_ListTextAreaName] to alter sendRes if desired
	/*
	if( button == JOYP_B )
	{
		sendRes.message = "DESTROYME";
	}*/
	return sendRes;
}

void FLW_BorderedVerticalListDialog::LockCursorH( bool lock )
{
	((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->LockCursorH( lock );
}

void FLW_BorderedVerticalListDialog::SetCellWidth( Real width, int index )
{
	((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->SetMatrixTextWidth( width, index );
	// CalculateLocalPositions() called internally by SetMatrixTextWidth().
}

void FLW_BorderedVerticalListDialog::ResizeWindowToShowAll()
{
	((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->ResizeColumnsToWidestText();
	FLW_BorderedDialog::SetDimension( ((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->GetWidth()
										+ GetBorderSize_Sides()*2 + m_LeftPadding + m_RightPadding,
									  ((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->GetHeight()
										+ GetBorderSize_TopAndBottom()*2 );
}

void FLW_BorderedVerticalListDialog::UpdateData()
{
	((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->PushDataTableToMatrix();
	((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->UpdateDataRowSize();
}

void FLW_BorderedVerticalListDialog::PushBackData( std::deque<wstring> *rowData )
{
	for( int i=0;
		i<((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->GetDataColumnSize(); ++i )
	{
		if( i < (int)rowData->size() )
			((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->PushData( (*rowData)[i] );
		else
			((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->PushData( L"" );
	}

	((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->UpdateDataRowSize();
}

void FLW_BorderedVerticalListDialog::ClearList()
{
	((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->ClearDataTable();
}

wstring FLW_BorderedVerticalListDialog::GetDataTableText( int idx )
{
	return ((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->GetDataTableText( idx );
}

int FLW_BorderedVerticalListDialog::GetCursorDataIndex()
{
	return ((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->GetCursorDataIndex();
}

void FLW_BorderedVerticalListDialog::ShowCursor( bool show )
{
	((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->ShowCursor( show );
}

void FLW_BorderedVerticalListDialog::AllowEmptySelection( bool b )
{
	((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->AllowEmptySelection( b );
}

/*======================================================================================
								FL Window ELEMENTS
--------------------------------------------------------------------------------------*/
/*
	FLWE_Interface
*/
FLWE_Interface::~FLWE_Interface()
{
	LOGHI( L"FLWE Element destructor ended. " ); LOGHIVAR( m_sID, true );
}

FLW_RESULT FLWE_Interface::Loop( std::map<string,FLW_RESULT> *resultStack, Real timeSinceLastFrame )
{
	/*
		resultStack is currently unused. It should be NULL.
	*/
	return FLW_RESULT(m_sID);
}

FLW_RESULT FLWE_Interface::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	/*
	std::map<string,FLWE_Interface*>::iterator iter;
	for( iter=m_Children.begin(); iter!=m_Children.end(); ++iter )
		if( iter->second->m_AcceptInput )
			iter->second->Input_PressedOnce( button );
	*/
	return FLW_RESULT(m_sID);
}

/*
	FLWE_Panel
*/
FLWE_Panel::FLWE_Panel( std::string sID, Ogre::OverlayContainer *overlayHandle )
: FLWE_Interface(sID,overlayHandle)
{
	OverlayManager& om = OverlayManager::getSingleton();
	m_PanelElement = static_cast<PanelOverlayElement*>( om.createOverlayElement("Panel", String(sID)) );
	m_PanelElement->setMetricsMode( Ogre::GMM_RELATIVE );
	m_PanelElement->setPosition( 0.0f, 0.0f );
	m_PanelElement->setDimensions( 1.0f, 1.0f );
	//m_PanelElement->setMaterialName( "panelCentre" );
	m_PanelElement->show();
	overlayHandle->addChild( m_PanelElement );	// attach it to an overlay
}

FLWE_Panel::~FLWE_Panel()
{
	OverlayManager::getSingleton().destroyOverlayElement( m_PanelElement );
}

/*
void FLWE_Panel::Loop( std::deque<FLW_RESULT> *resultStack, Real timeSinceLastFrame )
{
}*/

/*
	FLWE_Border
*/
FLWE_Border::FLWE_Border( string sID, Ogre::OverlayContainer *oc, Real borderSize_topAndBottom )
: FLWE_Interface(sID,oc)
{
	OverlayManager& om = OverlayManager::getSingleton();
	m_BorderElement = static_cast<BorderPanelOverlayElement*>( om.createOverlayElement("BorderPanel", String(sID)) );
	m_BorderElement->setMetricsMode( Ogre::GMM_RELATIVE );
	m_BorderElement->setPosition( 0, 0 );
	m_BorderElement->setDimensions( 1.0f, 1.0f );
	m_BorderElement->setMaterialName( "panelCentre" );
	m_BorderElement->setBorderMaterialName( "panelBorder" );
	m_BorderElement->setBorderSize( borderSize_topAndBottom*gDisplayRatio.ratio, borderSize_topAndBottom );
	m_BorderElement->show();

	OVERLAY_SetBorderPanelProperties( m_BorderElement );

	oc->addChild( m_BorderElement );	// attach it to main border
}

FLWE_Border::~FLWE_Border()
{
	OverlayManager::getSingleton().destroyOverlayElement( m_BorderElement );
}

/*
	FLWE_TextArea
*/
FLWE_TextArea::FLWE_TextArea( std::string sID, Ogre::OverlayContainer *overlayHandle, FL_OgreFont *fontHandle )
: FLWE_Interface(sID,overlayHandle), m_FontHandle(fontHandle)
{
	if( NULL == m_FontHandle )
		m_FontHandle = gFontHandle;

	Ogre::DisplayString defaultText(L"No_Text");

	OverlayManager& om = OverlayManager::getSingleton();
	m_TextArea = static_cast<TextAreaOverlayElement*>( om.createOverlayElement("TextArea",sID) );
	m_TextArea->setMetricsMode( Ogre::GMM_RELATIVE );
	m_TextArea->setPosition( 0.0f, 0.0f );
	m_TextArea->setDimensions( m_FontHandle->TextWidth(defaultText), m_FontHandle->GetHeight() );
	m_TextArea->setFontName( m_FontHandle->GetName() );
	m_TextArea->setCharHeight( m_FontHandle->GetHeight() );
	m_TextArea->setColour( ColourValue(1,1,1) );

	m_TextArea->setCaption( defaultText );
	m_TextArea->show();
	overlayHandle->addChild( m_TextArea );
}


FLWE_TextArea::~FLWE_TextArea()
{
	OverlayManager::getSingleton().destroyOverlayElement( m_TextArea );
}

/*
	Multi line Text Area
*/
FLWE_MultiLineTextArea::FLWE_MultiLineTextArea( std::string sID, Ogre::OverlayContainer *overlayHandle,
											   FL_OgreFont *fontHandle, int numLines, Real lineSpacing )
: FLWE_Interface(sID,overlayHandle), m_LineSpacing(lineSpacing), m_FontHandle(fontHandle), m_WordStack(NULL),
	m_Interval(0.02f), m_TimeCounter(0), m_PauseText(false), m_TextLineRow(0)
{
	/*
		Overlay container used to contain all text areas,
		so that relative coordinate of text areas are easy to manage.
		i.e. text area coordinates start from (0,0) to (1,1) in every case.
	*/
	if( NULL == m_FontHandle )
		m_FontHandle = gFontHandle;

	OverlayManager& om = OverlayManager::getSingleton();
	m_OverlayContainer = static_cast<OverlayContainer*>( om.createOverlayElement("Panel", String(sID+"_OverlayContainer")) );
	m_OverlayContainer->setMetricsMode( Ogre::GMM_RELATIVE );
	m_OverlayContainer->setPosition( 0.0f, 0.0f );
	m_OverlayContainer->show();
	overlayHandle->addChild( m_OverlayContainer );

	Ogre::DisplayString defaultText(L"Line_");

	TextAreaOverlayElement *textArea;	// handle

	for( int i=0; i<numLines; ++i )
	{
		textArea = static_cast<TextAreaOverlayElement*>( om.createOverlayElement("TextArea",sID+"_TextLine"+FL::LONG2STRING(i)) );
		textArea->setMetricsMode( Ogre::GMM_RELATIVE );
		textArea->setPosition( 0.0f, 0.0f );
		textArea->setCaption( DisplayString(defaultText+FL::LONG2WSTRING(i)) );
		textArea->setDimensions( 1.0f, m_FontHandle->GetHeight() );
		textArea->setFontName( m_FontHandle->GetName() );
		textArea->setCharHeight( m_FontHandle->GetHeight() );
		textArea->setColour( ColourValue(1,1,1) );

		m_OverlayContainer->addChild( textArea );
		m_TextLines.push_back( textArea );
	}

//	defaultText = L"����";
	defaultText = L"��";
	m_BlinkerText = static_cast<TextAreaOverlayElement*>( om.createOverlayElement("TextArea",sID+"_BlinkerText") );
	m_BlinkerText->setMetricsMode( Ogre::GMM_RELATIVE );
	m_BlinkerText->setCaption( defaultText );
	m_BlinkerText->setDimensions( m_FontHandle->TextWidth(defaultText), m_FontHandle->GetHeight() );
	m_BlinkerText->setPosition( (m_OverlayContainer->getWidth() - m_BlinkerText->getWidth())/2
									, m_OverlayContainer->getTop()
									+ m_OverlayContainer->getHeight()-m_FontHandle->GetHeight() );
	m_BlinkerText->setFontName( m_FontHandle->GetName() );
	m_BlinkerText->setCharHeight( m_FontHandle->GetHeight() );
	m_BlinkerText->setColour( ColourValue(1,1,1) );
	m_BlinkerText->hide();
	m_OverlayContainer->addChild( m_BlinkerText );

	/*
		Default size. Let width be 0.5f and height the sum of the elements above.
	*/
	m_OverlayContainer->setDimensions( 0.5f, m_TextLines.size()*m_FontHandle->GetHeight()
											+ (m_TextLines.size()-1)*m_LineSpacing
											+ m_BlinkerText->getHeight() );
	SetText( L"" );
}

FLWE_MultiLineTextArea::~FLWE_MultiLineTextArea()
{
	if( m_WordStack )
		delete m_WordStack;

	OverlayManager& om = OverlayManager::getSingleton();

	std::deque<TextAreaOverlayElement*>::iterator iter;
	for( iter=m_TextLines.begin(); iter!=m_TextLines.end(); ++iter )
		om.destroyOverlayElement( *iter );

	om.destroyOverlayElement( m_BlinkerText );
	om.destroyOverlayElement( m_OverlayContainer );
}

void FLWE_MultiLineTextArea::SetPosition( Real left, Real top )
{
	m_OverlayContainer->setPosition( left, top );

	int nthRow = 0;
	std::deque<TextAreaOverlayElement*>::iterator iter;
	for( iter=m_TextLines.begin(); iter!=m_TextLines.end(); ++iter )
	{
		if( (*iter)->isVisible() ) {
			(*iter)->setPosition( 0, nthRow*(m_FontHandle->GetHeight()+m_LineSpacing) );
			++nthRow;
		}
		else (*iter)->setPosition( 0, 0 );
	}

	m_BlinkerText->setPosition( (m_OverlayContainer->getWidth()-m_BlinkerText->getWidth())/2.0f,
								m_OverlayContainer->getTop()
									+ m_OverlayContainer->getHeight()-m_FontHandle->GetHeight()
								);
}

void FLWE_MultiLineTextArea::SetDimension( Real width, Real height )
{
	if( width>0 ) m_OverlayContainer->setWidth(width);
	if( height>0 ) m_OverlayContainer->setHeight(height);

	int numRows = _GetSizeVisibleRows();

	std::deque<TextAreaOverlayElement*>::iterator iter;
	for( iter=m_TextLines.begin(); iter!=m_TextLines.end(); ++iter )
	{
		if( numRows > 0 ) {
			--numRows;
			(*iter)->show();
			if( width > 0 )
				(*iter)->setDimensions( width, m_FontHandle->GetHeight() );
		}
		else {
			(*iter)->hide();
		}
	}
	// text areas need to be moved as well when resizing.
	SetPosition( m_OverlayContainer->getLeft(), m_OverlayContainer->getTop() );
}

FLW_RESULT FLWE_MultiLineTextArea::Loop( std::map<string,FLW_RESULT> *resultStack, Real timeSinceLastFrame )
{
	// NOTE how m_TimeCounter is used for both text pushing AND blinker blinking.
	//		as only 1 of them would be happening at any given time.

	/*	Process blinking arrow if necessary...	*/
	if( m_PauseText ) {
		if( m_TimeCounter <= 0 ) {
			m_TimeCounter = 0.5f;
			if( m_BlinkerText->isVisible() )
				m_BlinkerText->hide();
			else
				m_BlinkerText->show();
		}
		else
			m_TimeCounter -= timeSinceLastFrame;
		/*
			IMPORTANT to return here as if process go on, text push will be processed below.
		*/
		return FLW_RESULT(m_sID);
	}
	
	/*
		So text isn't paused. Process text.
	*/
	if( m_TimeCounter <= 0 )
	{
		bool newWord = false;
		m_TimeCounter = m_Interval;
		/* Prepare m_Word */
		if( m_Word.empty() ) {
			if( m_WordStack->HasMoreElements() ) {
				m_Word = *(m_WordStack->GetNext());
				newWord = true;
			}
			else
				return FLW_RESULT(m_sID);	// no more processing to be done.
		}
		/* Figure out which row m_Word should appear in IF it's a new word */
		if( newWord && m_Word[0]!='\t' && m_Word[0]!='\n' ) {
			// New word doesn't mean current text line is filled with other text (eg. newline has been force fed)
			// So check if current text line has text in it or empty.
			if( !m_WordStack->GetSpaceExists() ) {
				// No need to do anything.
				// This language doesn't use spaces as word divider.
			}
			else if( m_TextLines[m_TextLineRow]->getCaption().empty() ) {
				// No need to do anything.
			}
			else {
				DisplayString ds;
				ds = m_TextLines[m_TextLineRow]->getCaption() + L" " + m_Word;	// text exists, so insert space
				if( m_FontHandle->TextWidth( ds ) > m_TextLines[0]->getWidth() )
					_MoveToNextLine();	// takes care of scrolling
				else {
					// add a space as it's still the same row
					DisplayString addspace = m_TextLines[m_TextLineRow]->getCaption() + L" ";
					m_TextLines[m_TextLineRow]->setCaption( addspace );
				}
			}
		}
		/* Check for special symbol */
		if( m_Word[0] == '\t' ) {
			// meaning, wait for action button to be hit.
			_PauseText();
			m_Word.erase( 0, 1 );
		}
		else if( m_Word[0] == '\n' ) {
			_MoveToNextLine();	// force feed newline
			m_Word.erase( 0, 1 );
		}
		else {
			wstring letter;
			letter.push_back( m_Word[0] );
			/* Print a letter from m_Word, BUT check if this letter will make the line shoot off the width.
				This is necessary for languages where space isn't used as a divider.
			*/
			if( m_FontHandle->TextWidth( m_TextLines[m_TextLineRow]->getCaption()+letter )
				> m_TextLines[0]->getWidth() )	{
				_MoveToNextLine();	// takes care of scrolling
			}
			DisplayString appendLetter = m_TextLines[m_TextLineRow]->getCaption() + letter;
			m_TextLines[m_TextLineRow]->setCaption( appendLetter );
			m_Word.erase( 0, 1 );
		}
		/*
			Send report here thru result stack
		*/
		FLW_RESULT sendRes(m_sID);
		sendRes.message = "LETTEROUT";
		return sendRes;	// 1 letter pushed out
	}
	else {
		m_TimeCounter -= timeSinceLastFrame;
	}

	return FLW_RESULT(m_sID);
}

void FLWE_MultiLineTextArea::_MoveToNextLine()
{
	++m_TextLineRow;	// move to next line
	if( m_TextLineRow < (int)m_TextLines.size() ) {
		if( m_TextLines[m_TextLineRow]->isVisible() )
			return;	// no need to scroll
	}
	// So scrolling is needed.
	--m_TextLineRow;
	// Shift text up the sequence.
	TextAreaOverlayElement* prevLine;
	std::deque<TextAreaOverlayElement*>::iterator iter = m_TextLines.begin();
	prevLine = *iter;
	++iter;	// 0th line is purged, so start from 1st line
	for( ; iter!=m_TextLines.end(); ++iter ) {
		prevLine->setCaption( (*iter)->getCaption() );	// put this line into previous line
		(*iter)->setCaption( L"" );	// empty this line
		prevLine = *iter;			// store this text line handle
	}
}

void FLWE_MultiLineTextArea::_PauseText()
{
	m_PauseText = true;
	m_TimeCounter = 0.5f;
	m_BlinkerText->show();
	/* Add line feed only if the current text output is on the last line of text area. */
	if( (m_TextLineRow+1) == _GetSizeVisibleRows() )	// if on last line
	{
		_MoveToNextLine();
		/*	Need to step back up the row here as scrolling is done just to make room
			for the blinker. Next sequence of text should continue as if newline feed
			wasn't added.	*/
		--m_TextLineRow;
	}
}

int FLWE_MultiLineTextArea::_GetSizeVisibleRows()
{
	int numGaps = (int)( m_OverlayContainer->getHeight() / (m_FontHandle->GetHeight()+m_LineSpacing) );
	Real adjustedHeight = m_OverlayContainer->getHeight() - numGaps * m_LineSpacing;
	return (int)( adjustedHeight / m_FontHandle->GetHeight() );
}

void FLWE_MultiLineTextArea::SetText( wstring txt )
{
	ClearText();	// just in case
	m_WordStack = new FL::FL_LexerW( txt, false );
	m_WordStack->Begin();
	if( m_WordStack->HasMoreElements() )
		m_Word = *m_WordStack->GetNext();
	m_TimeCounter = m_Interval;
	m_TextLineRow = 0;	// start on 0th row
}

void FLWE_MultiLineTextArea::ClearText()
{
	SAFE_DELETE( m_WordStack );
	m_Word.clear();
	std::deque<TextAreaOverlayElement*>::iterator iter;
	for( iter=m_TextLines.begin(); iter!=m_TextLines.end(); ++iter )
		(*iter)->setCaption( L"" );
}

FLW_RESULT FLWE_MultiLineTextArea::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	FLW_RESULT result(m_sID);

	if( button == JOYP_A ) {
		if( m_PauseText ) {
			_UnpauseText();
		}
		else if( HasTextPushEnded() ) {
			result.message = "DESTROYME";
		}
	}
	else if( button==JOYP_B && HasTextPushEnded() ) {
		result.message = "DESTROYME";
	}
	else {
		FLWE_Interface::Input_PressedOnce( button );
	}
	return result;
}

/*
*/
FLWE_MatrixTextArea::FLWE_MatrixTextArea( std::string sID, Ogre::OverlayContainer *overlayHandle, FL_OgreFont *fontHandle,
										int rows, int columns, Real lineSpacingH, Real lineSpacingV )
: FLWE_Interface(sID,overlayHandle), m_FontHandle(fontHandle),
	m_CursorPosition(0), m_LockCursorH(false), m_LockCursorV(false),
	m_LineSpacingH(lineSpacingH), m_LineSpacingV(lineSpacingV), m_RowSize(rows), m_ColumnSize(columns)
	, m_bAllowEmptySlotSelection( false )
{
	if( NULL == fontHandle )
		m_FontHandle = fontHandle = gFontHandle;

	/* left padding feature is hidden for now as it doesn't look necessary. */
	m_LeftPadding = 0;
	m_RightPadding = 0;
	m_TopPadding = 0;
	m_BottomPadding = 0;

	Real cursor_height = fontHandle->GetHeight()*1.2f;
	Real cursor_width = cursor_height * gDisplayRatio.ratio;
	/*
				_____________________
				|
				|
				|
				|
				|
				|

		Overlay container used to contain all text areas,
		so that relative coordinate of text areas are easy to manage.
		i.e. text area coordinates start from (0,0) to (1,1) in every case.
		NOTE: width and height are still in screen (monitor) coordinates.
	*/
	OverlayManager& om = OverlayManager::getSingleton();
	m_OverlayContainer = static_cast<OverlayContainer*>( om.createOverlayElement("Panel", String(sID+"_OverlayContainer")) );
	m_OverlayContainer->setMetricsMode( Ogre::GMM_RELATIVE );
	m_OverlayContainer->setPosition( 0.0f, 0.0f );
	m_OverlayContainer->setDimensions( 1.0f, 1.0f );
	m_OverlayContainer->show();
	overlayHandle->addChild( m_OverlayContainer );

	Ogre::DisplayString defaultText(L"MAT_");
	TextAreaOverlayElement *textArea;	// handle
	int index = 0;

	for( int y=0; y<m_RowSize; ++y )	{
		for( int x=0; x<m_ColumnSize; ++x )
		{
			DisplayString ds = (defaultText+FL::LONG2WSTRING(index));
			textArea = static_cast<TextAreaOverlayElement*>( om.createOverlayElement("TextArea",sID+"_MatrixText"+FL::LONG2STRING(index)) );
			textArea->setMetricsMode( Ogre::GMM_RELATIVE );
			textArea->setPosition( 0.02f*(Real)x, 0.0f*fontHandle->GetHeight() );
			textArea->setCaption( ds );
			textArea->setDimensions( fontHandle->TextWidth(ds), fontHandle->GetHeight() );
			textArea->setFontName( fontHandle->GetName() );
			textArea->setCharHeight( fontHandle->GetHeight() );
			textArea->setColour( ColourValue(1,1,1) );

			m_OverlayContainer->addChild( textArea );
	//		overlayHandle->addChild( textArea );
			m_MatrixText.push_back( textArea );
			++index;
		}
	}
	m_OverlayContainer->setDimensions( GetWidth(), GetHeight() );

	m_CursorPanel = static_cast<BorderPanelOverlayElement*>
					( om.createOverlayElement("Panel", String(sID+"_MatTxtCursor")) );
	m_CursorPanel->setMetricsMode( Ogre::GMM_RELATIVE );
	m_CursorPanel->setMaterialName( "HandCursor" );
	m_CursorPanel->setDimensions( cursor_width, cursor_height );
	m_CursorPanel->setPosition( 0.0f, 0.0f );
	m_CursorPanel->show();
	m_OverlayContainer->addChild( m_CursorPanel );
}

FLWE_MatrixTextArea::~FLWE_MatrixTextArea()
{
	OverlayManager& om = OverlayManager::getSingleton();

	std::deque<TextAreaOverlayElement*>::iterator iter;
	for( iter=m_MatrixText.begin(); iter!=m_MatrixText.end(); ++iter )
		om.destroyOverlayElement( *iter );

	om.destroyOverlayElement( m_CursorPanel );
	om.destroyOverlayElement( m_OverlayContainer );
}

/*
FLW_RESULT FLWE_MatrixTextArea::Loop( std::map<string,FLW_RESULT> *resultStack, Real timeSinceLastFrame )
{
}*/

void FLWE_MatrixTextArea::SetPosition( Real left, Real top )
{
	m_OverlayContainer->setPosition( left, top );
}

/*
void FLWE_MatrixTextArea::SetDimension( Real width, Real height )
{
	//m_OverlayContainer->setDimensions( width, height );
}*/

FLW_RESULT FLWE_MatrixTextArea::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	FLW_RESULT res( m_sID );

	switch( button )
	{
	case JOYP_UP:
		if( m_LockCursorV || !m_CursorPanel->isVisible() )
			break;
		_MoveCursorBy1V( true );
		res.message = "MOVED"; res.i = m_CursorPosition;
		break;
	case JOYP_DOWN:
		if( m_LockCursorV || !m_CursorPanel->isVisible() )
			break;
		_MoveCursorBy1V( false );
		res.message = "MOVED"; res.i = m_CursorPosition;
		break;
	case JOYP_RIGHT:
		if( m_LockCursorH || !m_CursorPanel->isVisible() )
			break;
		_MoveCursorBy1H( false );
		res.message = "MOVED"; res.i = m_CursorPosition;
		break;
	case JOYP_LEFT:
		if( m_LockCursorH || !m_CursorPanel->isVisible() )
			break;
		_MoveCursorBy1H( true );
		res.message = "MOVED"; res.i = m_CursorPosition;
		break;
	case JOYP_A:
		res.message = "CHOICE";
		res.i = m_CursorPosition;
		break;
	}
	return res;
}

void FLWE_MatrixTextArea::SetText( wstring txt, int index )
{
	if( index < (int)m_MatrixText.size() )
	{
		m_MatrixText[index]->setCaption( txt );
		m_MatrixText[index]->setWidth( m_FontHandle->TextWidth(txt) );
	}
}

wstring FLWE_MatrixTextArea::GetText( int index )
{
	if( index >= (int)m_MatrixText.size() )
		return wstring();
	return m_MatrixText[index]->getCaption();
}

void FLWE_MatrixTextArea::SetCursorPosition( int index )
{
	if( index >= 0 )
		m_CursorPosition = index;

	Real left = m_MatrixText[m_CursorPosition]->getLeft();
	Real top = m_MatrixText[m_CursorPosition]->getTop();
	/*
		Ajudst (left,top) coordinate to cursor's hotspot.
	*/
	left -= m_CursorPanel->getWidth();

	m_CursorPanel->setPosition( left, top );
}

void FLWE_MatrixTextArea::SetMatrixTextWidth( Real width, int index )
{
	if( index == -1 ) {	// change ALL text area widths
		std::deque<TextAreaOverlayElement*>::iterator iter;
		for( iter=m_MatrixText.begin(); iter!=m_MatrixText.end(); ++iter )
			(*iter)->setWidth( width );
	}
	else {	// change width of the column in which index resides
		for( unsigned int i=index; i<m_MatrixText.size(); i+=m_ColumnSize )
			m_MatrixText[i]->setWidth( width );
	}

	// Recalculate m_OverlayContainer
	//m_OverlayContainer->setWidth( GetWidth() );

	CalculateLocalPositions();
}

Real FLWE_MatrixTextArea::GetWidth()
{
	// Account for left & right padding.
	Real tWidth = m_LeftPadding + m_RightPadding;
	// Account for text widths.
	for( int x=0; x<m_ColumnSize; ++x )
		tWidth += m_MatrixText[x]->getWidth();
	// Account for spacing between text.
	tWidth += (m_ColumnSize-1)*m_LineSpacingH;

	return tWidth;
}

Real FLWE_MatrixTextArea::GetHeight()
{
	Real height = 0;
	height += m_TopPadding + m_BottomPadding;	// padding
	height += (m_RowSize-1) * m_LineSpacingV;	// spacing
	for( int y=0; y<m_RowSize; ++y )
		height += m_MatrixText[y*m_ColumnSize]->getHeight();	// cell width

	return height;
}

Real FLWE_MatrixTextArea::ResizeColumnsToWidestText()
{
	// Find widest text for each column
	Real textWidth;
	std::deque<Real>	widest(m_ColumnSize,0);
	int index = 0;
	for( int y=0; y<m_RowSize; ++y ) {
		for( int x=0; x<m_ColumnSize; ++x ) {
			textWidth = m_FontHandle->TextWidth( m_MatrixText[index]->getCaption() );
			if( widest[x] < textWidth )	// if wider than current record...
				widest[x] = textWidth;
			++index;
		}
	}
	// Set all text areas according to the recorded widest size of each column
	index = 0;
	for( int y=0; y<m_RowSize; ++y ) {
		for( int x=0; x<m_ColumnSize; ++x ) {
			m_MatrixText[index]->setWidth( widest[x] );
			++index;
		}
	}

	Real totalWidth = GetWidth();

	// Finally resize overlay in which matrix texts reside.
	m_OverlayContainer->setDimensions( totalWidth, GetHeight() );

	// Recalculate text positions etc.
	CalculateLocalPositions();
	//SetPosition( m_OverlayContainer->getLeft(), m_OverlayContainer->getTop() );

	return totalWidth;
}

void FLWE_MatrixTextArea::CalculateLocalPositions()
{
	// Matrix text area positioning
	//Real leftPadding = m_CursorPanel->getWidth();
	Real leftPadding = m_LeftPadding;
	Real xOffset;
	int index = 0;

	for( int y=0; y<m_RowSize; ++y ) {
		xOffset = leftPadding;
		for( int x=0; x<m_ColumnSize; ++x )
		{
			m_MatrixText[index]->setPosition( xOffset+x*m_LineSpacingH,
												m_TopPadding+y*(m_FontHandle->GetHeight()+m_LineSpacingV) );
			xOffset += m_MatrixText[index]->getWidth();
			++index;
		}
	}
	// Recalculate cursor positioning
	SetCursorPosition();
	// Ensure Overlay container is of correct size
	m_OverlayContainer->setDimensions( GetWidth(), GetHeight() );
}

void FLWE_MatrixTextArea::_MoveCursorBy1H( bool negative )
{
	int newV = m_CursorPosition;
	// Find nth row in which cursor resides.
	int nthRow = (int)( (Real)newV / (Real)m_ColumnSize );
	int lower = nthRow * m_ColumnSize;
	int upper = lower + m_ColumnSize - 1;
	for( int i=0; i<m_ColumnSize; ++i )
	{
		// byX increment or decrement
		if( negative ) --newV;
		else ++newV;
		// bounds control
		if( newV < lower )
			newV = upper;	// wrap around
		else if( newV > upper )
			newV = lower;	// wrap around
		// check if this text element is active
		if( !m_MatrixText[newV]->getCaption().empty() || m_bAllowEmptySlotSelection ) {
			m_CursorPosition = newV;
			break;
		}
	}
	SetCursorPosition();
}

void FLWE_MatrixTextArea::_MoveCursorBy1V( bool negative )
{
	int newV = m_CursorPosition;
	/*
		Logic to find Nth column in which cursor resides.
	*/
	int nthCol = (newV+m_ColumnSize) % m_ColumnSize;
	int lower = nthCol;
	int upper = lower + (m_RowSize-1)*m_ColumnSize;
	for( int i=0; i<m_RowSize; ++i )
	{
		// byX increment or decrement
		if( negative ) newV -= m_ColumnSize;
		else newV += m_ColumnSize;
		// bounds control
		if( newV < lower )
			newV = upper;	// wrap around
		else if( newV > upper )
			newV = lower;	// wrap around
		// check if this text element is active
		if( !m_MatrixText[newV]->getCaption().empty() || m_bAllowEmptySlotSelection ) {
			m_CursorPosition = newV;
			break;
		}
	}
	SetCursorPosition();
}


/*---------------------------------------------------

		Scrollable version of Matrix Text Area

*/
FLWE_ScrollableMatrixTextArea::FLWE_ScrollableMatrixTextArea( std::string sID, Ogre::OverlayContainer *overlayHandle,
															 FL_OgreFont *fontHandle,
													int rows, int columns, Real lineSpacingH, Real lineSpacingV,
													Real rightPad, Real bottomPad )
: FLWE_MatrixTextArea(sID,overlayHandle,fontHandle,rows,columns,lineSpacingH,lineSpacingV ),
	m_DataColumnSize(0), m_DataRowSize(0), m_DataPosition(0)
{
	// Use padding to make room for scroll arrows.
	m_RightPadding = rightPad;
	m_BottomPadding = bottomPad;

	OverlayManager& om = OverlayManager::getSingleton();

	// Create arrow panels
	m_UpArrowPanel = static_cast<BorderPanelOverlayElement*>( om.createOverlayElement("Panel", String(sID+"_UpArrow")) );
	m_UpArrowPanel->setMetricsMode( Ogre::GMM_RELATIVE );
	m_UpArrowPanel->setMaterialName( "uparrow" );
	m_UpArrowPanel->setDimensions( 0.05f*gDisplayRatio.ratio, 0.05f );	// arrow size
	m_UpArrowPanel->hide();
	m_OverlayContainer->addChild( m_UpArrowPanel );

	m_DownArrowPanel = static_cast<BorderPanelOverlayElement*>( om.createOverlayElement("Panel", String(sID+"_DownArrow")) );
	m_DownArrowPanel->setMetricsMode( Ogre::GMM_RELATIVE );
	m_DownArrowPanel->setMaterialName( "downarrow" );
	m_DownArrowPanel->setDimensions( 0.05f*gDisplayRatio.ratio, 0.05f );	// arrow size
	m_DownArrowPanel->hide();
	m_OverlayContainer->addChild( m_DownArrowPanel );

	m_RightArrowPanel = static_cast<BorderPanelOverlayElement*>( om.createOverlayElement("Panel", String(sID+"_RightArrow")) );
	m_RightArrowPanel->setMetricsMode( Ogre::GMM_RELATIVE );
	m_RightArrowPanel->setMaterialName( "rightarrow" );
	m_RightArrowPanel->setDimensions( 0.05f*gDisplayRatio.ratio, 0.05f );	// arrow size
	m_RightArrowPanel->hide();
	m_OverlayContainer->addChild( m_RightArrowPanel );

	m_LeftArrowPanel = static_cast<BorderPanelOverlayElement*>( om.createOverlayElement("Panel", String(sID+"_LeftArrow")) );
	m_LeftArrowPanel->setMetricsMode( Ogre::GMM_RELATIVE );
	m_LeftArrowPanel->setMaterialName( "leftarrow" );
	m_LeftArrowPanel->setDimensions( 0.05f*gDisplayRatio.ratio, 0.05f );	// arrow size
	m_LeftArrowPanel->hide();
	m_OverlayContainer->addChild( m_LeftArrowPanel );
}

FLWE_ScrollableMatrixTextArea::~FLWE_ScrollableMatrixTextArea()
{
	OverlayManager& om = OverlayManager::getSingleton();

	om.destroyOverlayElement( m_UpArrowPanel );
	om.destroyOverlayElement( m_DownArrowPanel );
	om.destroyOverlayElement( m_RightArrowPanel );
	om.destroyOverlayElement( m_LeftArrowPanel );
}

FLW_RESULT FLWE_ScrollableMatrixTextArea::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	FLW_RESULT res(m_sID);

	switch( button )
	{
	case JOYP_UP:
	case JOYP_DOWN:
		res = FLWE_MatrixTextArea::Input_PressedOnce( button );
		break;
	case JOYP_RIGHT:
		if( m_LockCursorH || !m_CursorPanel->isVisible() )
			break;
		res = FLWE_MatrixTextArea::Input_PressedOnce( button );
		break;
	case JOYP_LEFT:
		if( m_LockCursorH || !m_CursorPanel->isVisible() )
			break;
		res = FLWE_MatrixTextArea::Input_PressedOnce( button );
		break;
	case JOYP_A:
		res.message = "CHOICE";
		res.i = GetCursorDataIndex();
		break;
	}
	res.id = m_sID;
	return res;
}

int FLWE_ScrollableMatrixTextArea::GetCursorDataIndex()
{
	int viewX = m_CursorPosition % m_ColumnSize;
	int viewY = m_CursorPosition / m_ColumnSize;
	int dataX = m_DataPosition % m_DataColumnSize;
	int dataY = m_DataPosition / m_DataColumnSize;

	int finalX = viewX + dataX;
	int finalY = viewY + dataY;

	return finalY * m_DataColumnSize + finalX;
}

/*
void FLWE_ScrollableMatrixTextArea::_MoveCursorBy1H( bool negative )
{
	int newV = m_CursorPosition;
	// Find nth row in which cursor resides.
	int nthRow = (int)( (Real)newV / (Real)m_ColumnSize );
	int lower = nthRow * m_ColumnSize;
	int upper = lower + m_ColumnSize - 1;
	for( int i=0; i<m_ColumnSize; ++i )
	{
		// byX increment or decrement
		if( negative ) --newV;
		else ++newV;
		// bounds control
		if( newV < lower )
			newV = upper;	// wrap around
		else if( newV > upper )
			newV = lower;	// wrap around
		// check if this text element is active
		if( !m_MatrixText[newV]->getCaption().empty() ) {
			m_CursorPosition = newV;
			break;
		}
	}
	SetCursorPosition();
}*/

void FLWE_ScrollableMatrixTextArea::_MoveCursorBy1V( bool negative )
{
	int newV = m_CursorPosition;
	/*
		Logic to find Nth column in which cursor resides.
	*/
	int nthCol = (newV+m_ColumnSize) % m_ColumnSize;
	int lower = nthCol;
	int upper = lower + (m_RowSize-1)*m_ColumnSize;
	for( int i=0; i<m_DataRowSize; ++i )
	{
		// increment or decrement
		if( negative ) newV -= m_ColumnSize;
		else newV += m_ColumnSize;
		/*
			Bounds control
			IF:	Hit the ceiling, check if it can scroll up.
				Hit the bottom, check if it can scroll down.
				If not, wrap around up or down respectively.
		*/
		if( newV < lower ) {	// Hit the Ceiling...
			if( m_DataRowSize <= m_RowSize ) {	// If Data table fits within view window...
				newV = upper;	// wrap around the cursor
			} else {	// If Data table is taller than view window...
				if( 0==(m_DataPosition/m_DataColumnSize) ) {	// If already at the top of the data table...
					newV = upper;	// wrap around the cursor
					// Wrap around to the end of the data table.
					SetDataPosition( m_DataColumnSize * (m_DataRowSize-m_RowSize) + m_DataPosition );
				}
				else {
					// Keep cursor where it is (at m_CursorPosition), but scroll up the data table.
					newV = m_CursorPosition;
					SetDataPosition( m_DataPosition - m_DataColumnSize );
				}
			}
		}
		else if( newV > upper ) {	// Hit the Bottom...
			if( m_DataRowSize <= m_RowSize ) {	// If Data table fits within view window...
				newV = lower;	// wrap around the cursor
			} else {	// If Data table is taller than view window...
				if( (m_DataRowSize-m_RowSize)==(m_DataPosition/m_DataColumnSize) ) {	// If already at the bottom of the data table...
					newV = lower;	// wrap around the cursor
					// Wrap around to the top of the data table.
					SetDataPosition( m_DataPosition % m_ColumnSize );
				}
				else {
					// Keep cursor where it is (at m_CursorPosition), but scroll down the data table.
					newV = m_CursorPosition;
					SetDataPosition( m_DataPosition + m_DataColumnSize );
				}
			}
		}
		if( m_bAllowEmptySlotSelection ) {
			m_CursorPosition = newV;	// Finally, move the cursor.
			break;						// Cursor moved, so break from loop.
		}
		else {
			// Is the intended new location within the bounds of data table?
			if( (newV+m_DataPosition) < (int)m_DataTable.size() ) {
				// check if text exists in this element of the data table
				if( !m_DataTable[newV+m_DataPosition].empty() ) {
					m_CursorPosition = newV;	// Finally, move the cursor.
					break;						// Cursor moved, so break from loop.
				}
			}
		}
	}
	SetCursorPosition();
}

int FLWE_ScrollableMatrixTextArea::SetDataPosition( int newPos )
{
	if( m_DataPosition == newPos )
		return m_DataPosition; // no difference
	else if( newPos < 0 )	// newPos can be -1 in which case just update view without moving around.
		newPos = m_DataPosition;	// i.e. do not alter but just update view

	//	Bounds Check - don't scroll off the data table
	int xPos = newPos % m_DataColumnSize;
	int yPos = newPos / m_DataColumnSize;
	if( (xPos+m_ColumnSize) > m_DataColumnSize || (yPos+m_RowSize) > m_DataRowSize )
		return m_DataPosition; // out of bounds.

	m_DataPosition = newPos;

	// UPDATE data
	PushDataTableToMatrix();

	return m_DataPosition;
}

void FLWE_ScrollableMatrixTextArea::_UpdateArrowsVisibility()
{
	if( m_DataColumnSize<=0 || m_DataRowSize<=0 ) {	// does data even exist?
		m_UpArrowPanel->hide();
		m_DownArrowPanel->hide();
		m_RightArrowPanel->hide();
		m_LeftArrowPanel->hide();
		return;
	}

	int xPos = m_DataPosition % m_DataColumnSize;
	int yPos = m_DataPosition / m_DataColumnSize;

	//		[ ROWS ]
	if( m_DataRowSize <= m_RowSize )
		m_UpArrowPanel->hide();
	else if( yPos == 0 ) {			// At the top row?
		m_UpArrowPanel->hide(); // up arrow not needed
	} else {
		m_UpArrowPanel->show();
	}
	if( m_DataRowSize <= m_RowSize )
		m_DownArrowPanel->hide();
	else if( yPos == (m_DataRowSize-m_RowSize) ) {	// At the bottom of the data table?
		m_DownArrowPanel->hide(); // down arrow not needed
	} else {
		m_DownArrowPanel->show();
	}

	//		[ COLUMNS ]
	if( m_DataColumnSize <= m_ColumnSize )
		m_LeftArrowPanel->hide();
	else if( xPos == 0 ) {				// At the left of the data table?
		m_LeftArrowPanel->hide();	// left arrow not needed
	} else {
		m_LeftArrowPanel->show();
	}
	if( m_DataColumnSize <= m_ColumnSize )
		m_RightArrowPanel->hide();
	else if( xPos == (m_DataColumnSize-m_ColumnSize) ) {	// At the right of the data table?
		m_RightArrowPanel->hide();	// right arrow not needed
	} else {
		m_RightArrowPanel->show();
	}
}

void FLWE_ScrollableMatrixTextArea::_UpdateArrowsLocation()
{
	Real width = m_OverlayContainer->getWidth();
	Real height = m_OverlayContainer->getHeight();

	m_UpArrowPanel->setPosition( width-m_UpArrowPanel->getWidth(), m_TopPadding );
	m_DownArrowPanel->setPosition( width-m_UpArrowPanel->getWidth(),
									height-m_BottomPadding-m_UpArrowPanel->getHeight() );
	m_LeftArrowPanel->setPosition( m_LeftPadding, height-m_LeftArrowPanel->getHeight() );
	m_RightArrowPanel->setPosition( width-m_RightPadding-m_RightArrowPanel->getWidth(),
									height-m_LeftArrowPanel->getHeight() );
}

void FLWE_ScrollableMatrixTextArea::PushDataTableToMatrix()
{
	for( int y=0; y<m_RowSize; ++y ) {
		for( int x=0; x<m_ColumnSize; ++x ) {
			unsigned int idx = y * m_ColumnSize + x;
			if( idx < m_MatrixText.size() ) {
				unsigned int di = m_DataPosition + x + y * m_DataColumnSize;
				if( di < m_DataTable.size() )
					m_MatrixText[idx]->setCaption( m_DataTable[di] );
				else
					m_MatrixText[idx]->setCaption( L"" );
			}
		}
	}

	//	UPDATE arrows visibility
	_UpdateArrowsVisibility();
}

void FLWE_ScrollableMatrixTextArea::SetMatrixTextWidth( Real width, int index )
{
	FLWE_MatrixTextArea::SetMatrixTextWidth( width, index );
	_UpdateArrowsLocation();
}

Real FLWE_ScrollableMatrixTextArea::ResizeColumnsToWidestText()
{
	Real width = FLWE_MatrixTextArea::ResizeColumnsToWidestText();
	_UpdateArrowsLocation();
	return width;
}

/*---------------------------
		Dialog Stack
---------------------------*/
FL_DialogStack::~FL_DialogStack()
{
	while( !m_DialogStack.empty() ) {
		//delete m_DialogStack.back();
		m_DialogStack.pop_back();
	}
}

void FL_DialogStack::Loop( Real timeSinceLastFrame )
{
	if( m_DialogStack.empty() )
		return;

	for( std::deque<FLW_Dialog*>::iterator iter=m_DialogStack.begin(); iter!=m_DialogStack.end(); ++iter ) {
		FLW_RESULT r = (*iter)->Loop( timeSinceLastFrame );
		if( !r.message.empty() )
			m_ResultStack.push_back( r );
	}
}

FLW_RESULT FL_DialogStack::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	if( !m_DialogStack.empty() ) {
		FLW_RESULT r = m_DialogStack.back()->Input_PressedOnce( button );
		if( !r.message.empty() )
			m_ResultStack.push_back( r );
		return r;
	}
	return FLW_RESULT();
}

void FL_DialogStack::Push( FLW_Dialog *handle )
{
	m_DialogStack.push_back( handle );
}

void FL_DialogStack::PopBack()
{
	m_DialogStack.pop_back();
}

void FL_DialogStack::PopBackAndHide()
{
	m_DialogStack.back()->Show( false );
	m_DialogStack.pop_back();
}

void FL_DialogStack::PopAndDelete()
{
	if( !m_DialogStack.empty() ) {
		DELETE_NULL( m_DialogStack.back() );
		m_DialogStack.pop_back();
	}
}

