// MakePatchFilelist.cpp : Defines the entry point for the console application.
//

#include "common.h"

#include "stdio.h"
#include <Reuben/Platform/File.h>
#include "Reuben/Math/CRC.h"

typedef Reuben::Platform::File File;
typedef Reuben::Platform::FileException FileException;

#define STRING_CURRENT_DIR _T(".\\*.*")
#define STRING_CONFIG_FILE _T("MakePatchFilelist_config.ini")
#define STRING_OUTPUT_INI_FILE _T("Filelist.txt")
#define STRING_CONFIG_FILE_INI_FILE _T("MakePatchFilelist.ini")
#define STRING_OUTPUT_XML_FILE _T("Filelist.xml")
#define STRING_OUTPUT_LOG _T("MakePatchFilelist")

struct FData
{
	String name;
	UInt64 size;
	UInt32 crc;
	FILETIME filetime;
};

typedef StlVector<FData, ManagedAllocator<FData> > FileDataList;
typedef StlVector<String, ManagedAllocator<String> > FilterList;

void InitLogger();
void CloseLogger();
BOOL GenerateFilelist(LPCTSTR targetDir);
void DisplayUsage();
void ExportXML();

int _tmain(int argc, _TCHAR* argv[])
{
	TCHAR defaultDir[2];
	TCHAR* targetDir;
	_stprintf(defaultDir, _T("."));
	if( argc < 2 )
		targetDir = defaultDir;
	else
		targetDir = argv[1];

	InitLogger();

	DisplayUsage();
	_tprintf( _T("Processing...") );

	if( GenerateFilelist(targetDir) )
	{
		_tprintf( _T("Filelist \"%s\" is output successfully\n"), STRING_OUTPUT_INI_FILE );
		_tprintf( _T("Filelist \"%s\" is output successfully\n"), STRING_OUTPUT_XML_FILE );
	}else
	{
		_tprintf( _T("Filelist \"%s\" is failed to output\n"), STRING_OUTPUT_INI_FILE );
		_tprintf( _T("Filelist \"%s\" is failed to output\n"), STRING_OUTPUT_XML_FILE );
	}

	CloseLogger();

	return 0;
}

void InitLogger()
{
	Reuben::Utility::ConfigFile config(STRING_CONFIG_FILE);
	gGlobalLogger = SafeCreateObject2(Logger, STRING_OUTPUT_LOG, STRING_CONFIG_FILE);
	gGlobalLogger->Open(config);

	// set object manager logger
	Reuben::Simulation::SetObjectLogger(gGlobalLogger);
}

void CloseLogger()
{
	SafeDeleteObject(gGlobalLogger);
}

