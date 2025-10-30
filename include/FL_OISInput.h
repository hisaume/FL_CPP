#ifndef __FL_OISINPUT_H__
#define __FL_OISINPUT_H__
/*---------------------------------------------------------------------------
|	|FILE|:    FL_OISInput.h
|
|	FL Library
|	version 2.4
|----------------------------------------------------------------------------
	|NOTES|:

	- OIS Buffered Input
	- Joypad Emulation
	- Key Mapping
---------------------------------------------------------------------------*/
#include <OIS.h>
#include <stdio.h>
#include <deque>
#include <vector>

/*----------------------------------------------------------------------
|-|	Joypad button defines
----------------------------------------------------------------------*/
/*
#define JOYP_UP			0
#define JOYP_DOWN		1
#define JOYP_RIGHT		2
#define JOYP_LEFT		3
#define JOYP_A			4
#define JOYP_B			5
#define JOYP_X			6
#define JOYP_Y			7
#define JOYP_R1			8
#define JOYP_R2			9
#define JOYP_L1			10
#define JOYP_L2			11
#define JOYP_START		12
#define JOYP_SELECT		13
#define JOYP_TOTAL		14
*/

enum eJOYPAD_BUTTON {
	JOYP_UP		= 0	,
	JOYP_DOWN		,
	JOYP_RIGHT		,
	JOYP_LEFT		,
	JOYP_A			,
	JOYP_B			,
	JOYP_X			,
	JOYP_Y			,
	JOYP_R1			,
	JOYP_R2			,
	JOYP_L1			,
	JOYP_L2			,
	JOYP_START		,
	JOYP_SELECT		,
	JOYP_NONE		,
	JOYP_TOTAL
};

/*	Long press duration in seconds i.e. how long a button is
	pressed before it registers as a long press.				*/
#define JP_LONG_PRESS		0.5f


/*----------------------------------------------------------------------
|-| Emulates a joypad style inputs
	On its own it's just a button tracker
	but classes further down below uses it.
----------------------------------------------------------------------*/
class FL_JoypadEmulator
{
public:
	FL_JoypadEmulator()		{	memset( m_Button, 0, sizeof(bool)*JOYP_TOTAL );
								memset( m_Hit, 0, sizeof(bool)*JOYP_TOTAL );
								memset( m_Duration, 0, sizeof(float)*JOYP_TOTAL );
							}
	//----- Setter methods -----
	void	ButtonDown( eJOYPAD_BUTTON id, float secs =0 )
							{
								m_Duration[id] += secs;
								if( !m_Button[id] )
								{
									m_Hit[id] = true;
								}
								m_Button[id] = true;
							}
	void	ButtonUp( eJOYPAD_BUTTON id )
							{
								m_Button[id] = false;
								m_Duration[id] = 0.0f;
								m_Hit[id] = false;
							}
	//----- Setter methods : specialized -----
	// These are the typical direction pad behaviour where opposite directions can't be pressed at the same time.
	void	ButtonDown_UP( float secs =0 )	{
												ButtonUp( JOYP_DOWN );
												ButtonDown( JOYP_UP, secs );
											}
	void	ButtonDown_DOWN( float secs =0 ){
												ButtonUp( JOYP_UP );
												ButtonDown( JOYP_DOWN, secs );
											}
	void	ButtonDown_LEFT( float secs =0 ){
												ButtonUp( JOYP_RIGHT );
												ButtonDown( JOYP_LEFT, secs );
											}
	void	ButtonDown_RIGHT( float secs =0 ){
												ButtonUp( JOYP_LEFT );
												ButtonDown( JOYP_RIGHT, secs );
											}
	//----- Getter methods -----
	/** @Returns true if button is down, but this can only be tested once. Subsequent calls
		results in false until the button is raised and depressed again. */
	bool	IsHit( eJOYPAD_BUTTON id )		{
												if( m_Hit[id] ) {
													m_Hit[id] = false;
													return true;
												}
												return false;
											}
	bool	IsDown( eJOYPAD_BUTTON id ) const		{	return m_Button[id]; }
	bool	IsDown( eJOYPAD_BUTTON id, float &duration ) const		{ duration = m_Duration[id]; return m_Button[id]; }
	// True if the button has been down for long (JP_LONG_PRESS)
	bool	IsDownLong( eJOYPAD_BUTTON id ) const	{
										if( m_Duration[id] >= JP_LONG_PRESS )
											return true;
										return false;
									}
	void	FrameStarted( float timeSinceLast )
	{
		for( int i=0; i<JOYP_TOTAL; ++i )
			if( m_Button[i] )	// If button is down...
				m_Duration[i] += timeSinceLast;
	}
protected:
	bool		m_Button[JOYP_TOTAL];		// up or down
	bool		m_Hit[JOYP_TOTAL];			// If a button has been 'hit'. It can only be counted once.
	float		m_Duration[JOYP_TOTAL];		// how long a button is held down in seconds
};

