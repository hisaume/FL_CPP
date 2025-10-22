#include <rpg_gui.h>
#include <FL_General.h>
#include <rpg_globals.h>
#include <rpg_state.h>
#include <rpg_database.h>
#include <rpg_helper.h>

#include <assert.h>

/*---------------------
	RPGD_PlayerMenu
---------------------*/
RPGD_PlayerMenu::RPGD_PlayerMenu()
	: FLW_BorderedMenuDialog( "PlayerMenu", NULL, NULL, 3, 2 )
	, m_PMenuItem( nullptr ), m_PMenuEquip( nullptr )
{
	/*	<string id="menu_dougu"		display="どうぐ" /><string id="menu_jyumon"		display="じゅもん" />
		<string id="menu_soubi" 	display="そうび" /><string id="menu_sakusen"	display="さくせん" />
		<string id="menu_tsuyosa"	display="つよさ" /><string id="menu_settei"		display="せってい" />		
	*/
	SetText( gStringTable["menu_dougu"], 0 ); SetText( gStringTable["menu_jyumon"], 1 );
	SetText( gStringTable["menu_soubi"], 2 ); SetText( gStringTable["menu_sakusen"], 3 );
	SetText( gStringTable["menu_tsuyosa"], 4 ); SetText( gStringTable["menu_settei"], 5 );
	FitBorderToTextAreaH();
	SetPosition( 0.6f, 0.1f );	// default position.
}

RPGD_PlayerMenu::~RPGD_PlayerMenu()
{
	delete m_PMenuItem;
	delete m_PMenuEquip;
}

FLW_RESULT RPGD_PlayerMenu::Loop( Real secondsSinceLastFrame )
{
	if( m_PMenuItem )
		m_PMenuItem->Loop( secondsSinceLastFrame );
	if( m_PMenuEquip )
		m_PMenuEquip->Loop( secondsSinceLastFrame );

	return FLW_BorderedMenuDialog::Loop( secondsSinceLastFrame );
}

FLW_RESULT RPGD_PlayerMenu::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	/*--------------------
		Child Process
	--------------------*/
	if( m_PMenuItem ) {
		FLW_RESULT res = m_PMenuItem->Input_PressedOnce( button );
		if( res.message == "CANCEL" ) {
			res.message.clear();
			DELETE_NULL( m_PMenuItem );
		}
		return FLW_RESULT( m_sID );
	}
	else if( m_PMenuEquip ) {
		FLW_RESULT res = m_PMenuEquip->Input_PressedOnce( button );
		if( res.message == "CANCEL" ) {
			res.message.clear();
			DELETE_NULL( m_PMenuEquip );
		}
		return FLW_RESULT( m_sID );
	}
	/*--------------------
		Parent Process
	--------------------*/
	if( button == JOYP_B ) {
		FLW_RESULT res( m_sID );
		res.message = "CANCEL";
		return res;
	}
	FLW_RESULT res = FLW_BorderedMenuDialog::Input_PressedOnce( button );
	if( res.message == "CHOICE" )
	{
		if( res.i == 0 ) {	// Item
			std::deque<string> namelist;
			for( auto iter=STATE->partylist.begin(); iter!=STATE->partylist.end(); ++iter )
				namelist.push_back( iter->first );
			m_PMenuItem = new RPGD_PMenu_Item( m_sID+"_PMenuItem", &namelist, true );
		}
		else if( res.i == 2 ) {	// Equipment
			std::deque<string> namelist;
			for( auto iter=STATE->partylist.begin(); iter!=STATE->partylist.end(); ++iter )
				namelist.push_back( iter->first );
			m_PMenuEquip = new RPGD_PMenu_Equipment( &namelist, true );
		}
	}
	res.id = m_sID;
	return res;
}

/*---------------------
	RPGD_NameList
---------------------*/
RPGD_NameList::RPGD_NameList( string id, std::deque<string> *name_list, bool show_AllEntry )
	: FLW_BorderedMenuDialog( id+"_NameList", NULL, NULL, name_list->size()+(show_AllEntry?1:0), 1 )
	, m_bAllEntry(show_AllEntry), m_NameList( *name_list )
{
	int i;
	for( i=0; i<(int)m_NameList.size(); ++i )
		SetText( STRING_TABLE(m_NameList[i],STATE->GetLife(m_NameList[i])->pnp.default_name), i );
	if( m_bAllEntry )
		SetText( gStringTable["menu_everybody"], i );

	FitBorderToTextAreaH();
	AlignWindowCenter();
}

FLW_RESULT RPGD_NameList::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	if( button == JOYP_B ) {
		FLW_RESULT res( m_sID );
		res.message = "CANCEL";
		return res;
	}

	FLW_RESULT res = FLW_BorderedMenuDialog::Input_PressedOnce( button );
	if( res.message == "CHOICE" ) {
		/*	Convert cursor location into life GUID.		*/
		if( res.i < (int)m_NameList.size() ) {
			res.s = m_NameList[res.i];
		}
		else {
			res.s = "_PARTY_";
		}
	}
	res.id = m_sID;
	return res;
}

/*----------------------------------
	RPGD_InventoryActionDialog
----------------------------------*/
RPGD_InventoryActionDialog::RPGD_InventoryActionDialog( string id, std::deque<string> *name_list )
	: FLW_BorderedMenuDialog( id+"_InventoryAction", NULL, NULL, 3, 1 )
	, m_NameList( *name_list )
	, m_InventoryAction_Give( nullptr )
{
	/*	つかう
		わたす
		すてる
	*/
	SetText( gStringTable["menu_use"], 0 );
	SetText( gStringTable["menu_handover"], 1 );
	SetText( gStringTable["menu_purge"], 2 );
	FitBorderToTextAreaH();
	//AlignWindowCenter();
	SetPosition( 0.21f, 0.18f );
}

RPGD_InventoryActionDialog::~RPGD_InventoryActionDialog()
{
	delete m_InventoryAction_Give;
}

FLW_RESULT RPGD_InventoryActionDialog::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	/*------------------
		Child process
	--------------------*/
	if( m_InventoryAction_Give ) {
		FLW_RESULT res = m_InventoryAction_Give->Input_PressedOnce( button );
		if( res.message == "CHOICE" ) {
			// res.s contains TARGET life ID, res.i holds slot idx (always -1 for now).
			// res.i2 holds quantity.
			res.message = "HANDOVER";//m_InventoryAction;
		}
		else if( res.message == "CANCEL" ) {
			res.message.clear();
			DELETE_NULL( m_InventoryAction_Give );
		}
		res.id = m_sID;
		return res;
	}
	/*--------------------
		Preant process
	--------------------*/
	if( button == JOYP_B ) {
		FLW_RESULT res( m_sID );
		res.message = "CANCEL";
		return res;
	}
	FLW_RESULT res = FLW_BorderedMenuDialog::Input_PressedOnce( button );
	if( res.message == "CHOICE" ) {
		res.message.clear();
		switch( res.i )
		{
		case 0:
			res.message = "USE";
			break;
		case 1:
			m_InventoryAction_Give = new RPGD_InventoryAction_Give( m_sID+"_InventoryActionGive", &m_NameList );
			m_InventoryAction_Give->SetZOrder( GetZOrder()+10 );
			break;
		case 2:
			res.message = "PURGE";
			break;
		}
	}
	res.id = m_sID;
	return res;
}

