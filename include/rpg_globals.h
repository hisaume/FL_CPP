#ifndef __RPG_GLOBALS_H__
#define __RPG_GLOBALS_H__
/*-----------------------------------------------------------------
|	|FILE|:		rpg_globals.h
|
|	FL RPG Library
|	version 1.12
|------------------------------------------------------------------
	|NOTES|:

-----------------------------------------------------------------*/
#include <vector>

/*
	Event flags
*/
class FlagList
{
public:
	FlagList( int size =100 )			{	flaglist.resize(size,false);	}
	~FlagList()							{}

	void Clear()						{	for( std::vector<bool>::iterator iter=flaglist.begin(); iter!=flaglist.end(); ++iter ) *iter=false;	}
	bool IsUp( int id )					{	if( id<(int)flaglist.size() ) return flaglist[id]; else return false;	}
	void PutUp( int id )				{	if( id<(int)flaglist.size() ) flaglist[id]=true;	}
	/* Use this sparringly. Generally flag should stay up if it goes up, to avoid complication in flag management. */
	void PutDown( int id )				{	if( id<(int)flaglist.size() ) flaglist[id]=false;	}
	std::vector<bool>*	GetFlagList()	{	return &flaglist; }
protected:
	std::vector<bool>	flaglist;
};

extern FlagList		FLAGLIST;


/*--------------------------------

			Messaging

--------------------------------*/

/*

MOVE			byLife		x			y			v
MELEE			byLife		toLife		
PROJECTILE		byLife		toLife		
CAST			byLife		toLife		Spell
USE				byLife		toLife		Item
ADDINVENTORY				toLife		Item

--------------------------------
Moving Object class hierarchy
Move by destination point (x,y)

*/
struct RPG_COMMAND
{
	string		verb;
	string		subject;
	string		target;
	std::deque<int>		i;
	std::deque<string>	s;
};

struct RPG_COMMANDRESULT
{
	RPG_COMMAND	command;
	string		crit_crip;		// empty for none
	string		miss_evade;		// empty for a hit
	int			damage;
};

/*-------------------------------------------------
	RPG_ACTION
	RPG_ACTIONPHASE

	An action is described by RPG_ACTION.
	An action is made up of multiple phases.
	RPG_ACTIONPHASE describes 1 such phase.

		************ Shouldn't this be in "state" file? **************

-------------------------------------------------*/
struct ACTIONPHASE_ELEMENT
{
	FL::FL_Timer	count_before;
	FL::FL_Timer	count_after;
	RPG_COMMAND		command;	// payload command

	/* Helper methods.
		@return -1 if the counter is not active,
				0 if count-down has just completed after 'seconds',
				1 if some duration is still left in the timer.
	*/
	int COUNT_BEFORE( float seconds )
	{
		if( FL::ISZERO(count_before.GetDuration()) )
			return -1;
		if( count_before.Elapsed( seconds ) )
			return 0;
		return 1;
	}
	int COUNT_AFTER( float seconds )
	{
		if( FL::ISZERO(count_after.GetDuration()) )
			return -1;
		if( count_after.Elapsed( seconds ) )
			return 0;
		return 1;
	}
};

struct RPG_ACTIONPHASE
{
	std::deque<ACTIONPHASE_ELEMENT> phaselist;
};

/*----------------------

	extern globals

----------------------*/
extern std::deque<RPG_COMMAND>	COMMAND;

/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif //__RPG_GLOBALS_H__