BOOL GenerateFilelist(LPCTSTR targetDir)
{
	SetCurrentDirectory(targetDir);

	BOOL succeed = TRUE;
	FileDataList filelist;
	filelist.clear();

	// prepare filter list
	FilterList filterlist;
	filterlist.clear();
	{
		String tempfile, tempbuf;
		tempfile = STRING_OUTPUT_INI_FILE;
		filterlist.push_back(tempfile);
		tempfile = STRING_OUTPUT_XML_FILE;
		filterlist.push_back(tempfile);
		tempfile = STRING_CONFIG_FILE_INI_FILE;
		filterlist.push_back(tempfile);
		tempfile = _T(".");
		filterlist.push_back(tempfile);
		tempfile = _T("..");
		filterlist.push_back(tempfile);
		tempfile.Format(_T("%s.log"), STRING_OUTPUT_LOG);
		filterlist.push_back(tempfile);
		TCHAR appname[MAX_PATH];
		GetModuleFileName(0, appname, MAX_PATH);
		tempbuf = appname;
		size_t pos = tempbuf.find_last_of(_T("\\"));
		tempfile = tempbuf.Right(tempbuf.GetLength()-(int)pos-1);
		filterlist.push_back(tempfile);
	}

	// loop of files
	WIN32_FIND_DATA FileData; 
	HANDLE hSearch = FindFirstFile(STRING_CURRENT_DIR, &FileData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		BOOL fFinished = FALSE;
		while (!fFinished) 
		{ 
			// do filtering
			BOOL skipThis = FALSE;
			if( FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				skipThis = TRUE;
			}else
			{
				String inputfile;
				// date check
				inputfile.Format(_T(".\\%s"), STRING_CONFIG_FILE_INI_FILE);
				DWORD datehigh, datelow;
				datehigh	= GetPrivateProfileInt(FileData.cFileName, _T("datehigh"), 0, inputfile.c_str());
				datelow		= GetPrivateProfileInt(FileData.cFileName, _T("datelow"), 0, inputfile.c_str());
				// size check
				inputfile.Format(_T(".\\%s"), STRING_OUTPUT_INI_FILE);
				TCHAR tempInt[MAX_PATH];
				ZeroMemory(tempInt, MAX_PATH * sizeof(TCHAR));
				UInt64 checksize	= 0;
				UInt64 filesize		= 0;
				GetPrivateProfileString(FileData.cFileName, _T("size"), _T(""), tempInt, MAX_PATH, inputfile.c_str());
				checksize = _ttoi64(tempInt);
				filesize = ((UInt64)MAXDWORD+1) * FileData.nFileSizeHigh + FileData.nFileSizeLow;
				if( datehigh==FileData.ftLastWriteTime.dwHighDateTime && datelow==FileData.ftLastWriteTime.dwLowDateTime &&
					checksize!=0 && checksize==filesize )
				{
					skipThis = TRUE;
				}else
				{
					for( Index i = C_INDEX(0); i < filterlist.size(); ++i )
					{
						if( filterlist[i].CompareNoCase(FileData.cFileName)==0 )
						{
							skipThis = TRUE;
							break;
						}
					}
				}
			}

			if( !skipThis )
			{
				// add in file list
				FData filedata;
				filedata.crc = 0;
				Reuben::Math::CRC32(FileData.cFileName, filedata.crc);
				filedata.size = ((UInt64)MAXDWORD+1) * FileData.nFileSizeHigh + FileData.nFileSizeLow;
				filedata.filetime = FileData.ftLastWriteTime;
				filedata.name = FileData.cFileName;
				filelist.push_back(filedata);
			}

			// find another
			if (!FindNextFile(hSearch, &FileData)) 
			{
				if (GetLastError() == ERROR_NO_MORE_FILES) 
					fFinished = TRUE; 
				else
				{
					succeed = FALSE;
					TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GenerateFilelist Error (1) [Next file find error]"));
				}
			}

		}
		FindClose(hSearch);

		if( succeed )
		{
			// ini output
			String output;
			String outputFile, outputFile2;
			outputFile.Format(_T(".\\%s"), STRING_OUTPUT_INI_FILE);
			outputFile2.Format(_T(".\\%s"), STRING_CONFIG_FILE_INI_FILE);
			for( Index i = C_INDEX(0); i < filelist.size(); ++i )
			{
				FData &filedata = filelist[i];
				output.Format(_T("%lu"), filedata.size);
				WritePrivateProfileString(filedata.name.c_str(), _T("size"), output.c_str(), outputFile.c_str());
				output.Format(_T("%x"), filedata.crc);
				WritePrivateProfileString(filedata.name.c_str(), _T("crc"), output.c_str(), outputFile.c_str());
				
				output.Format(_T("%u"), filedata.filetime.dwHighDateTime);
				WritePrivateProfileString(filedata.name.c_str(), _T("datehigh"), output.c_str(), outputFile2.c_str());
				output.Format(_T("%u"), filedata.filetime.dwLowDateTime);
				WritePrivateProfileString(filedata.name.c_str(), _T("datelow"), output.c_str(), outputFile2.c_str());
			}

			ExportXML();
		}
	}else
	{
		// initial file not found
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GenerateFilelist Error (2) [File not found]"));
		succeed = FALSE;
	}
	filterlist.clear();
	filelist.clear();

	return succeed;
}

