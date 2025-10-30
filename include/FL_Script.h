#ifndef __FL_SCRIPT_H__
#define __FL_SCRIPT_H__
/*---------------------------------------------------------------------------
|	|FILE|:    FL_Script.h
|
|	FL Library
|	version 2.4

---------------------------------------------------------------------------*/
#include "angelscript.h"
#include "scriptbuilder.h"
#include "scriptstdstring.h"
#include "scriptarray.h"

#include <windows.h>	// timeGetTime()
#include <assert.h>
#include <stdio.h>
#include <string>
#include <deque>


// Default timeout in milli seconds. After this time the script will terminate to prevent a hang.
#define SCRIPT_DEFAULT_TIMEOUT	30000
// Name of the default module internally used by FL_Script
#define FLSCRIPT_DEFAULT_MODULE_NAME	"DefaultModule"


// If SCRIPT_PRINTF is defined, printf() is used to print out system messages.
// Likely useless in a release version so comment it out when necessary.
//#define SCRIPT_PRINTF


// Print the script string to the standard output stream
void print( const std::string &msg);

/*----------------------------------------------------------------------
|-|
		FL_Script

----------------------------------------------------------------------*/
// Used by FL_Script as a callback to force terminate a script run. Prevents script hanging.
void LineCallback(asIScriptContext *ctx, DWORD *timeOut);

class FL_Script
{
	//----- Script Message Iterator -----
	struct ScriptMessageIterator
	{
		ScriptMessageIterator( std::deque<std::string> *parent )		{ _parent = parent; }
		void Begin()				{ _iter = _parent->begin(); }
		std::string* GetNext()		{ std::deque<std::string>::iterator tmp=_iter; ++_iter; return &(*tmp); }
		bool HasMoreElements()		{ return _iter != _parent->end(); }
		unsigned int Size()			{ return _parent->size(); }
		std::deque<std::string>				*_parent;
		std::deque<std::string>::iterator	_iter;
	};
public:
	FL_Script()
		: m_ScriptEngine(NULL), m_ScriptContext(NULL)
											{
												m_ScriptMessageIterator = new ScriptMessageIterator( &m_ScriptMessage );
												CreateScriptEngine();
											}
	virtual ~FL_Script()					{ DestroyScriptEngine(); delete m_ScriptMessageIterator; }

	void CreateScriptEngine()
	{
		if( m_ScriptEngine )
			return;
		m_ScriptEngine = asCreateScriptEngine( ANGELSCRIPT_VERSION );
		int res;
		res = m_ScriptEngine->SetMessageCallback( asMETHOD(FL_Script,ScriptMessageCallback), this, asCALL_THISCALL ); assert( res >= 0 );
		//m_ScriptEngine->SetEngineProperty(asEP_STRING_ENCODING, 1);
		RegisterStdString(m_ScriptEngine);
		RegisterScriptArray(m_ScriptEngine, true);
		RegisterFunctions();
		RegisterMethods();
	}
	void DestroyScriptEngine()						{ if(m_ScriptEngine){ m_ScriptEngine->Release(); m_ScriptEngine=NULL; } }

	/** Resets this class object without actually freeing the script engine or registered functions.
	*/
	void Reset()	{ _TerminateScriptContext(m_ScriptContext); DiscardModule(FLSCRIPT_DEFAULT_MODULE_NAME); }

	int		DiscardModule( const std::string& s )			{ return m_ScriptEngine->DiscardModule(s.c_str()); }
	// Standard compile test. Used by tools to check grammer/symantics to inform the script writer of errors.
	bool	CheckScript( const std::string& filename );
	/*	Standard call to load and compile a script file in 1 visit.
		@Returns 1 if success. */
	int		LoadAndCompile( const std::string& filename, const std::string& module_name =FLSCRIPT_DEFAULT_MODULE_NAME )
													{
														if( LoadScript(filename,module_name)<0 )	return -1;
														if( !Compile() )	return -1;
														return 1;
													}
	/*
		As a general rule, call LoadScript() followed by Compile() unless there is a good reason to do otherwise;
		These 2 methods are tied with m_ScriptBuilder, so alteration to the builder in between is troublesome.
	*/
	// Loads a script, but does not compile. Call Compile() immediately after, unless for error checks.
	int		LoadScript( const std::string& filename, const std::string& module_name =FLSCRIPT_DEFAULT_MODULE_NAME );
	// Compiles what's been loaded immediately before. Call LoadScript to actually load a script.
	bool	Compile();

