#include <rpg_gui_battle.h>
#include <FL_General.h>



RPGD_BattleMenu::RPGD_BattleMenu( const string& id )
	: FLW_BorderedMenuDialog( id, NULL, NULL, BMENU_ROWSIZE, 1 )
	, m_FocusMenuID( BMENU_CENTRE )
{
	AllowEmptySelection();

	m_ComIDList[BMENU_LEFT].resize( BMENU_ROWSIZE );
	m_ComIDList[BMENU_CENTRE].resize( BMENU_ROWSIZE );
	m_ComIDList[BMENU_RIGHT].resize( BMENU_ROWSIZE );

	Real width = 0.18f;

	SetMatrixTextWidth( width );
	FitBorderToTextArea();

	// Create Left menu & Right menu.
	//FLW_BorderedMenuDialog		*m_LeftMenu, *m_RightMenu;
	m_LeftMenu = new FLW_BorderedMenuDialog( id+"_LeftMenu", NULL, NULL, BMENU_ROWSIZE, 1 );
	m_LeftMenu->SetMatrixTextWidth( width );
	m_LeftMenu->FitBorderToTextArea();
	m_LeftMenu->Show( false );
	m_LeftMenu->SetZOrder( GetZOrder()+1 );

	m_RightMenu = new FLW_BorderedMenuDialog( id+"_RightMenu", NULL, NULL, BMENU_ROWSIZE, 1 );
	m_RightMenu->SetMatrixTextWidth( width );
	m_RightMenu->FitBorderToTextArea();
	m_RightMenu->Show( false );
	m_RightMenu->SetZOrder( GetZOrder()+1 );

	AlignWindowCenter();
	SetPosition( GetLeft(), GetTop() );		// positions children too.
}

RPGD_BattleMenu::~RPGD_BattleMenu()
{
	delete m_LeftMenu;
	delete m_RightMenu;
}

FLW_RESULT RPGD_BattleMenu::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	FLW_RESULT res;

	if( button == JOYP_B ) {
		res.message = "CANCEL";
		res.id = m_sID;
		return res;
	}

	switch( m_FocusMenuID )
	{
	case BMENU_CENTRE:
		if( button == JOYP_RIGHT )
			MoveToRightMenu( GetCursorPosition() );
		else if( button == JOYP_LEFT )
			MoveToLeftMenu( GetCursorPosition() );
		else
			res = FLW_BorderedMenuDialog::Input_PressedOnce( button );
		break;
	case BMENU_LEFT:
		if( button == JOYP_RIGHT )
			MoveToCentreMenu( GetCursorPosition() );
		else
			res = m_LeftMenu->Input_PressedOnce( button );
		break;
	case BMENU_RIGHT:
		if( button == JOYP_LEFT )
			MoveToCentreMenu( GetCursorPosition() );
		else
			res = m_RightMenu->Input_PressedOnce( button );
		break;
	}

	if( res.message == "CHOICE" ) {
		// convert menu index into command ID
		res.s = m_ComIDList[m_FocusMenuID][res.i];
		if( res.s.empty() )			// IF no command...
			res.message.clear();	// don't register CHOICE
	}
	res.id = m_sID;
	return res;
}

void RPGD_BattleMenu::SetPosition( Real left, Real top )
{
	Real offset = GetWidth() / 2;

	FLW_BorderedMenuDialog::SetPosition( left, top );
	m_LeftMenu->SetPosition( GetLeft()-offset, GetTop() );
	m_RightMenu->SetPosition( GetLeft()+offset, GetTop() );
}

void RPGD_BattleMenu::SetZOrder( unsigned short z )
{
	FLW_BorderedMenuDialog::SetZOrder( z );
	m_LeftMenu->SetZOrder( z+5 );
	m_RightMenu->SetZOrder( z+5 );
}

void RPGD_BattleMenu::SetText_Left( std::deque<string> id_list )
{
	auto iter = id_list.begin();
	for( int i=0; i<BMENU_ROWSIZE; ++i ) {
		if( iter != id_list.end() ) {
			m_LeftMenu->SetText( gStringTable[*iter], i );
			++iter;
		}
		else
			m_LeftMenu->SetText( L"", i );
	}
	m_ComIDList[BMENU_LEFT] = id_list;
}

void RPGD_BattleMenu::SetText_Centre( std::deque<string> id_list )
{
	auto iter = id_list.begin();
	for( int i=0; i<BMENU_ROWSIZE; ++i ) {
		if( iter != id_list.end() ) {
			SetText( gStringTable[*iter], i );
			++iter;
		}
		else
			SetText( L"", i );
	}
	m_ComIDList[BMENU_CENTRE] = id_list;
}

void RPGD_BattleMenu::SetText_Right( std::deque<string> id_list )
{
	auto iter = id_list.begin();
	for( int i=0; i<BMENU_ROWSIZE; ++i ) {
		if( iter != id_list.end() ) {
			m_RightMenu->SetText( gStringTable[*iter], i );
			++iter;
		}
		else
			m_RightMenu->SetText( L"", i );
	}
	m_ComIDList[BMENU_RIGHT] = id_list;
}

void RPGD_BattleMenu::MoveToLeftMenu( int nth_row )
{
	m_FocusMenuID = BMENU_LEFT;

	// Visibility
	Show();
	m_LeftMenu->Show();
	m_RightMenu->Show( false );

	// Cursors
	ShowCursor( false );
	m_LeftMenu->ShowCursor();
	m_RightMenu->ShowCursor( false );
}

void RPGD_BattleMenu::MoveToCentreMenu( int nth_row )
{
	m_FocusMenuID = BMENU_CENTRE;

	// Visibility
	Show();
	m_LeftMenu->Show( false );
	m_RightMenu->Show( false );

	// Cursors
	ShowCursor();
	m_LeftMenu->ShowCursor( false );
	m_RightMenu->ShowCursor( false );
}

void RPGD_BattleMenu::MoveToRightMenu( int nth_row )
{
	m_FocusMenuID = BMENU_RIGHT;

	// Visiblity
	Show();
	m_LeftMenu->Show( false );
	m_RightMenu->Show();

	// Cursors
	ShowCursor( false );
	m_LeftMenu->ShowCursor( false );
	m_RightMenu->ShowCursor();
}


