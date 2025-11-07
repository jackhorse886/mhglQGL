//-- Common
#include "Common.h"

static PCSTRING gServiceTypeString[] =
{
	_T(""),
	_T("Log"),
	_T("World"),
	_T("Object"),
	_T("Message"),
	_T("Manager"),
	_T("Proxy"),
	_T("Login"),
	_T("Map"),
	_T("Battle")
};

CONST ServiceType StringToServiceType(PCSTRING pType)
{
	for (Index i = C_INDEX(0); i < SERVICE_TYPE_COUNT; ++i)
	{
		if (::_tcsicmp(gServiceTypeString[i], pType) == 0)
		{
			return ((ServiceType)i);
		}
	}
	return (SERVICE_NONE);
}

PCSTRING ServiceTypeToString(CONST ServiceType eType)
{
	if (eType > SERVICE_NONE && eType < SERVICE_TYPE_COUNT)
		return (gServiceTypeString[(Int)eType]);
	return (gServiceTypeString[0]);
}

Boolean IsFileExist(PCSTRING pFilename)
{
	DWORD attr = GetFileAttributes(pFilename);
	return (attr != INVALID_FILE_ATTRIBUTES && attr != FILE_ATTRIBUTE_DIRECTORY);
}

/////// Logger Functions ///////
Logger* gGlobalLogger = NULL;

Logger& GetGlobalLogger(VOID)
{
	return (*gGlobalLogger);
}
