#ifdef _DEBUG
#pragma warning( disable : 4786 )
#endif
#include "CFindFile.h"

CFindFile::CFindFile(String lpfilename){
	TCHAR *buf;
	buf = (TCHAR*)malloc((lpfilename.GetLength() + 1) * sizeof(TCHAR));
	_tcscpy(buf,lpfilename.c_str());
	open(buf);
	free(buf);
}

void CFindFile::open(TCHAR* lpfilename){
	TCHAR buf[MAX_PATH];
	hFindFile = NULL;
	String filename,temp;
	memset((void*)&FindFileData,0,sizeof(WIN32_FIND_DATA));
	_tcscpy(searchfilename,lpfilename);
	_tcscpy(searchpath,searchfilename);
	PathToFolderName(searchpath);
	nowcount = 1;
	totalcount = 0;

	hFindFile = FindFirstFile(searchfilename,&FindFileData);
	DWORD abc = GetLastError();
	if (hFindFile == INVALID_HANDLE_VALUE) return;

	filename = FindFileData.cFileName;
	if( _tcscmp(filename.c_str(),_T(".")) != 0 && _tcscmp(filename.c_str(),_T("..")) != 0){
		_tcscpy(buf,searchpath);
		_tcscat(buf,_T("\\"));
		_tcscat(buf,filename.c_str());
		temp = buf;
		filenames.push_back(filename);
		pathnames.push_back(temp);
	}

	while (FindNextFile(hFindFile,&FindFileData) != 0){
		filename = FindFileData.cFileName;
		if( _tcscmp(filename.c_str(),_T(".")) != 0 && _tcscmp(filename.c_str(),_T("..")) != 0){
			_tcscpy(buf,searchpath);
			_tcscat(buf,_T("\\"));
			_tcscat(buf,filename.c_str());
			temp = buf;
			filenames.push_back(filename);
			pathnames.push_back(temp);
		}
	}

	totalcount = (int)filenames.size();

	sort( filenames.begin(), filenames.end() , std::less<String>());
	sort( pathnames.begin(), pathnames.end() , std::less<String>());

	if(hFindFile != NULL && hFindFile != INVALID_HANDLE_VALUE){
		FindClose(hFindFile);
	}
}

int CFindFile::GetFileName(String* filename)
{
	if (nowcount > totalcount) return 0;
	*filename = filenames[nowcount-1];
	nowcount++;
	return 1;
}

int CFindFile::GetPathName(String* filename)
{
	if (nowcount > totalcount) return 0;
	*filename = pathnames[nowcount-1];
	nowcount++;
	return 1;
}

void CFindFile::PathToFolderName(TCHAR *p){
	int i,len,enp = -1;
	len = (int)_tcslen(p);
	for (i = len - 1; i >= 0 ; i--){
		if (*(p + i) == '\\'){
			enp = i;
			break;
		}
	}
	if (enp == -1) return;
	*(p + enp) = '\0';
}