/*------------------------------
	RPGD_InventoryListDialog
------------------------------*/
RPGD_InventoryListDialog::RPGD_InventoryListDialog( string id, RPG_INVENTORY *inventory,
													std::deque<string> *namelist )
	: FLW_BorderedMenuDialog( id+"_InventoryList", NULL, NULL, inventory->max_size, 1 )
	, m_InventoryActionDialog( nullptr ), m_InventoryHandle( inventory )
	, m_NameList( *namelist ), m_SlotIndex( 0 )
{
	SetTextFromInventory();
	AlignWindowCenter();
}

FLW_RESULT RPGD_InventoryListDialog::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	/*--------------------
		Child process
	--------------------*/
	if( m_InventoryActionDialog ) {
		FLW_RESULT res = m_InventoryActionDialog->Input_PressedOnce( button );
		if( res.message == "HANDOVER" ) {
			// res.s holds TARGET life GUID & res.i holds index (always -1 for now).
			// res.i2 holds quantity.
			res.i3 = res.i2;
			res.i2 = res.i;
			res.i = m_SlotIndex;
			DELETE_NULL( m_InventoryActionDialog );
			SetCursorPosition( 0 ); // reset cursor position to top.
		}
		else if( res.message == "USE" || res.message == "PURGE" ) {
			res.i = m_SlotIndex;
			DELETE_NULL( m_InventoryActionDialog );
			SetCursorPosition( 0 ); // reset cursor position to top.
		}
		else if( res.message == "CANCEL" ) {
			res.message.clear();
			DELETE_NULL( m_InventoryActionDialog );
			SetCursorPosition( 0 ); // reset cursor position to top.
		}
		res.id = m_sID;
		return res;
	}
	/*--------------------
		Preant process
	--------------------*/
	if( button == JOYP_B ) {
		FLW_RESULT res( m_sID );
		res.message = "CANCEL";
		return res;
	}
	FLW_RESULT res = FLW_BorderedMenuDialog::Input_PressedOnce( button );
	if( res.message == "CHOICE" ) {
		res.message.clear();
		// Need to memorize the chosen slot index.
		m_SlotIndex = res.i;
		m_InventoryActionDialog = new RPGD_InventoryActionDialog( m_sID+"_InventoryAction", &m_NameList );
		m_InventoryActionDialog->SetZOrder( GetZOrder()+10 );
	}
	else if( res.message == "MOVED" ) {
		res.message.clear();
		// update item info window.
		res;
	}
	res.id = m_sID;
	return res;
}

void RPGD_InventoryListDialog::SetTextFromInventory()
{
	int idx = 0;
	for( auto iter=m_InventoryHandle->content.begin()
			; iter!=m_InventoryHandle->content.end(); ++iter )
	{
		if( iter->Empty() ) {
			SetText( L"", idx++ );
		}
		else {
			SetText( gStringTable[iter->guid], idx++ );
		}
	}
	if( m_InventoryHandle->Empty() )
		SetText( gStringTable["menu_empty"], 0 );

	FitBorderToTextAreaH();
}

/*-------------------------------------
	RPGD_PartyInventoryListDialog
-------------------------------------*/
RPGD_PartyInventoryListDialog::RPGD_PartyInventoryListDialog( string id, RPG_INVENTORY *inventory,
																std::deque<string> *namelist )
	: FLW_BorderedVerticalListDialog( id+"_PartyInventoryList", NULL, NULL, 8, 2 )
	, m_InventoryActionDialog( nullptr ), m_InventoryHandle( inventory )
	, m_NameList( *namelist ), m_SlotIndex( 0 )
{
	/*	4 columns contain 2 columns of name-quantity pair.
	*/
	//UpdateData();
	//SetTextFromInventory(); <-- called by caller now
	ResizeWindowToShowAll();
	AlignWindowCenter();
	LockCursorH( false );
}

RPGD_PartyInventoryListDialog::~RPGD_PartyInventoryListDialog()
{
	delete m_InventoryActionDialog;
}

FLW_RESULT RPGD_PartyInventoryListDialog::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	/*--------------------
		Child process
	--------------------*/
	if( m_InventoryActionDialog ) {
		FLW_RESULT res = m_InventoryActionDialog->Input_PressedOnce( button );
		if( res.message == "HANDOVER" ) {
			// res.s holds TARGET life GUID & res.i holds index (always -1 for now).
			// res.i2 holds quantity.
			res.i3 = res.i2;
			res.i2 = res.i;
			res.i = m_SlotIndex;
			DELETE_NULL( m_InventoryActionDialog );
		}
		else if( res.message == "USE" || res.message == "PURGE" ) {
			res.i = m_SlotIndex;
			DELETE_NULL( m_InventoryActionDialog );
		}
		else if( res.message == "CANCEL" ) {
			res.message.clear();
			DELETE_NULL( m_InventoryActionDialog );
		}
		res.id = m_sID;
		return res;
	}
	/*--------------------
		Preant process
	--------------------*/
	if( button == JOYP_B ) {
		FLW_RESULT res( m_sID );
		res.message = "CANCEL";
		return res;
	}
	FLW_RESULT res = FLW_BorderedVerticalListDialog::Input_PressedOnce( button );
	if( res.message == "CHOICE" ) {
		res.message.clear();
		if( GetDataTableText(res.i) != L" " ) {
			// Need to memorize the chosen slot index.
			m_SlotIndex = res.i;
			m_InventoryActionDialog = new RPGD_InventoryActionDialog( m_sID+"_InventoryAction", &m_NameList );
			m_InventoryActionDialog->SetZOrder( GetZOrder()+10 );
			//m_InventoryActionDialog->SetPosition( 0.35f, 0.2f );
		}
	}
	else if( res.message == "MOVED" ) {
		res.message.clear();
		// update item info window.
		res;
	}
	res.id = m_sID;
	return res;
}

inline wstring TWO_DIGIT_INT( int i )
{
	wstring twodigit = FL::LONG2WSTRING((long)i);
	if( twodigit.length() == 1 ) {
		twodigit = L" " + twodigit;
	}
	return twodigit;
}

