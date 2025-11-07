#ifdef _DEBUG
#pragma warning( disable : 4786 )
#endif

//二重定義の禁止
#ifndef IDCFindFile
#define IDCFindFile

#include "common.h"
#include <windows.h>
#include <algorithm>
#include <iostream>
#include <vector>
#include <String>
#include <functional>

using namespace std;

class CFindFile{
	HANDLE hFindFile;
	WIN32_FIND_DATA FindFileData;
	vector<String> filenames;
	vector<String> pathnames;
	int totalcount;					//?出したファイ??
	int nowcount;					//現在のカウ?ト
	TCHAR searchfilename[MAX_PATH];
	TCHAR searchpath[MAX_PATH];
	void PathToFolderName(TCHAR *p);
public:
	CFindFile(String lpfilename);
	void open(TCHAR* lpfilename);
	int GetFileName(String* filename);
	int GetPathName(String* filename);
};

#endif
