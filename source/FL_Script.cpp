#include "FL_Script.h"

#include <Windows.h>
#include <iostream>
#include <vector>

#include <FL_General.h>
#include <FL_GeneralTools.h>


void print( std::string &msg )
{
	printf( "%s", msg.c_str() );
}


/*----------------------------------------------------------------------
	Callback by FL_Script to prevent script hanging the application.
----------------------------------------------------------------------*/
void LineCallback(asIScriptContext *ctx, DWORD *timeOut)
{
	// If the time out is reached we abort the script
	if( *timeOut < timeGetTime() ) {
		bool Too_Much_Time_Debugging_Time_Out = true;	// change timeout value to something much highter for debugging.
		ctx->Abort();
	}

	// It would also be possible to only suspend the script,
	// instead of aborting it. That would allow the application
	// to resume the execution where it left of at a later 
	// time, by simply calling Execute() again.
}
/*----------------------------------------------------------------------
|-|
	FL_Script

----------------------------------------------------------------------*/
#define LOG_SCRIPT_MESSAGES()													\
			do {																\
				GetScriptMessageIterator()->Begin();							\
				while( GetScriptMessageIterator()->HasMoreElements() )			\
					LOGVAR( *(GetScriptMessageIterator()->GetNext()), true );	\
				GetScriptMessage()->clear();									\
			} while( 0 )

bool FL_Script::CheckScript( std::string filename )
{
	if( !LoadScript(filename) ) {
		m_ScriptMessage.push_back( "ERROR: Script could not be loaded. Ensure the path/filename is correct. The text format must be UTF-8.\n" );
		return false;
	}
	if( !Compile() ) {
		return false;
	}
	return true;
}

void FL_Script::ScriptMessageCallback( const asSMessageInfo *msg )
{
	const char *type = "ERR ";
	if( msg->type == asMSGTYPE_WARNING ) 
		type = "WARN";
	else if( msg->type == asMSGTYPE_INFORMATION ) 
		type = "INFO";

	char buffer[500];
	sprintf_s( buffer, "Script %s : %s (%d, %d) : %s\n", type, msg->section, msg->row, msg->col, msg->message );

	m_ScriptMessage.push_back( std::string(buffer) );
#ifdef SCRIPT_PRINTF
	printf( "%s\n", buffer );
#endif
}
bool FL_Script::_SetLineCallback( asIScriptContext *ctx, DWORD *timeOut )
{
	*timeOut += timeGetTime();
	int r = ctx->SetLineCallback( asFUNCTION(LineCallback), timeOut, asCALL_CDECL );
	//int r = ctx->SetLineCallback( asMETHOD(FL_Script,LineCallback), &fudge, asCALL_THISCALL );
	if( r < 0 )
	{
		m_ScriptMessage.push_back( "Failed to set the LineCallback function.\n" );
#ifdef SCRIPT_PRINTF
		printf( "Failed to set the line callback function.\n" );
#endif
		return false;
	}
	return true;
}

int FL_Script::LoadScript( std::string filename, std::string module_name )
{
	m_ScriptMessage.push_back( std::string("[") + filename + "," + module_name + "] Loading...\n" );
	int r = m_ScriptBuilder.StartNewModule( m_ScriptEngine, module_name.c_str() ); assert( r >= 0 );
	r = m_ScriptBuilder.AddSectionFromFile( filename.c_str() );
	if( r < 0 ) {
		m_ScriptMessage.push_back( std::string("Pre-compile error: loading script file failed (AddSectionFromFile).\n") );
		return -1;
	}
	return 1;
}

bool FL_Script::Compile()
{
	m_ScriptMessage.push_back( "Compiling... " );
	int r = m_ScriptBuilder.BuildModule(); //assert( r>=0 );
	if( r < 0 ) {
		m_ScriptMessage.push_back( std::string("Compiler detected errors.\n") );
		return false;
	}
	return true;
}

/*
// ONE OFF load & compile. Consolidates loading and compiling. Quick convenient method when a full blown script support isn't necessary.
int FL_Script::LoadAndCompileOneOff( std::string filename, std::string module_name =FLSCRIPT_DEFAULT_MODULE_NAME )
{
	int r;
	CScriptBuilder builder; // not using m_Builder as to not interfere.
	r = builder.StartNewModule( m_ScriptEngine, module_name.c_str() ); assert( r >= 0 );
	r = builder.AddSectionFromFile( filename.c_str() );
	if( r < 0 ) {
		m_ScriptMessage.push_back( std::string("Pre-compile error: loading script file failed (AddSectionFromFile).") );
		return 0;
	}
	r = builder.BuildModule(); //assert( r>=0 );
	if( r < 0 ) {
		m_ScriptMessage.push_back( std::string("Compile error: ")+std::string(filename) );
		return -1;
	}
	return 1;
}*/