void RPGD_PartyInventoryListDialog::SetTextFromInventory()
{
	/*	Each row has 2 columns, each column has 1 string (quantity is merged with item name).
		NOTE:	Assumes m_InventoryHandle holds an inventory with even number
				of slots because they must be pushed in per row (2 slots per row).
				If Odd number, the last slot won't be inserted.
	*/
	((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->GetDataTableHandle()->clear();

	std::deque<wstring>	rowdata;
	if( m_InventoryHandle->Empty() ) {
		rowdata.push_back( gStringTable["menu_empty"] );
		PushBackData( &rowdata );
		UpdateData();
		return;
	}

	rowdata.clear();
	int idx = 0;
	for( auto iter=m_InventoryHandle->content.begin()
			; iter!=m_InventoryHandle->content.end(); ++iter )
	{
		idx++;
		if( iter->Empty() ) {
			rowdata.push_back( L" " );
		}
		else {
			rowdata.push_back( TWO_DIGIT_INT(iter->quantity) + L"x " + gStringTable[iter->guid] );
		}
		if( idx%2 == 0 ) {
			idx = 0;
			PushBackData( &rowdata );
			rowdata.clear();
		}
	}
	UpdateData();
}

/*----------------------------------
	RPGD_EquipmentActionDialog
----------------------------------*/
RPGD_EquipmentActionDialog::RPGD_EquipmentActionDialog( string id )
	: FLW_BorderedMenuDialog( id, NULL, NULL, 2, 1 )
{
	SetText( gStringTable["menu_equipact_soubi"], 0 );
	SetText( gStringTable["menu_equipact_hazusu"], 1 );
	//SetText( gStringTable["menu_equipact_best"], 2 );
	FitBorderToTextAreaH();
	AlignWindowCenter();
}

FLW_RESULT RPGD_EquipmentActionDialog::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	if( button == JOYP_B ) {
		FLW_RESULT res( m_sID );
		res.message = "CANCEL";
		return res;
	}

	FLW_RESULT res = FLW_BorderedMenuDialog::Input_PressedOnce( button );
	if( res.message == "CHOICE" ) {
		if( res.i == 0 )			// そうび
			res.message = "SOUBI";
		else if( res.i == 1 )		// はずす
			res.message = "HAZUSU";
		else if( res.i == 2 )		// さいきょう
			res.message = "SAIKYOU";
		else
			res.message.clear();
	}

	res.id = m_sID;
	return res;
}

/*--------------------------------
	RPGD_EquipmentStatsDialog
--------------------------------*/
RPGD_EquipmentStatsDialog::RPGD_EquipmentStatsDialog( string id, string item_id )
	: FLW_BorderedMenuDialog( id, NULL, NULL, 6, 1 )
	, m_ItemID( item_id )
{
	FilloutDialog();
	ShowCursor( false );

	//ResizeColumnsToWidestText();
	SetMatrixTextWidth( 0.25f );	// ... then set width. FIXED width.
	FitBorderToTextArea();			// auto adjust border
}

inline wstring WSTRSIGN( int v )			{ if( v >= 0 ) return L"+"; else return L""; }	// don't return '-' as it's already part of a -ve #.
inline wstring WSTRSIGNANDVALUE( int v )	{ return WSTRSIGN(v) + LONG2WSTRING(v); }
void RPGD_EquipmentStatsDialog::FilloutDialog()
{
	/*		(0)ロトのつるぎ
			(1)こうげき力 55 火+10
			(2)ぼうぎょ力 10 無+5 火+2 水+2
			(3)系:ローブ系/重鎧/おの/短剣/etc.	左/右手
			(4)HP+20 MP+5 ち+3 み+1
			(5)			  た+1 き+1 か+1
	*/
	if( m_ItemID.empty() )
		return;
	RPG_ITEM &item = DATABASE->Item[m_ItemID];
	wstring dstr;
	/*----------------------------------------
			Line 0		"ロトのつるぎ"		*/
	dstr = L"[ " + gStringTable[m_ItemID] + L" ]";
	SetText( dstr, 0 );	// item name

	/*----------------------------------------
			Line 1		"攻撃力 50 火+5"		*/
	bool params_exist = false;
	dstr = gStringTable["stats_atk"] + L": ";
	if( item.equipment.atk_physical == 0 )
		dstr += L" - ";
	else
		dstr += LONG2WSTRING(item.equipment.atk_physical);			
	if( item.equipment.atk_elemental != 0 ) {
		if( item.equipment.element == STR_NEUTRAL )
			dstr += L"  " + gStringTable["elemental_neutral_abbrev"];
		else if( item.equipment.element == STR_FIRE )
			dstr += L"  " + gStringTable["elemental_fire_abbrev"];
		else if( item.equipment.element == STR_WATER )
			dstr += L"  " + gStringTable["elemental_water_abbrev"];
		else if( item.equipment.element == STR_POISON )
			dstr += L"  " + gStringTable["elemental_poison_abbrev"];
		dstr += WSTRSIGN(item.equipment.atk_elemental) + LONG2WSTRING(item.equipment.atk_elemental);
	} // 火+5	or  火-5
	SetText( dstr, 1 );

	/*--------------------------------------------------------
			Line 2		"ぼうぎょ力 10 無+5 火+2 水+2"		*/
	params_exist = false;
	dstr = gStringTable["stats_def"] + ": ";
	if( item.equipment.def.physical != 0 ) {
		dstr += LONG2WSTRING(item.equipment.def.physical);
		params_exist = true;
	}
	if( item.equipment.def.neutral != 0 ) {
		dstr += L"  " + gStringTable["elemental_neutral_abbrev"] + WSTRSIGN(item.equipment.def.neutral) + LONG2WSTRING(item.equipment.def.neutral);
		params_exist = true;
	}
	if( item.equipment.def.fire != 0 ) {
		dstr += L"  " + gStringTable["elemental_fire_abbrev"] + WSTRSIGN(item.equipment.def.fire) + LONG2WSTRING(item.equipment.def.fire);
		params_exist = true;
	}
	if( item.equipment.def.water != 0 ) {
		dstr += L"  " + gStringTable["elemental_water_abbrev"] + WSTRSIGN(item.equipment.def.water) + LONG2WSTRING(item.equipment.def.water);
		params_exist = true;
	}
	if( item.equipment.def.poison != 0 ) {
		dstr += L"  " + gStringTable["elemental_poison_abbrev"] + WSTRSIGN(item.equipment.def.poison) + LONG2WSTRING(item.equipment.def.poison);
		params_exist = true;
	}
	if( !params_exist )
		dstr += L" - ";
	SetText( dstr, 2 );

	/*--------------------------------------------------------------------
			Line 3		"系:ローブ系/重鎧/おの/短剣/etc.	右／左手"	*/
	dstr = gStringTable["wear_category"] + L": " + gStringTable["wear_"+item.equipment.wear_category];
	dstr += L"  " + gStringTable[EQUIPSLOT_ID[item.equipment.slot]];
	SetText( dstr, 3 );

	/*--------------------------------------------------------------------
			Line 4 & 5		"HP+20 MP+5 ち+3 み+1 た+1 き+1 か+1"		*/
	dstr.clear();
	wstring dstr_line1;
	if( item.equipment.stats.maxhp != 0 )
		dstr += gStringTable["stats_hp"] + WSTRSIGNANDVALUE(item.equipment.stats.maxhp);
	if( item.equipment.stats.maxmp != 0 )
		dstr += L" " + gStringTable["stats_mp"] + WSTRSIGNANDVALUE(item.equipment.stats.maxmp);
	if( item.equipment.stats.strength != 0 )
		dstr += L" " + gStringTable["stats_strength_abbrev"] + WSTRSIGNANDVALUE(item.equipment.stats.strength);
	if( item.equipment.stats.physical != 0 )
		dstr += L" " + gStringTable["stats_physical_abbrev"] + WSTRSIGNANDVALUE(item.equipment.stats.physical);
	// Length control
	if( GetFontHandle()->TextWidth(dstr) > 0.20f ) {
		dstr_line1 = dstr;
		dstr = L"   "; // indent
	}
	if( item.equipment.stats.stamina != 0 )
		dstr += L" " + gStringTable["stats_stamina_abbrev"] + WSTRSIGNANDVALUE(item.equipment.stats.stamina);
	if( item.equipment.stats.accuracy_pct != 0 )
		dstr += L" " + gStringTable["stats_dexterity_abbrev"] + WSTRSIGNANDVALUE(item.equipment.stats.accuracy_pct);
	if( item.equipment.stats.intelligence != 0 )
		dstr += L" " + gStringTable["stats_intelligence_abbrev"] + WSTRSIGNANDVALUE(item.equipment.stats.intelligence);
	if( dstr_line1.length() > 0 ) {
		SetText( dstr_line1, 4 );
		SetText( dstr, 5 );
	}
	else {
		SetText( dstr, 4 );
		SetText( L" ", 5 ); // empty line 5
	}
}

void RPGD_EquipmentStatsDialog::DisplayNothingEquiped()
{
	int i = 0;
	SetText( gStringTable["menu_nothingequiped_msg"], i++ );
	SetText( L"", i++ );
	SetText( L"", i++ );
	SetText( L"", i++ );
	SetText( L"", i++ );
	SetText( L"", i++ );
}

/*----------------------------------------
	RPGD_EquipmentStatsWithDescDialog
----------------------------------------*/
RPGD_EquipmentStatsWithDescDialog::RPGD_EquipmentStatsWithDescDialog( string id, string item_id )
	: RPGD_EquipmentStatsDialog( id, item_id )
{
	m_EquipmentDescriptionAreaName = id + "_EquipmentDescArea";
	FLWE_Interface *txtArea = new FLWE_MultiLineTextArea( m_EquipmentDescriptionAreaName, m_OverlayContainer, NULL, 4 );
	txtArea->SetPosition( GetBorderSize_Sides()+GetLeft()+m_LeftPadding
						, GetBorderSize_TopAndBottom()+GetTop()+6*GetFontHandle()->GetHeight() );	// Need to add 0.04f to height??????
	((FLWE_MultiLineTextArea*)txtArea)->SetDimension( ((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName])->GetWidth()
													, ((FLWE_MultiLineTextArea*)txtArea)->GetHeight() );
	((FLWE_MultiLineTextArea*)txtArea)->SetTimeInterval( 0.008f );
	AddElement( txtArea );

	SetDimension( GetWidth(), GetHeight()+((FLWE_MultiLineTextArea*)txtArea)->GetHeight() );
	FilloutDialog();
}

void RPGD_EquipmentStatsWithDescDialog::SetDimension( Real width, Real height )
{
	RPGD_EquipmentStatsDialog::SetDimension( width, height );

	int descriptionTextLines = 4;
	Real lineSpacing = 0.0f;
	((FLWE_MultiLineTextArea*)m_Elements[m_EquipmentDescriptionAreaName])
		->SetDimension( GetWidth() - 2*GetBorderSize_Sides() - m_LeftPadding - m_RightPadding
						, descriptionTextLines*GetFontHandle()->GetHeight() + (descriptionTextLines-1)*lineSpacing );
}

/*
FLW_RESULT RPGD_EquipmentStatsWithDescDialog::Loop( float seconds )
{
	return RPGD_EquipmentStatsDialog::Loop( seconds );
}*/
void RPGD_EquipmentStatsWithDescDialog::FilloutDialog()
{
	RPGD_EquipmentStatsDialog::FilloutDialog();

	if( m_EquipmentDescriptionAreaName.length() > 0 ) {
		((FLWE_MultiLineTextArea*)m_Elements[m_EquipmentDescriptionAreaName])->SetText( gStringTable[m_ItemID+"_desc"] );
	}
}

/*--------------------------------
	RPGD_EquipmentListDialog
--------------------------------*/
RPGD_EquipmentListDialog::RPGD_EquipmentListDialog( string id, LIFE_PNP *life_pnp )
	: FLW_BorderedMenuDialog( id, NULL, NULL, RPG_EQUIPSLOTS::SLOT_SIZE, 1, 0.02f, 0.133f, 0.05f )
	, m_LifePNP( life_pnp ), m_ChosenSlot(RPG_EQUIPSLOTS::RHAND)
	, m_PartyEquipmentBagDialog( nullptr )
	, m_EquipmentActionDialog( nullptr )
	, m_StatsWithDescDialog( nullptr )
	, m_MessageBox( nullptr )
	, m_ForceRefreshView( false )
{
	/*
		Create Left-side caption elements e.g. みぎて、 ひだりて
	*/
	Real longest_caption_width = 0;
	// Draw the captions
	Real top=GetBorderSize_TopAndBottom(), left=GetBorderSize_Sides()+0.01f, width=0.1f;
	FLWE_Interface *singleTxt;
	for( int i=0; i<RPG_EQUIPSLOTS::SLOT_SIZE; ++i ) {
		singleTxt = new FLWE_TextArea( m_sID+"_CaptionTxt"+string(FL::LONG2STRING(i)), m_OverlayContainer, NULL );
		singleTxt->SetPosition( left,
								top + i*((FLWE_TextArea*)singleTxt)->GetFontHandle()->GetHeight() );
		singleTxt->SetDimension( width, ((FLWE_TextArea*)singleTxt)->GetFontHandle()->GetHeight() );
		((FLWE_TextArea*)singleTxt)->SetText( gStringTable[EQUIPSLOT_ID[i]] );
		AddElement( singleTxt );
		// record text width if it's longest so far.
		Real width = ((FLWE_TextArea*)singleTxt)->GetFontHandle()->TextWidth( gStringTable[EQUIPSLOT_ID[i]] );
		if( longest_caption_width < width )
			longest_caption_width = width;
	}
	// Draw dividers
	for( int i=0; i<RPG_EQUIPSLOTS::SLOT_SIZE; ++i ) {
		singleTxt = new FLWE_TextArea( m_sID+"DividerTxt"+string(FL::LONG2STRING(i)), m_OverlayContainer, NULL );
		singleTxt->SetPosition( left + longest_caption_width + 0.02f,
								top + i*((FLWE_TextArea*)singleTxt)->GetFontHandle()->GetHeight() );
		singleTxt->SetDimension( width, ((FLWE_TextArea*)singleTxt)->GetFontHandle()->GetHeight() );
		((FLWE_TextArea*)singleTxt)->SetText( L"|" );
		AddElement( singleTxt );
	}
	// Adjust border
	FitBorderToTextAreaV();
	SetDimension( 0.3f, GetHeight() );
	/*
		Upload data
	*/
	RPG_EQUIPSLOTS *equipslots = &m_LifePNP->equipment;
	for( int i=0; i<RPG_EQUIPSLOTS::SLOT_SIZE; ++i ) {
		if( false == gStringTable[equipslots->slot[(RPG_EQUIPSLOTS::EQUIPSLOT)i]].empty() )
			SetText( gStringTable[equipslots->slot[(RPG_EQUIPSLOTS::EQUIPSLOT)i]], i );
		else
			SetText( L" ", i );
	}
}

RPGD_EquipmentListDialog::~RPGD_EquipmentListDialog()
{
	delete m_MessageBox;
	delete m_StatsWithDescDialog;
	delete m_EquipmentActionDialog;
	delete m_PartyEquipmentBagDialog;
}

FLW_RESULT RPGD_EquipmentListDialog::Loop( Real seconds )
{
	if( m_ForceRefreshView ) {
		m_ForceRefreshView = false;
		/*------------------
			Upload data
		------------------*/
		RPG_EQUIPSLOTS *equipslots = &m_LifePNP->equipment;
		for( int i=0; i<RPG_EQUIPSLOTS::SLOT_SIZE; ++i ) {
			if( false == gStringTable[equipslots->slot[(RPG_EQUIPSLOTS::EQUIPSLOT)i]].empty() )
				SetText( gStringTable[equipslots->slot[(RPG_EQUIPSLOTS::EQUIPSLOT)i]], i );
			else
				SetText( L" ", i );
		}
		NewStatsDialog( equipslots->slot[GetCursorPosition()] );
	}

	if( m_StatsWithDescDialog )
		m_StatsWithDescDialog->Loop( seconds );
	if( m_PartyEquipmentBagDialog )
		m_PartyEquipmentBagDialog->Loop( seconds );

	return FLW_BorderedMenuDialog::Loop( seconds );
}

FLW_RESULT RPGD_EquipmentListDialog::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	/*--------------------
		Child Process
	--------------------*/
	if( m_MessageBox ) {
		if( button == JOYP_A || button == JOYP_B )
			DELETE_NULL( m_MessageBox );
		return FLW_RESULT( m_sID );
	}
	else if( m_EquipmentActionDialog ) {
		FLW_RESULT res = m_EquipmentActionDialog->Input_PressedOnce( button );
		if( res.message == "CANCEL" ) {
			DELETE_NULL( m_EquipmentActionDialog );
		}
		else if( res.message == "SOUBI" ) {
			/*	Create a temporary list (conversionList) with HELPER. If this is empty, it means there is nothing
				in the equipment bag for m_ChosenSlot, so don't allow creating m_PartyEquipmentBagDialog.
			*/
			std::deque<int> conversionList;
			std::deque<RPG_EQUIPSLOTS::EQUIPSLOT> slot_stack;
			slot_stack.push_back( m_ChosenSlot ); // this is always the case.
			/*
				Additional equipment for m_ChosenSlot:
									If right hand, it can be (1) 1H right hand weapon (2) 1H either hand weapon (3) 2H weapon.
									If left hand, it can be (1) 1H off-hand weapon (incl. shield) (2) 1H either hand weapon
									Else just m_ChosenSlot.			*/
			switch( m_ChosenSlot ) {
			case RPG_EQUIPSLOTS::RHAND:
				slot_stack.push_back( RPG_EQUIPSLOTS::EITHERHAND );
				slot_stack.push_back( RPG_EQUIPSLOTS::TWOHAND );
				break;
			case RPG_EQUIPSLOTS::LHAND:
				slot_stack.push_back( RPG_EQUIPSLOTS::EITHERHAND );
				break;
			}
			std::deque<string> item_id_list;	// following function requires it, but discarded here.
			HELPER::Inventory_CreateEquipSlotAndWearRestrictedList( STATE->inventory_equip, slot_stack, m_LifePNP->wear_category
																	, &conversionList, &item_id_list );
			/*
				Now decide whether there's any equipment which can be used in the chosen slot.
			*/
			if( false == conversionList.empty() ) {	// IF there's something in the bag which can go into this chosen slot...
				DELETE_NULL( m_EquipmentActionDialog );
				m_PartyEquipmentBagDialog = new RPGD_PartyEquipmentBagDialog( m_sID + "_PartyEquipBag", &slot_stack ); // <-- The ONLY entry point for this.
				m_PartyEquipmentBagDialog->SetTextFromInventory();
				m_PartyEquipmentBagDialog->SetZOrder( GetZOrder()+20 );
				m_PartyEquipmentBagDialog->ShowCurrentItemDialog( true, m_LifePNP->equipment.slot[m_ChosenSlot] );
				m_PartyEquipmentBagDialog->SetPosition( 0.20f, 0.16f );
				m_PartyEquipmentBagDialog->AllowEmptySelection( true );
			}
			else {	// Nothing in the bag which can go into m_ChosenSlot, so tell the player so.
				m_MessageBox = new FLW_BorderedSingleTextDialog( m_sID+"_MsgBox", NULL, NULL );
				m_MessageBox->SetText( gStringTable["menu_noequipment_msg"] );
				m_MessageBox->SetZOrder( GetZOrder()+100 );
				m_MessageBox->AlignWindowCenter();
				DELETE_NULL( m_EquipmentActionDialog );		// shut down equip-action dialog.
			}
		}
		else if( res.message == "HAZUSU" ) {
			DELETE_NULL( m_EquipmentActionDialog );
			m_ForceRefreshView = true;
			// Issue command
			RPG_COMMAND com;
			com.verb = "LIFE_Unequip";
			com.subject = m_LifePNP->guid;		// who's equipment?
			com.i.push_back( m_ChosenSlot );	// which equipment slot?
			COMMAND.push_back( com );
		}
		else if( res.message == "SAIKYOU" ) {
			// POSSIBLE FUTURE IMPLEMENTATION
			DELETE_NULL( m_EquipmentActionDialog );
			/*
			RPG_COMMAND com;
			com.verb = "LIFE_EquipSaikyou";
			com.subject = m_LifePNP->guid;
			com.i.push_back( m_ChosenSlot );
			COMMAND.push_back( com );
			*/
		}
		return FLW_RESULT( m_sID );
	}
	else if( m_PartyEquipmentBagDialog ) {
		FLW_RESULT res = m_PartyEquipmentBagDialog->Input_PressedOnce( button );
		if( res.message == "CANCEL" ) {
			res.message.clear();
			DELETE_NULL( m_PartyEquipmentBagDialog );
		}
		else if( res.message == "CHOICE" ) {
			res.message.clear();
			RPG_COMMAND com;
			com.verb = "LIFE_EquipByInventorySlot";
			com.s.push_back( m_LifePNP->guid );	// life ID
			com.i.push_back( res.i );			// inventory slot index
			com.i.push_back( m_ChosenSlot );	// specific target slot where equipment is going into
			COMMAND.push_back( com );
			DELETE_NULL( m_PartyEquipmentBagDialog );
			m_ForceRefreshView = true;
		}
		return res;
	}
	/*--------------------
		Parent Process
	--------------------*/
	if( button == JOYP_B ) {
		FLW_RESULT res( m_sID );
		res.message = "CANCEL";
		return res;
	}
	// Main
	FLW_RESULT res = FLW_BorderedMenuDialog::Input_PressedOnce( button );
	if( res.message == "CHOICE" ) {
		// res.i corresponds to index in equip slot. Store this for later use.
		m_ChosenSlot = (RPG_EQUIPSLOTS::EQUIPSLOT)res.i;
		m_EquipmentActionDialog = new RPGD_EquipmentActionDialog( m_sID + "_EquipAction" );
		m_EquipmentActionDialog->SetZOrder( GetZOrder()+5 );
		FLWE_MatrixTextArea &ta = *((FLWE_MatrixTextArea*)m_Elements[m_MatrixTextAreaName]);
		m_EquipmentActionDialog->SetPosition( GetLeft_CursorPanel(), GetTop_CursorPanel() );
	}
	else if( res.message == "MOVED" ) {
		NewStatsDialog( m_LifePNP->equipment.slot[res.i] );
	}

	res.id = m_sID;
	return res;
}

