#pragma once
#ifndef _MY_STACKWALKER_H_
#define _MY_STACKWALKER_H_

#include "Common.h"
#include "Common/Stackwalker.h"

class MyStackWalker : public StackWalker,
	public Reuben::Utility::Singleton<MyStackWalker>
{
public:
	MyStackWalker() : StackWalker(RetrieveNone|RetrieveSymbol|RetrieveLine) {}

protected:
  virtual void OnOutput(LPCSTR szText);

private:
	SINGLETON(MyStackWalker);
};

INLINE void MyStackWalker::OnOutput(LPCSTR szText)
{
	WriteLog(LOG_LV_VERBOSE, String(szText).c_str());
}

INLINE VOID ShowCallstack()
{
	MyStackWalker::GetInstance().ShowCallstack();
}

#endif // _MY_STACKWALKER_H_