int FL_Script::RunSCRIPT( const char *fname, asIScriptContext **context, std::string module_name )
{
	if( context == NULL )
		context = &m_ScriptContext;
	if( _RunSCRIPT_Prepare(context,fname,module_name) < 0 )
	{
		LOG_SCRIPT_MESSAGES();
		return 0;
	}
	if( _RunSCRIPT_Execute(context ) < 0 )
	{
		LOG_SCRIPT_MESSAGES();
		return 0;
	}
	return 1;
}

int FL_Script::RunSCRIPT_Int( const char *fname, int arg1, asIScriptContext **context, std::string module_name )
{
	if( context == NULL )
		context = &m_ScriptContext;

	if( _RunSCRIPT_Prepare(context,fname,module_name) < 0 ) {
		LOG_SCRIPT_MESSAGES();
		return 0;
	}

	(*context)->SetArgDWord( 0, arg1 );	// set arguments to pass to callback

	if( _RunSCRIPT_Execute(context) < 0 ) {
		LOG_SCRIPT_MESSAGES();
		return 0;
	}

	return 1;
}

int FL_Script::RunSCRIPT_IntInt( const char *fname, int arg1, int arg2, asIScriptContext **context, std::string module_name )
{
	if( context == NULL )
		context = &m_ScriptContext;

	if( _RunSCRIPT_Prepare(context,fname,module_name) < 0 ) {
		LOG_SCRIPT_MESSAGES();
		return 0;
	}

	(*context)->SetArgDWord( 0, arg1 );	// set arguments to pass to callback
	(*context)->SetArgDWord( 1, arg2 );	// set arguments to pass to callback

	if( _RunSCRIPT_Execute(context) < 0 ) {
		LOG_SCRIPT_MESSAGES();
		return 0;
	}

	return 1;
}

int FL_Script::RunSCRIPT_IntIntStringStringString( const char *fname, int arg1, int arg2, wstring *argStr,
													asIScriptContext **context, std::string module_name )
{
	if( context == NULL )
		context = &m_ScriptContext;

	if( _RunSCRIPT_Prepare(context,fname,module_name) < 0 ) {
		LOG_SCRIPT_MESSAGES();
		return 0;
	}

	string str[3];
	str[0] = FL::UTF16_to_UTF8( argStr[0] );
	str[1] = FL::UTF16_to_UTF8( argStr[1] );
	str[2] = FL::UTF16_to_UTF8( argStr[2] );
	(*context)->SetArgDWord( 0, arg1 );
	(*context)->SetArgDWord( 1, arg2 );
	(*context)->SetArgObject( 2, &str[0] );
	(*context)->SetArgObject( 3, &str[1] );
	(*context)->SetArgObject( 4, &str[2] );

	if( _RunSCRIPT_Execute(context) < 0 ) {
		LOG_SCRIPT_MESSAGES();
		return 0;
	}

	return 1;
}

int FL_Script::RunSCRIPT_IntStringStringString( const char *fname, int arg1, wstring *argStr,
												asIScriptContext **context, std::string module_name )
{
	if( context == NULL )
		context = &m_ScriptContext;

	if( _RunSCRIPT_Prepare(context,fname,module_name) < 0 ) {
		LOG_SCRIPT_MESSAGES();
		return 0;
	}

	string str[3];
	str[0] = FL::UTF16_to_UTF8( argStr[0] );
	str[1] = FL::UTF16_to_UTF8( argStr[1] );
	str[2] = FL::UTF16_to_UTF8( argStr[2] );
	(*context)->SetArgDWord( 0, arg1 );
	(*context)->SetArgObject( 1, &str[0] );
	(*context)->SetArgObject( 2, &str[1] );
	(*context)->SetArgObject( 3, &str[2] );

	if( _RunSCRIPT_Execute(context) < 0 ) {
		LOG_SCRIPT_MESSAGES();
		return 0;
	}

	return 1;
}

int FL_Script::RunSCRIPT_StringStringStringString( const char *fname, std::string *argStr, asIScriptContext **context, std::string module_name )
{
	if( context == NULL )
		context = &m_ScriptContext;

	if( _RunSCRIPT_Prepare(context,fname,module_name) < 0 ) {
		LOG_SCRIPT_MESSAGES();
		return 0;
	}

	(*context)->SetArgObject( 0, &argStr[0] );
	(*context)->SetArgObject( 1, &argStr[1] );
	(*context)->SetArgObject( 2, &argStr[2] );
	(*context)->SetArgObject( 3, &argStr[3] );

	if( _RunSCRIPT_Execute(context) < 0 ) {
		LOG_SCRIPT_MESSAGES();
		return 0;
	}

	return 1;
}