void RPGD_EquipmentListDialog::ShowCursor( bool show )
{
	// Cursor being shown is the cue to create the stats dialog.

	if( show )
		NewStatsDialog( m_LifePNP->equipment.slot[GetCursorPosition()] );
	else
		DELETE_NULL( m_StatsWithDescDialog );

	FLW_BorderedMenuDialog::ShowCursor( show );
}

void RPGD_EquipmentListDialog::NewStatsDialog( string item_id )
{
	DELETE_NULL( m_StatsWithDescDialog );
	if( item_id.empty() )
		return;
	m_StatsWithDescDialog = new RPGD_EquipmentStatsWithDescDialog( m_sID+"_StatsWithDesc", item_id );
	m_StatsWithDescDialog->SetPosition( GetLeft()+GetWidth(), GetTop() );
	m_StatsWithDescDialog->SetZOrder( GetZOrder()+10 );
}

/*-----------------------------------
	RPGD_PartyEquipmentBagDialog
-----------------------------------*/
std::deque<string> ___ERROR_EVASION_circumvent_namelist___(1,"ERROR_EVASION_circumvent_namelist");

RPGD_PartyEquipmentBagDialog::RPGD_PartyEquipmentBagDialog( string id, std::deque<RPG_EQUIPSLOTS::EQUIPSLOT> *slot_stack )
	: RPGD_PartyInventoryListDialog( id, &STATE->inventory_equip, &___ERROR_EVASION_circumvent_namelist___ )
	, m_bAllowChoice( false ), m_SlotRestricted_SlotStack( slot_stack!=nullptr?*slot_stack:std::deque<RPG_EQUIPSLOTS::EQUIPSLOT>() )
	, m_StatsDialog( nullptr )
	, m_StatsWithDescDialog( nullptr )
{
	//AllowEmptySelection( true );
	int i = 0;

	Real posx=0.1f, posy=0.1f;
	//Real statsDialogWidth = 0.35f;

	m_StatsDialog = new RPGD_EquipmentStatsDialog( m_sID+"_EquipStats", string() );
	m_StatsDialog->SetPosition( posx, posy );
	m_StatsDialog->FilloutDialog();

	m_StatsWithDescDialog = new RPGD_EquipmentStatsWithDescDialog( m_sID+"_EquipStatsWithDesc", string() );
	m_StatsWithDescDialog->SetPosition( posx, posy+m_StatsDialog->GetHeight() );

	SetTextFromInventory();	// fills out m_SlotRestricted_inventoryConversionList which is needed below.
	int bag_index = 0; //GetCursorPosition();	// should be 0.
	if( false == m_SlotRestricted_SlotStack.empty() )
		bag_index = m_SlotRestricted_IndexConversionList[bag_index]; // convert
	string first_itemid = STATE->inventory_equip.GetSlot( bag_index )->guid;
	m_StatsWithDescDialog->ChangeItemID( first_itemid );
	m_StatsWithDescDialog->FilloutDialog(); // fill stats dialog accordingly.

	SetPosition( m_StatsDialog->GetLeft()+m_StatsDialog->GetWidth(), m_StatsDialog->GetTop() );
}