/*----------------------------------------------------------------------
|-| OIS KeyCode mapped version of Joypad emulator
----------------------------------------------------------------------*/
class FL_JoypadEmulatorKeyMap : public FL_JoypadEmulator
{
public:
	//----- KeyCode Map methods -----
	void	SetDefaultMap();
	void	MapKey( OIS::KeyCode kc, eJOYPAD_BUTTON b );
	bool	Exist( OIS::KeyCode id ) const;					// Returns true if the given KeyCode is mapped.
	int		SearchKeyCode( eJOYPAD_BUTTON b ) const;			// Returns the KeyCode assigned to the joypad button. Returns -1 if nonexistent.
	int		GetJoypadButton( OIS::KeyCode kc ) const		{ return m_KeyMap.at(kc); }
	//----- Setter methods -----
	void	KeyDown( OIS::KeyCode id, float secs =0 );
	void	KeyUp( OIS::KeyCode id )	{
											if( !Exist(id) )	return;
											FL_JoypadEmulator::ButtonUp( m_KeyMap[id] );
										}
	//----- Getter methods -----
/*	bool	IsHit( OIS::KeyCode id )	{
											if( !Exist(id) )	return false;
											return FL_JoypadEmulator::IsHit( m_KeyMap[id] );
										}
	bool	IsDown( OIS::KeyCode id )	{
											if( !Exist(id) )	return false;
											return FL_JoypadEmulator::IsDown( m_KeyMap[id] );
										}
	bool	IsDown( OIS::KeyCode id, float &duration )
										{
											if( !Exist(id) )	return false;
											return FL_JoypadEmulator::IsDown( m_KeyMap[id], duration );
										}
	// True if the button has been down for long (JP_LONG_PRESS)	
	bool	IsDownLong( OIS::KeyCode id )	{
												if( !Exist(id) )	return false;
												return FL_JoypadEmulator::IsDownLong( m_KeyMap[id] );
											}
*/
protected:
	std::map< OIS::KeyCode , eJOYPAD_BUTTON >	m_KeyMap;
	std::deque< OIS::KeyCode >					m_KeyExist;
};

/*----------------------------------------------------------------------
|-|	OIS Buffered Input wrapper

	NOTE1 : On deriving a class from this...
		If the listener methods (e.g. keyPressed()) is overriden in the
		derived class, this class's listener methods must be explicitly
		called in order to use Joypad emulation. Otherwise m_Joypad is not
		updated.
	e.g.
		bool DerivedClass::keyPressed( const OIS::KeyEvent &e )
		{
			FL_BufferedInput::keyPressed( e );	// explicitly call this class's listner

			// Do other input handling
			// ...

			return true;
		}

	NOTE2 : capture()
		This class does not call capture() to capture input. That MUST be done by the
		derived class every frame. Otherwise the callback to listeners will never happen.

----------------------------------------------------------------------*/
class FL_BufferedInput : public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener
{
public:
	FL_BufferedInput() : m_JoyCode( 10, JOYP_NONE ), m_bLMouseDown(false), m_bRMouseDown(false)
							{
								m_Joypad.SetDefaultMap();	// maps the keys by default
								SetDefaultJoypadMap();
							}
	void CreateBufferedInput( OIS::Keyboard *key, OIS::Mouse *mouse =0, OIS::JoyStick *joy =0 )
												{
													if( key )
														key->setEventCallback( this );
													if( mouse )
														mouse->setEventCallback( this );
													if( joy ) {
														joy->setEventCallback( this );
														m_AxisThreshold = joy->MAX_AXIS / 2;
													}
												}
protected:
	//----- Keyboard listeners -----
	virtual bool keyPressed( const OIS::KeyEvent &e );
	virtual bool keyReleased( const OIS::KeyEvent &e );
	//----- Mouse listeners -----
	virtual bool mouseMoved( const OIS::MouseEvent &e );
	virtual bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );
	//----- Joypad listeners -----
	virtual bool buttonPressed( const OIS::JoyStickEvent &e, int button );
	virtual bool buttonReleased( const OIS::JoyStickEvent &e, int button );
	virtual bool axisMoved( const OIS::JoyStickEvent &e, int axis );

	/*	Physical joypad (USB or else) returns a number when a button is pressed.
		Which button corresponds to which eJOYPAD_BUTTON code should be customizable.	*/
	void SetDefaultJoypadMap();
	void MapJoypadButton( int button, eJOYPAD_BUTTON jb );
	// Returns the eJOYPAD_BUTTON code corresponding to the hardware button
	eJOYPAD_BUTTON	GetJoyCode( int button );
	// Parent must call this every loop if the key press duration feature is to be used. timeSinceLast is in seconds.
	void FrameStarted( float timeSinceLast )
	{
		m_Joypad.FrameStarted( timeSinceLast );
	}

protected:
	FL_JoypadEmulatorKeyMap			m_Joypad;
	std::vector<eJOYPAD_BUTTON>		m_JoyCode;			// mapping hardware code to eJOYPAD_BUTTON code
	int								m_AxisThreshold;	// The value beyond which a keypress is recognized on the joystick

	bool	m_bLMouseDown, m_bRMouseDown;

	enum _AXIS	{ ZERO, POS, NEG };						// used in axisMoved()
};

/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif	//__FL_OISINPUT_H__