void DisplayUsage()
{
	_tprintf( _T("Generate INI and XML Filelist\n") );
	_tprintf( _T("\n") );
	_tprintf( _T("MakePatchFilelist [targetDirectory]\n") );
	_tprintf( _T("\n") );
	_tprintf( _T("Example:\n") );
	_tprintf( _T("MakePatchFilelist .\n") );
	_tprintf( _T("MakePatchFilelist TargetDir\n") );
	_tprintf( _T("MakePatchFilelist C:\\\n") );
	_tprintf( _T("MakePatchFilelist C:\\TargetDir\n\n") );
}

#define MAX_INI_LENGTH 200000

void ExportXML()
{
	FileDataList filelist;
	filelist.clear();

	// ini input
	String inputfile;
	inputfile.Format(_T(".\\%s"), STRING_OUTPUT_INI_FILE);

	LPTSTR lpszReturnBuffer;
	lpszReturnBuffer = SafeAllocate(TCHAR, MAX_INI_LENGTH);
	TCHAR* pNextSection = NULL;
	GetPrivateProfileSectionNames(lpszReturnBuffer, MAX_INI_LENGTH, inputfile.c_str());
	pNextSection = lpszReturnBuffer;
	TCHAR tempInt[MAX_PATH];
	while (*pNextSection != 0x00)
	{
		pNextSection = pNextSection + _tcslen(pNextSection) + 1;
		if(*pNextSection != 0x00)
		{
			ZeroMemory(tempInt, MAX_PATH * sizeof(TCHAR));
			FData fdata;
			// name
			fdata.name	= pNextSection;
			// size
			GetPrivateProfileString(fdata.name.c_str(), _T("size"), _T(""), tempInt, MAX_PATH, inputfile.c_str());
			fdata.size	= _ttoi64(tempInt);
			// crc
			GetPrivateProfileString(fdata.name.c_str(), _T("crc"), _T(""), tempInt, MAX_PATH, inputfile.c_str());
			_stscanf(tempInt, _T("%x"), &fdata.crc);

			filelist.push_back(fdata);
		}
	}
	SafeDeallocate(lpszReturnBuffer);

	// xml output
	String output;
	char buffer[4096];
	int len = 0;
	File f0;
	try{
		DeleteFile(STRING_OUTPUT_XML_FILE);
		f0.Open(STRING_OUTPUT_XML_FILE, File::MODE_WRITE);
		if( f0.IsOpened() )
		{
			char abc[3];
			abc[0] = (char)0xef; abc[1] = (char)0xbb; abc[2] = (char)0xbf;
			f0.Write((Byte*)abc, 3);
			output.Format(_T("<?xml version=\"1.0\" ?>\r\n<FileList>\r\n"));
			len = WideCharToMultiByte(CP_UTF8, 0, output.c_str(), -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_UTF8, 0, output.c_str(), -1, buffer, len, NULL, NULL);
			f0.Write((Byte*)buffer, len-1);
			for( Index i = C_INDEX(0); i < filelist.size(); ++i )
			{
				FData &filedata = filelist[i];
				output.Format(_T("\t<File>\r\n"));
				output.AppendFormat(_T("\t\t<Name>%s</Name>\r\n"), filedata.name.c_str());
				output.Replace(_T("&"), _T("&amp;"));
				output.AppendFormat(_T("\t\t<Size>%lu</Size>\r\n"), filedata.size);
				output.AppendFormat(_T("\t\t<CRC>%x</CRC>\r\n"), filedata.crc);
				output.AppendFormat(_T("\t</File>\r\n"));
				len = WideCharToMultiByte(CP_UTF8, 0, output.c_str(), -1, NULL, 0, NULL, NULL);
				WideCharToMultiByte(CP_UTF8, 0, output.c_str(), -1, buffer, len, NULL, NULL);
				f0.Write((Byte*)buffer, len-1);
			}
			output.Format(_T("</FileList>\r\n"));
			len = WideCharToMultiByte(CP_UTF8, 0, output.c_str(), -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_UTF8, 0, output.c_str(), -1, buffer, len, NULL, NULL);
			f0.Write((Byte*)buffer, len-1);
			f0.Close();
		}
	}catch(...)
	{
		TRACE_ERRORDTL(GLOBAL_LOGGER, _T("ExportXML Error (1) [XML generation exception found]"));
	}

}