RPGD_PartyEquipmentBagDialog::~RPGD_PartyEquipmentBagDialog()
{
	delete m_StatsDialog;
	delete m_StatsWithDescDialog;
}

FLW_RESULT RPGD_PartyEquipmentBagDialog::Loop( Real seconds )
{
	if( m_StatsDialog )
		m_StatsDialog->Loop( seconds );
	if( m_StatsWithDescDialog )
		m_StatsWithDescDialog->Loop( seconds );

	return FLW_RESULT( m_sID );
}

FLW_RESULT RPGD_PartyEquipmentBagDialog::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	/* Note how parent class is by-passed as parent behaviour is undesirable.
		Go straight to the parent class OF the parent class i.e. FLW_BorderedVerticalListDialog */

	/*--------------------
		Preant process
	--------------------*/
	if( button == JOYP_B ) {
		FLW_RESULT res( m_sID );
		res.message = "CANCEL";
		return res;
	}
	FLW_RESULT res = FLW_BorderedVerticalListDialog::Input_PressedOnce( button );
	if( res.message == "CHOICE" ) {
		if( GetDataTableText(res.i)!=L" " && !GetDataTableText(res.i).empty() ) { // Disallow selecting an empty slot.
			// res.i contains the list index.
			if( false == m_SlotRestricted_SlotStack.empty() ) {	// IF slot restriction is used...
				/* res.i holds the index to the temporary list.
					Convert this into index for the real inventory ie. m_InventoryHandle
				*/
				res.i = m_SlotRestricted_IndexConversionList[res.i];	// conversion.
			}
		}
		else
			res.message.clear();	// unchoosable choice.
	}
	else if( res.message == "MOVED" ) {
		// res.i contains the list index.
		res.message.clear();
		// update item info window.
		bool within_data = false;
		if( false == m_SlotRestricted_SlotStack.empty() ) {	// IF slot restriction is used...
			/* res.i holds the index to the temporary list.
				Convert this into index for the real inventory ie. m_InventoryHandle
			*/
			if( res.i < (int)m_SlotRestricted_IndexConversionList.size() ) { // if within the data...
				res.i = m_SlotRestricted_IndexConversionList[res.i];	// conversion.
				within_data = true;
			}
		}
		if( m_StatsWithDescDialog ) {
			string current_itemid;
			if( false == m_SlotRestricted_SlotStack.empty() ) {
				if( within_data )
					current_itemid = STATE->inventory_equip.GetSlot(res.i)->guid;
			}
			else {
				if( res.i < STATE->inventory_equip.max_size )
					current_itemid = STATE->inventory_equip.GetSlot(res.i)->guid;
			}
			if( current_itemid.empty() )
				m_StatsWithDescDialog->Show( false );
			else {
				m_StatsWithDescDialog->Show();
				m_StatsWithDescDialog->ChangeItemID( current_itemid );
			}
		}
	}
	res.id = m_sID;
	return res;
}

