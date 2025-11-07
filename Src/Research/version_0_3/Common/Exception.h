#pragma once
#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <Reuben\System\Reuben.h>
#include <Reuben\Diagnostics\ExceptionManager.h>

class ServiceExceptionHandler : public Reuben::Diagnostics::ExceptionHandler
{
public:

	virtual PCSTRING GetApplicationName(VOID) CONST;
	virtual PCSTRING GetVersionString(VOID) CONST;
	virtual CONST UInt32 GetVersionNumber(VOID) CONST;
	virtual CONST UInt32 GetBuildNumber(VOID) CONST;

};

INIT_EXCEPTION_HANDLER(ServiceExceptionHandler);

#endif // _EXCEPTION_H_
