#ifndef	__RPG_GUI_H__
#define __RPG_GUI_H__
/*-----------------------------------------------------------------
|	|FILE|:		rpg_gui.h
|
|	FL RPG Library
|	version 1.12
|------------------------------------------------------------------
	|NOTES|:

-----------------------------------------------------------------*/
#include <FLO_Window.h>
#include <rpg_data_basic.h>
#include <rpg_state.h>


/*------------------------------------------------------------------

		Smallest Components used by stems of Player Menu

------------------------------------------------------------------*/
/*
	Lists names. Has the option of displaying "All" at the end.

		name1
		name2
		...
		name5
		All

	This is also the base class of many dialogs.
	Many dialogs are essentially a name list with an extra window
	for more information involving the chosen character.
*/
class RPGD_NameList : public FLW_BorderedMenuDialog //FLW_BorderedVerticalListDialog
{
public:
	RPGD_NameList( const string& id, std::deque<string> *name_list, bool show_AllEntry );
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );

protected:
	bool m_bAllEntry;		// display "All" at the end of the list.
	std::deque<string>	m_NameList;
};

/*
	Inventory Action:	Use, Give, Drop
						つかう　わたす　すてる
*/
class RPGD_InventoryAction_Give; // forward declare
class RPGD_InventoryActionDialog : public FLW_BorderedMenuDialog
{
public:
	RPGD_InventoryActionDialog( const string& id, std::deque<string> *name_list );
	~RPGD_InventoryActionDialog();
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
protected:
	std::deque<string>	m_NameList;
	RPGD_InventoryAction_Give *m_InventoryAction_Give;
};
/*
	Lists inventory of a given character.

		Item - Quantity
*/
class RPGD_InventoryListDialog : public FLW_BorderedMenuDialog
{
public:
	RPGD_InventoryListDialog( const string& id, RPG_INVENTORY *inventory, std::deque<string> *namelist );
	FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	void SetTextFromInventory();
	bool Empty()					{ return m_InventoryHandle->Empty(); }
protected:
	int							m_SlotIndex;
	std::deque<string>			m_NameList;
	RPG_INVENTORY				*m_InventoryHandle;
	RPGD_InventoryActionDialog	*m_InventoryActionDialog;
};

class RPGD_PartyInventoryListDialog : public FLW_BorderedVerticalListDialog
{
public:
	RPGD_PartyInventoryListDialog( const string& id, RPG_INVENTORY *inventory, std::deque<string> *namelist );
	~RPGD_PartyInventoryListDialog();
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	virtual void SetTextFromInventory();
	bool Empty()					{ return m_InventoryHandle->Empty(); }
protected:
	int							m_SlotIndex;
	std::deque<string>			m_NameList;
	RPG_INVENTORY				*m_InventoryHandle;
	RPGD_InventoryActionDialog	*m_InventoryActionDialog;
};

/*		|------------|
		| そうび	 |
		| はずす	 |
		| さいきょう |	<-- Not in place for now
		|------------|
*/
class RPGD_EquipmentActionDialog : public FLW_BorderedMenuDialog
{
public:
	RPGD_EquipmentActionDialog( const string& id );
	//~RPGD_EquipmentActionDialog();
	FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );

protected:
	string m_CurrentItem;
};

/*		|-----------|
		| S.Sword	|	(name)
		|			|	(atk/def)
		|			|	(other stats1)
		|			|	(other stats2)
		|-----------|
*/
class RPGD_EquipmentStatsDialog : public FLW_BorderedMenuDialog
{
public:
	RPGD_EquipmentStatsDialog( const string& id, const string& item_id );
	virtual void FilloutDialog();
	virtual void ChangeItemID( const string& item_id )		{ m_ItemID=item_id; FilloutDialog(); }
	void DisplayNothingEquiped();

protected:
	string m_ItemID;
};

/*		|-----------|
		| S.Sword	|	(name)
		|			|	(atk/def)
		|			|	(other stats1)
		|			|	(other stats2)
		| ........	|	(description field)
		| ........	|
		|-----------|
*/
class RPGD_EquipmentStatsWithDescDialog : public RPGD_EquipmentStatsDialog
{
public:
	RPGD_EquipmentStatsWithDescDialog( const string& id, const string& item_id );
	//virtual FLW_RESULT Loop( float seconds );
	virtual void SetDimension( Real width, Real height );
	virtual void FilloutDialog();

protected:
	string m_EquipmentDescriptionAreaName;
};