void RPGD_PartyEquipmentBagDialog::SetZOrder( unsigned short z )
{
	RPGD_PartyInventoryListDialog::SetZOrder( z );
	/*
		set z order of children as parent +1 or whatever.
	*/
	if( m_StatsDialog )
		m_StatsDialog->SetZOrder( z+2 );
	if( m_StatsWithDescDialog )
		m_StatsWithDescDialog->SetZOrder( z+4 );
}

void RPGD_PartyEquipmentBagDialog::SetPosition( Real left, Real top )
{
	/*----------------------------
			Bag Dialogs			
			always in the same place at (left,top)
	*/
	if( !m_StatsDialog->IsShown() && !m_StatsWithDescDialog->IsShown() )
		RPGD_PartyInventoryListDialog::SetPosition( left, top );
	else
		//	At least 1 of the stats windows is visible.
		RPGD_PartyInventoryListDialog::SetPosition( left, top );
	/*----------------------------
			Stats Dialogs
			shown on the RIGHT SIDE of the bag window
	*/
	Real leftOfStatsWindows = GetLeft() + GetWidth();
	m_StatsDialog->SetPosition( leftOfStatsWindows, top );
	if( m_StatsDialog->IsShown() )
		m_StatsWithDescDialog->SetPosition( leftOfStatsWindows, top+m_StatsDialog->GetHeight() );
	else
		m_StatsWithDescDialog->SetPosition( leftOfStatsWindows, top );
}

void RPGD_PartyEquipmentBagDialog::SetTextFromInventory()
{
	((FLWE_ScrollableMatrixTextArea*)m_Elements[m_ListTextAreaName])->GetDataTableHandle()->clear();

	std::deque<wstring>	rowdata;
	if( m_InventoryHandle->Empty() ) {
		rowdata.push_back( gStringTable["menu_empty"] );
		PushBackData( &rowdata );
		UpdateData();
		return;
	}

	rowdata.clear();
	int idx = 0;

	/* If this dialog is only for items which go into a specific slot(s),
		it needs to create a new list to hold only those which suit this need.
	*/
	if( false == m_SlotRestricted_SlotStack.empty() ) {	// IF slot restriction is used...
		m_SlotRestricted_IndexConversionList.clear();		// conversion list is kept as it will be used to return correct inv slot on user choice.
		std::deque<string>	_SlotRestricted_ItemIDList;		// Item ID does not concern this class.
		HELPER::Inventory_CreateEquipSlotRestrictedList( *m_InventoryHandle, m_SlotRestricted_SlotStack
														, &m_SlotRestricted_IndexConversionList, &_SlotRestricted_ItemIDList );
		// Display listed data
		for( auto iter=_SlotRestricted_ItemIDList.begin(); iter!=_SlotRestricted_ItemIDList.end(); ++iter )
		{
			++idx;
			rowdata.push_back( gStringTable[*iter] );
			if( idx%2 == 0 ) {
				idx = 0;
				PushBackData( &rowdata );
				rowdata.clear();
			}
		}
	}
	else {
		// Display entire equipment-inventory content.
		for( auto iter=m_InventoryHandle->content.begin(); iter!=m_InventoryHandle->content.end(); ++iter )
		{
			++idx;
			if( iter->Empty() ) {
				rowdata.push_back( L"" );
			}
			else {
				rowdata.push_back( gStringTable[iter->guid] );
			}
			if( idx%2 == 0 ) {
				idx = 0;
				PushBackData( &rowdata );
				rowdata.clear();
			}
		}
	}
	/* It's possible that rowdata is not empty as data is pushed in pairs, so odd # of data would
		result in a left over which wouldn't be displayed without the following.
	*/
	if( false == rowdata.empty() ) {	// if there's still data remaining to be inserted...
		rowdata.push_back( L"" );	// get another empty data in to make a data set.
		PushBackData( &rowdata );
	}
	UpdateData();
}