int FL_Script::_RunSCRIPT_Prepare( asIScriptContext **context, const char *fname, std::string module_name )
{
	std::string msg;

	// Running more than 1 context at the same time results in losing the previous context.
	// If the previous context has not ended, this could in theory lead to some unexpected behaviour.
	if( *context ) {
		// m_ScriptContextName may not be accurate here if it's no of the immediately prepared context
		(*context)->GetFunction()->GetName();

		msg = "RunSCRIPT WARN: Previous context [ "; msg += m_ScriptContextName; msg += " ] is in the state of ";
		switch( (*context)->GetState() )
		{
		case asEXECUTION_FINISHED:
			msg += "FINISHED.";			break;
		case asEXECUTION_SUSPENDED:
			msg += "SUSPENDED.";		break;
		case asEXECUTION_ABORTED:
			msg += "ABORTED.";			break;
		case asEXECUTION_EXCEPTION:
			msg += "EXCEPTION.";		break;
		case asEXECUTION_PREPARED:
			msg += "PREPARED.";			break;
		case asEXECUTION_UNINITIALIZED:
			msg += "UNINITIALIZED.";	break;
		case asEXECUTION_ACTIVE:
			msg += "ACTIVE";			break;
		case asEXECUTION_ERROR:
			msg += "ERROR";				break;
		}
		msg += ". Force terminating in order to run [ "; msg += fname; msg += " ].\n";
		m_ScriptMessage.push_back( msg );
#ifdef SCRIPT_PRINTF
		printf( "%s\n", msg.c_str() );
#endif
	}

	int funcId = GetFunctionID( fname, module_name );
	if( funcId < 0 ) {
		msg = std::string("RunSCRIPT error. Function not found: ") + fname; msg += "\n";
		m_ScriptMessage.push_back( msg );
#ifdef SCRIPT_PRINTF
		printf( "%s\n", msg.c_str() );
#endif
		return -1;
	}
	m_ScriptContextName = fname;	// store the function name being for which the context is set
	// Create our context, prepare it, and then execute
	(*context) = m_ScriptEngine->CreateContext();
	DWORD timeOut = SCRIPT_DEFAULT_TIMEOUT;
	if( !_SetLineCallback( (*context), &timeOut ) )
		return 0;
	int r = (*context)->Prepare(funcId);

	switch( r ) {
		case asCONTEXT_ACTIVE:	// 	The context is still active or suspended.
			m_ScriptMessage.push_back( std::string("(") + fname + ") : context->Prepare reports \'CONTEXT ACTIVE\'.\n" );
			break;
		case asNO_FUNCTION:		// 	The function id doesn't exist.
			m_ScriptMessage.push_back( std::string("(") + fname + ") : context->Prepare reports \'NO_FUNCTION\' with funcId=" + FL::LONG2STRING(funcId) + ".\n" );
			break;
		case 0:					// default
			//m_ScriptMessage.push_back( std::string("(") + fname + ") : context->Prepare reports normal operation.\n" );
			break;
		default:
			m_ScriptMessage.push_back( std::string("(") + fname + ") : context->Prepare reports ERROR value (" + FL::LONG2STRING(r) + ").\n" );
			break;
	}

	return r;
}

int FL_Script::_RunSCRIPT_Execute( asIScriptContext **context )
{
	std::string msg;
	int r = (*context)->Execute();
	if( r != asEXECUTION_FINISHED ) {
		// The execution didn't complete as expected. Determine what happened.
		switch( r ) {
		case asERROR:
			msg = "Runtime ERR: context->Execute reports ERROR. [ "; msg += m_ScriptContextName; msg += " ].\n";
			m_ScriptMessage.push_back( msg );
			return -1;
		case asEXECUTION_ABORTED:
			msg = "Runtime ERR: context->Execute reports ABORTED. [ "; msg += m_ScriptContextName; msg += " ].\n";
			m_ScriptMessage.push_back( msg );
			return -1;
		case asEXECUTION_SUSPENDED:
			//msg = "Runtime REPORT: context->Execute reports SUSPENDED. [ "; msg += m_ScriptContextName; msg += " ].\n";
			//m_ScriptMessage.push_back( msg );
			return 1;
		case asEXECUTION_EXCEPTION:
			msg = "Runtime ERR: Exception thrown during execution [ "; msg += m_ScriptContextName; msg += " ].\n";
			m_ScriptMessage.push_back( msg );
#ifdef SCRIPT_PRINTF
			printf( "%s\n", msg.c_str() );
#endif
			return -1;
			// An exception occurred, let the script writer know what happened so it can be corrected.
			//printf("An exception '%s' occurred. Please correct the code and try again.\n", m_ScriptContext->GetExceptionString());
		}
	}
	
	// TODO:	For now, assume that the execution terminated fully.
	//			If suspend support is necessary, edit this part according to return value 'r' from above.
	(*context)->Release();
	(*context) = NULL;
	m_ScriptContextName.clear();

	return 1;
}