/*		|-----------||------------------------------|
		| S.Sword	||								|
		|			||								|
		|-----------||								|
					 |								|
		|-----------||								|
		| R.Sword	||								|
		|			||								|
		| ........	||								|
		| ........	||								|
		|-----------||								|
					 |								|
					 |------------------------------|

		Caller MUST call SetTextFromInventory() after creating a new object of this class.
		Otherwise nothing will be shown.
*/
class RPGD_PartyEquipmentBagDialog : public RPGD_PartyInventoryListDialog
{
public:
	RPGD_PartyEquipmentBagDialog( const string& id, std::deque<RPG_EQUIPSLOTS::EQUIPSLOT> *slot_stack =nullptr );
	~RPGD_PartyEquipmentBagDialog();
	virtual FLW_RESULT Loop( Real seconds );
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	virtual void SetZOrder( unsigned short z );
	virtual void SetPosition( Real left, Real top );

	virtual void SetTextFromInventory();
	void ShowCurrentItemDialog( bool _show, const string& item_id =string() );
	void ShowChosenItemDialog( bool _show );

protected:
	bool	m_bAllowChoice;
	string	m_CurrentItemID;
	std::deque<RPG_EQUIPSLOTS::EQUIPSLOT>	m_SlotRestricted_SlotStack;
	std::deque<int>							m_SlotRestricted_IndexConversionList;
	RPGD_EquipmentStatsDialog				*m_StatsDialog;
	RPGD_EquipmentStatsWithDescDialog		*m_StatsWithDescDialog;
};

/*		|---------------------------||----------|
		|	みぎて		:	.....	||	Sword	|
		|	ひだりて	:	.....	||			|
		|	あたま		:	.....	||	......	|
		|	からだ		:	.....	||	......	|
		|	あし		:	.....	||----------|
		|	ゆびわ		:	.....	|
		|---------------------------|
*/
class RPGD_EquipmentListDialog : public FLW_BorderedMenuDialog
{
public:
	RPGD_EquipmentListDialog( const string& id, LIFE_PNP *life_pnp );
	~RPGD_EquipmentListDialog();
	virtual FLW_RESULT Loop( Real seconds );
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	virtual void ShowCursor( bool show );
	void NewStatsDialog( const string& item_id );

protected:
	bool								m_ForceRefreshView;
	LIFE_PNP							*m_LifePNP;
	RPG_EQUIPSLOTS::EQUIPSLOT			m_ChosenSlot;
	RPGD_EquipmentActionDialog			*m_EquipmentActionDialog;
	RPGD_PartyEquipmentBagDialog		*m_PartyEquipmentBagDialog;
	FLW_BorderedSingleTextDialog		*m_MessageBox;
	RPGD_EquipmentStatsWithDescDialog	*m_StatsWithDescDialog;
};

/*---------------------------------------------------

		Main Stem Components for Player Menu

---------------------------------------------------*/
/*
	"Item" entry for Player Menu.
*/
class RPGD_PMenu_Item : public RPGD_NameList
{
public:
	RPGD_PMenu_Item( const string& id, std::deque<string> *name_list, bool show_AllEntry );
	~RPGD_PMenu_Item();
	
	virtual FLW_RESULT Loop( Real secsSinceLastFrame );
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	virtual void SetZOrder( unsigned short z )	{	RPGD_NameList::SetZOrder(z);
													if( m_InventoryListDialog )
														m_InventoryListDialog->SetZOrder(z);
												}
	void CreateInventoryListDialog( const string& life_guid );
	void DestroyInventoryListDialog();
	void CreatePartyInventoryListDialog();
	void DestroyPartyInventoryListDialog();
	void ShowCursor_Inventory( bool _show )	{	if( m_InventoryListDialog )
													m_InventoryListDialog->ShowCursor( _show );
												if( m_PartyInventoryListDialog )
													m_PartyInventoryListDialog->ShowCursor( _show );
											}
protected:
	string						m_ChosenLifeID;
	bool						m_ForceInventoryUpdate;
	bool						m_InventoryMode;
	RPGD_InventoryListDialog		*m_InventoryListDialog;
	RPGD_PartyInventoryListDialog	*m_PartyInventoryListDialog;
};
/*
	Specialized case of RPGD_PMenu_Item, used right after
	Inventory Action of Give or わたす, to pick the
	target life (to whom item is passed).

	On Input_PressedOnce, it only returns the chosen life ID
	rather than progressing to yet another Inventory Action sequence.
*/
class RPGD_InventoryAction_Give : public RPGD_PMenu_Item
{
public:
	RPGD_InventoryAction_Give( const string& id, std::deque<string> *name_list );
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
};