void RPGD_PartyEquipmentBagDialog::ShowCurrentItemDialog( bool _show, string item_id )
{
	/*
		if item_id is empty, it means nothing is equipped for now.
		Display stating so.
	*/
	if( item_id.empty() )
		m_StatsDialog->DisplayNothingEquiped();
	else
		m_StatsDialog->ChangeItemID( item_id );
	// Visibility control
	m_StatsDialog->Show( _show );
	SetPosition( GetLeft(), GetTop() ); // triggers repositioning of all dialogs
}

void RPGD_PartyEquipmentBagDialog::ShowChosenItemDialog( bool _show )
{
	m_StatsWithDescDialog->Show( _show );
	SetPosition( GetLeft(), GetTop() ); // triggers repositioning of all dialogs
}

/*---------------------
	RPGD_PMenu_Item
---------------------*/
RPGD_PMenu_Item::RPGD_PMenu_Item( string id, std::deque<string> *name_list, bool show_AllEntry )
	: RPGD_NameList( id+"_PMenu_Item", name_list, show_AllEntry )
	, m_InventoryListDialog( nullptr ), m_InventoryMode( false ), m_ForceInventoryUpdate( false )
	, m_PartyInventoryListDialog( nullptr )
{
	SetPosition( 0.15f, 0.20f );
	CreateInventoryListDialog( m_NameList[0] );	// Default index of which the content is displayed.
}

RPGD_PMenu_Item::~RPGD_PMenu_Item()
{
	delete m_PartyInventoryListDialog;
	delete m_InventoryListDialog;
}

FLW_RESULT RPGD_PMenu_Item::Loop( Real secsSinceLastFrame )
{
	/*	After alteration to the inventory, this flag will be set
		true. Refresh the inventory view using the new data.	*/
	if( m_ForceInventoryUpdate ) {
		m_ForceInventoryUpdate = false;
		if( m_InventoryListDialog )
			m_InventoryListDialog->SetTextFromInventory();
		if( m_PartyInventoryListDialog )
			m_PartyInventoryListDialog->SetTextFromInventory();
	}

	return RPGD_NameList::Loop( secsSinceLastFrame );
}

FLW_RESULT RPGD_PMenu_Item::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	/*
		Item choice process & Final process
	*/
	if( m_InventoryMode )
	{
		FLW_RESULT res;
		/*-----------------------------------------------------
			Decide which inventory list should receive input
		*/
		if( m_InventoryListDialog )
			res = m_InventoryListDialog->Input_PressedOnce( button );
		else if( m_PartyInventoryListDialog ) {
			res = m_PartyInventoryListDialog->Input_PressedOnce( button );
		}
		/*-----------------------
			Message handling
		-----------------------*/
		if( res.message == "USE" ) {
			res.message.clear();
			/*	Need:	life GUID		(m_ChosenLifeID)
						Slot Nth index	(res.i)				*/
			// Send command.
			// GUI state.
			m_ForceInventoryUpdate = true;
			m_InventoryMode = false;
			ShowCursor_Inventory( false );
		}
		else if( res.message == "HANDOVER" ) {
			/*	Need:	life GUID				(m_ChosenLifeID)
						Slot Nth index			(res.i)
						target life GUID		(res.s)
						target slot Nth index	(res.i2) <--- always -1 for now.
						quantity				(res.i3) <--- always 1 for now	*/
			// Send command.
			RPG_COMMAND com;
			com.verb = "LIFE_Handover";
			res.message.clear();
			com.subject = m_ChosenLifeID;
			com.target = res.s;
			com.i.push_back( res.i );	// from this slot
			com.i.push_back( res.i2 );	// to this slot
			com.i.push_back( res.i3 );	// to this slot
			COMMAND.push_back( com );
			// GUI state.
			m_ForceInventoryUpdate = true;
			m_InventoryMode = false;
			ShowCursor_Inventory( false );
		}
		else if( res.message == "PURGE" ) {
			res.message.clear();
			/*	Need:	life GUID		(m_ChosenLifeID)
						Slot Nth index	(res.i)				*/
			// Send command.
			// GUI state.
			m_ForceInventoryUpdate = true;
			m_InventoryMode = false;
			ShowCursor_Inventory( false );
		}
		else if( res.message == "CANCEL" ) {
			res.message.clear();
			m_InventoryMode = false;
			ShowCursor_Inventory( false );
		}
		res.id = m_sID;
		return res;
	}
	/*
		Name choice process
	*/
	FLW_RESULT res = RPGD_NameList::Input_PressedOnce( button );
	if( res.message == "MOVED" )
	{
		res.message.clear();
		DestroyInventoryListDialog();
		DestroyPartyInventoryListDialog();
		if( res.i < (int)m_NameList.size() )
			CreateInventoryListDialog( m_NameList[res.i] );
		else
			CreatePartyInventoryListDialog();
	}
	else if( res.message == "CHOICE" )
	{
		res.message.clear();
		if( res.s == "_PARTY_" ) {
			if( !m_PartyInventoryListDialog->Empty() ) {
				m_PartyInventoryListDialog->ShowCursor( true );
				m_InventoryMode = true;
			}
		}
		else {
			if( !m_InventoryListDialog->Empty() ) {
				m_InventoryListDialog->ShowCursor( true );
				m_InventoryMode = true;
			}
		}
		if( m_InventoryMode )
			m_ChosenLifeID = res.s;
	}
	else if( res.message == "CANCEL" ) {
		// do nothing. just pass this onto caller.
	}
	res.id = m_sID;
	return res;
}

void RPGD_PMenu_Item::CreateInventoryListDialog( string guid )
{
	LIFE_OBJECT *life = STATE->GetLife( guid );
	if( nullptr == life ) {
		ERR( guid, L"RPGD_PMenu_Item ctor - GetLife for this GUID has returned nullptr. assert about to fail.\n" );
		bool life_guid_does_not_exist = true;
		assert( false == life_guid_does_not_exist );
	}
	m_InventoryListDialog = new RPGD_InventoryListDialog( m_sID+"_PMenuItem", &life->pnp.inventory, &m_NameList );
	m_InventoryListDialog->SetZOrder( GetZOrder()+1 );
	m_InventoryListDialog->ShowCursor( false );
	m_InventoryListDialog->SetPosition( GetLeft()+GetWidth(), GetTop()	);
}

