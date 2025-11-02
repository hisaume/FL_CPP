#ifndef __RPG_GUI_BATTLE_H__
#define __RPG_GUI_BATTLE_H__
/*-----------------------------------------------------------------
|	|FILE|:		rpg_gui_battle.h
|
|	FL RPG Library
|	version 1.12
|------------------------------------------------------------------
	|NOTES|:

-----------------------------------------------------------------*/
#include <FLO_Window.h>


/*
		L.Menu		 CENTRE.Menu		R.Menu
			|-------|----------|------------|
			|	<---| command1 |--->		|
			|		| command2 |			|
			|		| command3 |			|
			|		| command4 |			|
			|		| command5 |			|
			|-------|----------|------------|

	Paged Battle Menu. Contains Left, Centre and Right menu.
	Default (parent menu) is the CENTRE menu.
	For right/left menus, child menu is used.
*/
#define BMENU_ROWSIZE	5
#define BMENU_LEFT		0
#define	BMENU_CENTRE	1
#define BMENU_RIGHT		2

class RPGD_BattleMenu : public FLW_BorderedMenuDialog
{
public:
	RPGD_BattleMenu( const string& id );
	~RPGD_BattleMenu();
	//virtual FLW_RESULT Loop( Real seconds );
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	void SetPosition( Real left, Real top );
	void SetZOrder( unsigned short z );

	void SetText_Left( std::deque<string> id_list );
	void SetText_Centre( std::deque<string> id_list );
	void SetText_Right( std::deque<string> id_list );
	void MoveToLeftMenu( int nth_row );
	void MoveToCentreMenu( int nth_row );
	void MoveToRightMenu( int nth_row );

protected:
	//void IsLeftEmpty();
	//void IsRightEmpty();

protected:
	FLW_BorderedMenuDialog		*m_LeftMenu, *m_RightMenu;
	std::deque<string>			m_ComIDList[BMENU_RIGHT+1];
	int							m_FocusMenuID;
};


/*
	Status Window for Battle - displayed next to command list?

	|-------------------------------------------------------|
	|														|
	|														|
	|														|
	|														|
	|														|
	|-------------------------------------------------------|
*/





/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif //__RPG_GUI_BATTLE_H__