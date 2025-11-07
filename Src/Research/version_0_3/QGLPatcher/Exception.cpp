#include "Common.h"
#include "Exception.h"
#include "Version.h"

PCSTRING ServiceExceptionHandler::GetApplicationName(VOID) CONST
{
	return (_T("Battle Service"));
}

PCSTRING ServiceExceptionHandler::GetVersionString(VOID) CONST
{
	return (VERSION_STRING);
}

CONST UInt32 ServiceExceptionHandler::GetVersionNumber(VOID) CONST
{
	return (VERSION_NUMBER);
}

CONST UInt32 ServiceExceptionHandler::GetBuildNumber(VOID) CONST
{
	return (VERSION_BUILD);
}