void RPGD_PMenu_Item::DestroyInventoryListDialog()
{
	DELETE_NULL( m_InventoryListDialog );
}

void RPGD_PMenu_Item::CreatePartyInventoryListDialog()
{
	m_PartyInventoryListDialog = new RPGD_PartyInventoryListDialog( m_sID+"_PMenuItem", &STATE->inventory_item, &m_NameList );
	m_PartyInventoryListDialog->SetTextFromInventory();
	m_PartyInventoryListDialog->SetZOrder( GetZOrder()+1 );
	m_PartyInventoryListDialog->ShowCursor( false );
	m_PartyInventoryListDialog->SetPosition( GetLeft()+GetWidth(), GetTop()	);
}

void RPGD_PMenu_Item::DestroyPartyInventoryListDialog()
{
	DELETE_NULL( m_PartyInventoryListDialog );
}

/*--------------------------------
	RPGD_InventoryAction_Give
--------------------------------*/
RPGD_InventoryAction_Give::RPGD_InventoryAction_Give( string id, std::deque<string> *name_list )
	: RPGD_PMenu_Item( id+"_InventoryActionGive", name_list, true )
{
	// EMPTY
	SetPosition( 0.25f, 0.3f );
	if( m_InventoryListDialog )
		m_InventoryListDialog->SetPosition( GetLeft()+GetWidth(), GetTop() );
	if( m_PartyInventoryListDialog )
		m_PartyInventoryListDialog->SetPosition( GetLeft()+GetWidth(), GetTop() );
}

FLW_RESULT RPGD_InventoryAction_Give::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	FLW_RESULT res = RPGD_NameList::Input_PressedOnce( button );
	/*	CHOICE or CANCEL should just be passed onto caller.
		If CHOICE, res.s contains life GUID already.
	*/
	if( res.message == "MOVED" )
	{
		res.message.clear();
		DestroyInventoryListDialog();
		DestroyPartyInventoryListDialog();
		if( res.i < (int)m_NameList.size() )
			CreateInventoryListDialog( m_NameList[res.i] );
		else
			CreatePartyInventoryListDialog();
	}
	else if( res.message == "CHOICE" ) {
		// res.i holds target slot idx, res.i2 holds quantity to be transfered.
		res.i = -1;	// -1 for now as there's no choice allowed.
		res.i2 = 1;
	}
	res.id = m_sID;
	return res;
}


/*--------------------------------

		RPGD_PMenu_Equipment

--------------------------------*/
RPGD_PMenu_Equipment::RPGD_PMenu_Equipment( std::deque<string> *name_list, bool show_AllEntry )
	: RPGD_NameList( "PMenuEquipment", name_list, show_AllEntry )
	, m_EquipmentListDialog(nullptr), m_PartyEquipmentBagDialog(nullptr), m_EquipMode(false)
{
	SetPosition( 0.15f, 0.20f );
	CreateEquipmentListDialog( m_NameList[0] );	// Default index of which the content is displayed.
}

RPGD_PMenu_Equipment::~RPGD_PMenu_Equipment()
{
	delete m_EquipmentListDialog;
	delete m_PartyEquipmentBagDialog;
}

FLW_RESULT RPGD_PMenu_Equipment::Loop( Real secsSinceLastFrame )
{
	if( m_EquipmentListDialog )
		m_EquipmentListDialog->Loop( secsSinceLastFrame );;
	if( m_PartyEquipmentBagDialog )
		m_PartyEquipmentBagDialog->Loop( secsSinceLastFrame );;

	return RPGD_NameList::Loop( secsSinceLastFrame );
}

FLW_RESULT RPGD_PMenu_Equipment::Input_PressedOnce( eJOYPAD_BUTTON button )
{
	if( m_EquipMode ) {
		FLW_RESULT res;
		// Decide which child should receive the input.
		if( m_EquipmentListDialog )
			res = m_EquipmentListDialog->Input_PressedOnce( button );
		else if( m_PartyEquipmentBagDialog )
			res = m_PartyEquipmentBagDialog->Input_PressedOnce( button );
		// Process result.
		if( res.message == "CANCEL" ) {
			res.message.clear();
			m_EquipMode = false;
			ShowCursor_Children( false );
			if( m_PartyEquipmentBagDialog ) // if cancelling out of party bag mode, stats window need to be removed.
				m_PartyEquipmentBagDialog->ShowCurrentItemDialog( false );
		}
		//return res;
		return FLW_RESULT();
	}

	/*------------------------
		Name choice process
	*/
	FLW_RESULT res = RPGD_NameList::Input_PressedOnce( button );
	if( res.message == "MOVED" )
	{
		res.message.clear();
		DestroyEquipmentListDialog();
		DestroyPartyEquipmentBagDialog();
		if( res.i < (int)m_NameList.size() )
			CreateEquipmentListDialog( m_NameList[res.i] );
		else
			CreatePartyEquipmentBagDialog();
	}
	else if( res.message == "CHOICE" )
	{
		res.message.clear();
		if( res.s == "_PARTY_" ) {
			if( !m_PartyEquipmentBagDialog->Empty() ) {
				m_PartyEquipmentBagDialog->ShowCursor( true );
				//m_PartyEquipmentBagDialog->ShowCurrentItemDialog( false );
				m_PartyEquipmentBagDialog->ShowChosenItemDialog( true );
				m_EquipMode = true;
			}
		}
		else {
			m_EquipmentListDialog->ShowCursor( true );
			m_EquipMode = true;
		}
	}
	else if( res.message == "CANCEL" ) {
		// do nothing. just pass this onto caller.
	}
	res.id = m_sID;
	return res;
}

void RPGD_PMenu_Equipment::CreateEquipmentListDialog( string life_guid )
{
	m_EquipmentListDialog = new RPGD_EquipmentListDialog( m_sID+"_EquipmentEntry", &STATE->GetLife(life_guid)->pnp );
	m_EquipmentListDialog->ShowCursor( false );
	m_EquipmentListDialog->SetPosition( GetLeft()+GetWidth(), GetTop() );
	/*
		WARNING: Setting z order will put equipslot caption behind the overlay (ie. they'll be invisible)
					as RPGD_EquipmentListDialog does not override SetZOrder() to handle caption text elements.
					This can be done at future date if necessary.
	*/
	//m_EquipmentListDialog->SetZOrder( GetZOrder()+1 );
}

void RPGD_PMenu_Equipment::DestroyEquipmentListDialog()
{
	DELETE_NULL( m_EquipmentListDialog );
}

void RPGD_PMenu_Equipment::CreatePartyEquipmentBagDialog()
{
	m_PartyEquipmentBagDialog = new RPGD_PartyEquipmentBagDialog( m_sID+"_PartyEquipmentBag" );
	m_PartyEquipmentBagDialog->SetTextFromInventory();
	m_PartyEquipmentBagDialog->SetZOrder( GetZOrder()+1 );
	m_PartyEquipmentBagDialog->ShowCursor( false );
	m_PartyEquipmentBagDialog->ShowChosenItemDialog( false );
	m_PartyEquipmentBagDialog->ShowCurrentItemDialog( false );
	m_PartyEquipmentBagDialog->SetPosition( GetLeft()+GetWidth(), GetTop()	);
	m_PartyEquipmentBagDialog->AllowEmptySelection( true );
}

void RPGD_PMenu_Equipment::DestroyPartyEquipmentBagDialog()
{
	DELETE_NULL( m_PartyEquipmentBagDialog );
}