/*
	Equipment config
*/

class RPGD_PMenu_Equipment : public RPGD_NameList
{
public:
	RPGD_PMenu_Equipment( std::deque<string> *name_list, bool show_AllEntry );
	~RPGD_PMenu_Equipment();
	virtual FLW_RESULT Loop( Real secsSinceLastFrame );
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );
	//virtual void SetZOrder( unsigned short z )	{	RPGD_NameList::SetZOrder(z);
	//												if( m_InventoryListDialog )
		//												m_InventoryListDialog->SetZOrder(z);
			//									}
	void CreateEquipmentListDialog( const string& life_guid );
	void DestroyEquipmentListDialog();
	void CreatePartyEquipmentBagDialog();
	void DestroyPartyEquipmentBagDialog();
	void ShowCursor_Children( bool _show )	{	if( m_EquipmentListDialog )
													m_EquipmentListDialog->ShowCursor( _show );
												if( m_PartyEquipmentBagDialog )
													m_PartyEquipmentBagDialog->ShowCursor( _show );
											}
protected:
	bool							m_EquipMode;
	RPGD_EquipmentListDialog		*m_EquipmentListDialog;
	RPGD_PartyEquipmentBagDialog	*m_PartyEquipmentBagDialog;
};

/*
	Lists spells acquired.
*/
/*
class RPGD_SpellListDialog : public FLW_BorderedVerticalListDialog
{
public:
	RPGD_SpellListDialog();
	~RPGD_SpellListDialog();
};

/*
	Shows a window full of status.

		name1
		name2
		...
		name5
		All
*/
/*
class RPGD_StatusDialog : public FLW_BorderedVerticalListDialog
{
};

/*
*/
/*
class RPGD_TacticsDialog : public FLW_BorderedVerticalListDialog
{
};

/*
*/
/*
class RPGD_ConfigDialog : public FLW_BorderedVerticalListDialog
{
};


/*-----------------------------------------------------------------------------------
	Classic DQ menu.

		<string id="menu_dougu"		display="どうぐ" />
			|--NameListDialog & ItemDialog
				|- Name1
				|- Name2 ------ transfer control to ItemList
				|- ...
				|- Party

		<string id="menu_soubi" 	display="そうび" />
			|--NameListDialog & EquipDialog
			
		<string id="menu_tsuyosa"	display="つよさ" />
			|--NameListDialog & StatusDialog

		<string id="menu_jyumon"	display="じゅもん" />
			|--NameListDialog & SpellDialog

		<string id="menu_sakusen"	display="さくせん" />
			|--?

		<string id="menu_settei"	display="せってい" />
			|--?
-----------------------------------------------------------------------------------*/
class RPGD_PlayerMenu : public FLW_BorderedMenuDialog
{
public:
	RPGD_PlayerMenu();
	~RPGD_PlayerMenu();
	FLW_RESULT Loop( Real secondsSinceLastFrame );
	virtual FLW_RESULT Input_PressedOnce( eJOYPAD_BUTTON button );

	void SaveCursorPosition()				{ m_SaveCursorPosition = FLW_BorderedMenuDialog::GetCursorPosition(); }
	void RestoreCursorPosition()			{ SetCursorPosition( m_SaveCursorPosition ); }

protected:
	int		m_SaveCursorPosition;
	RPGD_PMenu_Item			*m_PMenuItem;
	RPGD_PMenu_Equipment	*m_PMenuEquip;
};
/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif	//__RPG_GUI_H__