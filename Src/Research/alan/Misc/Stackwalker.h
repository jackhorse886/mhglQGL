/*////////////////////////////////////////////////////////////////////////////
 *  Project:
 *    Memory_and_Exception_Trace
 *
 * ///////////////////////////////////////////////////////////////////////////
 *  File:
 *    Stackwalker.h
 *
 *  Remarks:
 *
 *
 *  Note:
 *
 *
 *  Author:
 *    Jochen Kalmbach
 *
 *////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef __STACKWALKER_H__
#define __STACKWALKER_H__

// Comment to turn off running stack walker
//#define RUN_STACK_WALKER

#if defined(RUN_STACK_WALKER) && defined(COMPILER_DEBUG)
// Only valid in the following environment: Intel platform, MS VC++ 5/6/7/7.1/8
#ifndef _X86_
#error Only INTEL envirnoments are supported!
#endif

// Only MS VC++ 5 to 7
#if (_MSC_VER < 1100) || (_MSC_VER > 1400)
#error Only MS VC++ 5/6/7/7.1/8 supported. Check if the '_CrtMemBlockHeader' has not changed with this compiler!
#endif

typedef enum eAllocCheckOutput
{
  ACOutput_Simple,
  ACOutput_Advanced,
  ACOutput_XML
};

// Make extern "C", so it will also work with normal C-Programs
#ifdef __cplusplus
extern "C" {
#endif
extern int InitAllocCheckWN(eAllocCheckOutput eOutput, LPCTSTR pszFilename, ULONG ulShowStackAtAlloc = 0);
extern int InitAllocCheck(eAllocCheckOutput eOutput = ACOutput_Simple, BOOL bSetUnhandledExeptionFilter = TRUE, ULONG ulShowStackAtAlloc = 0);  // will create the filename by itself

extern ULONG DeInitAllocCheck();

extern DWORD StackwalkFilter( EXCEPTION_POINTERS *ep, DWORD status, LPCTSTR pszLogFile);

extern void OnlyInstallUnhandeldExceptionFilter(eAllocCheckOutput eOutput = ACOutput_Simple);

#ifdef __cplusplus
}
#endif

#else // RUN_STACK_WALKER

#define InitAllocCheck(X)
#define DeInitAllocCheck()

#endif
#endif  // __STACKWALKER_H__