	//----- Messages -----
	std::deque<std::string>* GetScriptMessage()				{ return &m_ScriptMessage; }
	ScriptMessageIterator* GetScriptMessageIterator()		{ return m_ScriptMessageIterator; }

	//----- Execution -----
	/*
		If asIScriptContext is not given, default m_ScriptContext is used.
		If module name is not given, FLSCRIPT_DEFAULT_MODULE is used.
		For running multiple scripts at once, both must be given by the caller to differentiate between the scripts.
	*/
	// Primary access to running a 'callback' function with no parameter in the script. Returns 1 if success, less if not.
	int		RunSCRIPT( const char *function_name, asIScriptContext **context =NULL, const std::string& module_name =FLSCRIPT_DEFAULT_MODULE_NAME );
	// Run a function of the name 'function_name' with an integer parameter arg1
	int		RunSCRIPT_Int( const char *function_name, int arg1, asIScriptContext **context =NULL, const std::string& module_name =FLSCRIPT_DEFAULT_MODULE_NAME );
	// Run a function of the name 'function_name' with 2 integer parameters arg1 and arg2
	int		RunSCRIPT_IntInt( const char *function_name, int arg1, int arg2, asIScriptContext **context =NULL, const std::string& module_name =FLSCRIPT_DEFAULT_MODULE_NAME );
	// Run a function of the name 'function_name' with 1 integer and 3 strings
	int		RunSCRIPT_IntStringStringString( const char *function_name, int arg1, std::wstring *argStr, asIScriptContext **context =NULL, const std::string& module_name =FLSCRIPT_DEFAULT_MODULE_NAME );
	// Run a function of the name 'function_name' with 2 integers and 3 strings
	int		RunSCRIPT_IntIntStringStringString( const char *function_name, int arg1, int arg2, std::wstring *argStr, asIScriptContext **context =NULL, const std::string& module_name =FLSCRIPT_DEFAULT_MODULE_NAME );
	int		RunSCRIPT_StringStringStringString( const char *function_name, std::string *argStr, asIScriptContext **context =NULL, const std::string& module_name =FLSCRIPT_DEFAULT_MODULE_NAME );
	// Runs main() from the primary script context, which is typical of most programs. Returns 0 or less if error.
	int		RunMain()		{	return RunSCRIPT( "void main()" );	}

protected:
	// Script engine messages callback for engine-level messages
	void	FL_Script::ScriptMessageCallback( const asSMessageInfo *msg );
	// Callback for context execution
	bool	_SetLineCallback( asIScriptContext *ctx, DWORD *timeOut );
	// Returns less than 0 if the function_name is not found in the script
	int		GetFunctionID( const std::string& function_name, const std::string& module_name =FLSCRIPT_DEFAULT_MODULE_NAME )
			{
				asIScriptModule *mod = m_ScriptEngine->GetModule( module_name.c_str() );
				return mod->GetFunctionIdByDecl( function_name.c_str() );
			}
	int		GetVariableID( const std::string& var_name, const std::string& module_name =FLSCRIPT_DEFAULT_MODULE_NAME )
			{
				asIScriptModule *mod = m_ScriptEngine->GetModule( module_name.c_str() );
				return mod->GetTypeIdByDecl( var_name.c_str() );
			}
	void	RegisterFunctions()		{	int r = m_ScriptEngine->RegisterGlobalFunction("void print( const string &in )",asFUNCTION(print), asCALL_CDECL); assert( r >= 0 ); }
	void	RegisterMethods()		{}

	// Prepares m_ScriptContext
	int		_RunSCRIPT_Prepare( asIScriptContext **context, const char *func_name, const std::string& module_name );
	// Exeuctes m_ScriptContext
	int		_RunSCRIPT_Execute( asIScriptContext **context );
	// Forcifully terminates the current context execution
	void	_TerminateScriptContext( asIScriptContext *context )
										{	if( context==NULL ) return;
											context->Release();
											context = NULL;
											m_ScriptContextName.clear();
										}
protected:
	asIScriptEngine			*m_ScriptEngine;
	asIScriptContext		*m_ScriptContext;		// Primary script context
	std::string				m_ScriptContextName;	// context name of immediate previously prepared context. for messaging.
	CScriptBuilder			m_ScriptBuilder;

	std::deque<std::string>	m_ScriptMessage;
	ScriptMessageIterator*	m_ScriptMessageIterator;
};

/*------------------------------------------------
|	END OF FILE
------------------------------------------------*/
#endif