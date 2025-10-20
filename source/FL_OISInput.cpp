#include <FL_OISInput.h>

/*----------------------------------------------------------------------
|-|	FL_JoypadEmulatorKeyMap
----------------------------------------------------------------------*/

void FL_JoypadEmulatorKeyMap::SetDefaultMap()
{
	MapKey( OIS::KC_UP,		JOYP_UP );
	MapKey( OIS::KC_DOWN,	JOYP_DOWN );
	MapKey( OIS::KC_LEFT,	JOYP_LEFT );
	MapKey( OIS::KC_RIGHT,	JOYP_RIGHT );

	MapKey( OIS::KC_A,	JOYP_A );
	MapKey( OIS::KC_B,	JOYP_B );
	MapKey( OIS::KC_X,	JOYP_X );
	MapKey( OIS::KC_Y,	JOYP_Y );

	MapKey( OIS::KC_Q,	JOYP_L1 );
	MapKey( OIS::KC_1,	JOYP_L2 );
	MapKey( OIS::KC_W,	JOYP_R1 );
	MapKey( OIS::KC_2,	JOYP_R2 );

	MapKey( OIS::KC_RETURN,		JOYP_START );
	MapKey( OIS::KC_LCONTROL,	JOYP_SELECT );
}

void FL_JoypadEmulatorKeyMap::MapKey( OIS::KeyCode kc, eJOYPAD_BUTTON b )
{
	if( !Exist(kc) )	// if its not mapped yet...
	{
		m_KeyExist.push_back(kc);	// add to keycode tracker
	}
	m_KeyMap[kc] = b;
}

// Returns true if the given KeyCode is mapped.
bool FL_JoypadEmulatorKeyMap::Exist( OIS::KeyCode id )
{
	std::deque<OIS::KeyCode>::iterator iter;
	for( iter=m_KeyExist.begin(); iter!=m_KeyExist.end(); ++iter )
		if( (*iter) == id )
			return true;
	return false;
}

//----- Setter methods -----
void FL_JoypadEmulatorKeyMap::KeyDown( OIS::KeyCode id, float secs )
{
	if( !Exist(id) )
		return;

	switch( m_KeyMap[id] )
	{
	case JOYP_UP:
		ButtonDown_UP( secs );	break;
	case JOYP_DOWN:
		ButtonDown_DOWN( secs ); break;
	case JOYP_LEFT:
		ButtonDown_LEFT( secs ); break;
	case JOYP_RIGHT:
		ButtonDown_RIGHT( secs ); break;
	default:
		FL_JoypadEmulator::ButtonDown( m_KeyMap[id], secs ); break;
	}
}

int FL_JoypadEmulatorKeyMap::SearchKeyCode( eJOYPAD_BUTTON b )
{
	std::map<OIS::KeyCode,eJOYPAD_BUTTON>::iterator iter;
	for( iter=m_KeyMap.begin(); iter!=m_KeyMap.end(); ++iter )
	{
		if( (*iter).second == b )
			return (*iter).first;
	}
	return -1;	// not found.
}


/*------------------------------------------------------------------------------------
[]	FL_BufferedInput
------------------------------------------------------------------------------------*/
bool FL_BufferedInput::keyPressed( const OIS::KeyEvent &e )
{
	m_Joypad.KeyDown( e.key );
	return true;
}
bool FL_BufferedInput::keyReleased( const OIS::KeyEvent &e )
{
	m_Joypad.KeyUp( e.key );
	return true;
}
bool FL_BufferedInput::mouseMoved( const OIS::MouseEvent &e )
{
	return true;
}
bool FL_BufferedInput::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id )
{
	if( id == OIS::MB_Right )
		m_bRMouseDown = true;
	if( id == OIS::MB_Left )
		m_bLMouseDown = true;

	return true;
}
bool FL_BufferedInput::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id )
{
	if( id == OIS::MB_Right )
		m_bRMouseDown = false;
	if( id == OIS::MB_Left )
		m_bLMouseDown = false;

	return true;
}
bool FL_BufferedInput::buttonPressed( const OIS::JoyStickEvent &e, int button )
{
	m_Joypad.ButtonDown( GetJoyCode(button) );
	return true;
}
bool FL_BufferedInput::buttonReleased( const OIS::JoyStickEvent &e, int button )
{
	m_Joypad.ButtonUp( GetJoyCode(button) );
	return true;
}
bool FL_BufferedInput::axisMoved( const OIS::JoyStickEvent &e, int axis )
{
	int val		= e.state.mAxes[axis].abs;	// absolute value of the axis
	_AXIS ax = ZERO;
	if( val > m_AxisThreshold )			ax = POS;
	else if( val < -m_AxisThreshold )	ax = NEG;
	else								ax = ZERO;

	switch( axis )
	{
	case 0:	// UP+ / DOWN-
		switch( ax )
		{
		case POS:
			m_Joypad.ButtonDown_UP();	break;
		case NEG:
			m_Joypad.ButtonDown_DOWN();	break;
		case ZERO:
			m_Joypad.ButtonUp( JOYP_UP );
			m_Joypad.ButtonUp( JOYP_DOWN );		break;
		}
		break;
	case 1: // RIGHT+ / LEFT-
		switch( ax )
		{
		case POS:
			m_Joypad.ButtonDown_RIGHT();	break;
		case NEG:
			m_Joypad.ButtonDown_LEFT();		break;
		case ZERO:
			m_Joypad.ButtonUp( JOYP_RIGHT );
			m_Joypad.ButtonUp( JOYP_LEFT );		break;
		}
		break;
	case 2: // TWIST RIGHT+ / TWIST LEFT-
		switch( ax ) {
		case POS:	break;
		case NEG:	break;
		case ZERO:	break;
		}
		break;
	}

	return true;
}

void FL_BufferedInput::SetDefaultJoypadMap()
{
	MapJoypadButton( 0,		JOYP_A );
	MapJoypadButton( 1,		JOYP_B );
	MapJoypadButton( 2,		JOYP_X );
	MapJoypadButton( 3,		JOYP_Y );

	MapJoypadButton( 4,		JOYP_L1 );
	MapJoypadButton( 5,		JOYP_L2 );
	MapJoypadButton( 6,		JOYP_R1 );
	MapJoypadButton( 7,		JOYP_R2 );

	MapJoypadButton( 8,		JOYP_START );
	MapJoypadButton( 9,		JOYP_SELECT );
}

void FL_BufferedInput::MapJoypadButton( int button, eJOYPAD_BUTTON jb )
{
	int size = m_JoyCode.size();

	if( (button+1) < size )	// element already exist
	{
		m_JoyCode[button] = jb;
	}
	else {
		for( int i=size; i<(button+1); ++i )
		{
			m_JoyCode.push_back( JOYP_NONE );
		}
		m_JoyCode[button] = jb;
	}
}

eJOYPAD_BUTTON FL_BufferedInput::GetJoyCode( int button )
{
	if( (unsigned int)(button+1) < m_JoyCode.size() )	// if element exists
		return m_JoyCode[button];
	else
		return JOYP_NONE;
}
