#ifndef __RPG_SCRIPT_FLRPG_H__
#define __RPG_SCRIPT_FLRPG_H__
/*-----------------------------------------------------------------
|	|FILE|:		rpg_script_flrpg.h
|
|	FL RPG Library
|	version 1.12
|------------------------------------------------------------------
	|NOTES|:

-----------------------------------------------------------------*/
#include <FL_General.h>
#include <FL_Script.h>

#include <stack>



/*-----------------------------------------------------------------------------------------------
	Initialization & Clean up global script pointer
-----------------------------------------------------------------------------------------------*/
void CreateScriptEngine();
void DestroyScriptEngine();

/*-------------------------------------------------
	Script Globals used in and out of script
-------------------------------------------------*/
extern int		g_Script_SYSTEM;
extern int		g_Script_ANSWER;

// Set language as it is set for the game.
void			FL_ScriptSetLanguage( string lang );

/*------------------------------------------
	Event types sent to SCRIPT_Event()

	_AONLIFE might handle all the other variations of player action.

------------------------------------------*/
enum EVENT_TYPE
{
	EVENTTYPE_AONLIFE		=0,
	EVENTTYPE_ITEMONLIFE,
	EVENTTYPE_SPELLONLIFE,
	EVENTTYPE_SKILLONLIFE,

	EVENTTYPE_AONBOX,
	EVENTTYPE_ITEMONBOX,
	EVENTTYPE_SPELLONBOX,
	EVENTTYPE_SKILLONBOX,

	EVENTTYPE_AREA
};

/*-----------------------------------------------------------------------------------------------
|-|		FL_ScriptFLRPG

	Extends FL_Script to be more specific for FLRPG.
-----------------------------------------------------------------------------------------------*/
class FL_ScriptFLRPG : public FL_Script
{
public:
	enum SCRIPTTYPE {
		SCRIPTTYPE_UNDEFINED =0, SCRIPTTYPE_STORY,	SCRIPTTYPE_ZONE, SCRIPTTYPE_BATTLEAI
	};

	FL_ScriptFLRPG()
		: m_ZoneEntryContext(NULL), m_ZoneSetupContext(NULL)
	{
		RegisterFunctions();
		RegisterMethods();
	}
	~FL_ScriptFLRPG()
	{
		ResetZoneScriptContext();
	}
	bool CheckScriptCallbacks( SCRIPTTYPE type );

	void ResetZoneScriptContext()			{	_TerminateScriptContext(m_ZoneSetupContext);
												_TerminateScriptContext(m_ZoneEntryContext);
												while( !m_ZoneEventContext.empty() ) {
													_TerminateScriptContext(m_ZoneEventContext.top());
													m_ZoneEventContext.pop();
												}
												DiscardModule( "Zone" );
											}

	/*--------------------------------------------
				Story script CALLBACKs
	--------------------------------------------*/
	/*	Runs CALLBACK_InitializeResources() in the script file.
		@Returns 0 if error occured. Non-zero if success.	*/
	int RUN_CALLBACK_InitializeResources();
	/*	Runs CALLBACK_Main() in the script file.
		@Returns 0 if error occured. Non-zero if success.	*/
	int RUN_CALLBACK_Main( string message, string Zone, string EventPlane, string Interaction );


	void SuspendZoneEvent()		{ if(m_ZoneEventContext.empty()) return; m_ZoneEventContext.top()->Suspend(); }
	/*	Every call to this method will result in resuming a suspended EVENT execusion. If current EVENT function's execution comes
		to an end, in the following round the next suspended function resumes. This process repeats till all
		functions have ended.
		@Returns 0 if existing script execution has come to an end, for any reason. >0 if still running.
		@REMARK	Supposed to be called every game loop when any script is suspended. If suspend state should remain
				the caller must decide when to call this.
		@REMARK2	In theory, 1 call to this should exhaust the entire stack of functions (unless one gets suspended)
					in a loop. For now, the control is passed back to caller when 1 function returns. It's up to the
					caller to decide when to call this again to resume the consumption of suspended functions. */
	int Go_ZoneEvent()
	{
		if( m_ZoneEventContext.empty() )
			return 0;
		if( m_ZoneEventContext.top() == NULL ) {
			m_ZoneEventContext.pop();
			return 0;	// end this round with just popping.
		}
		int r = m_ZoneEventContext.top()->Execute();	// continue unfinished business.
		switch( r ) {	// return value check.
		case asEXECUTION_SUSPENDED:
			return 1;	// instructed to suspend. return as is.
		case asEXECUTION_ABORTED:
		case asEXECUTION_FINISHED:
			// dont bother reporting.
			break;
		case asEXECUTION_EXCEPTION:
			ERR( L"Go_ZoneEvent", L"Context execusion has thrown exception.\n" );
			break;
		case asERROR:
			ERR( L"Go_ZoneEvent", L"Context execusion has returned an error.\n" );
			break;
		}
		m_ZoneEventContext.top()->Release();
		m_ZoneEventContext.pop();	// end of this context.
		return 0;
	}

protected:
	void RegisterFunctions();
	void RegisterMethods();

protected:
	//SCRIPTTYPE			m_ScriptType;
	// Stack of context. This is because technically its possible for one function to suspend while another function executes.
	std::stack<asIScriptContext*>	m_ZoneEventContext;			// As primary context is used for story, need secondary context for a zone.
	asIScriptContext				*m_ZoneSetupContext;
	asIScriptContext				*m_ZoneEntryContext;
	//asIScriptContext				*m_BattleAIScriptContext;		// Battle A.I. script
};

extern FL_ScriptFLRPG		*gScriptEngine;
#define SCRIPT				gScriptEngine


/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif	//__RPG_SCRIPT_FLRPG_H__