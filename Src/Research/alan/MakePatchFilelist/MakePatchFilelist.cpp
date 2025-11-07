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
#define STRING_OUTPUT_XML_FILE _T("Filelist.xml")
#define STRING_OUTPUT_LOG _T("MakePatchFilelist")

struct FData
{
	String name;
	UInt64 size;
	UInt32 crc;
};

typedef StlVector<FData, ManagedAllocator<FData> > FileDataList;
typedef StlVector<String, ManagedAllocator<String> > FilterList;

void InitLogger();
void CloseLogger();
BOOL GenerateFilelist(LPCTSTR targetDir);
void DisplayUsage();

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
				for( Index i = C_INDEX(0); i < filterlist.size(); ++i )
				{
					if( filterlist[i].CompareNoCase(FileData.cFileName)==0 )
					{
						skipThis = TRUE;
						break;
					}
				}
			}

			if( !skipThis )
			{
				// add in file list
				FData filedata;
				filedata.crc = 0;
				Reuben::Math::CRC32::Calculate(FileData.cFileName, filedata.crc);
				filedata.size = ((UInt64)MAXDWORD+1) * FileData.nFileSizeHigh + FileData.nFileSizeLow;
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
			String outputFile;
			outputFile.Format(_T(".\\%s"), STRING_OUTPUT_INI_FILE);
			for( Index i = C_INDEX(0); i < filelist.size(); ++i )
			{
				FData &filedata = filelist[i];
				output.Format(_T("%lu"), filedata.size);
				WritePrivateProfileString(filedata.name.c_str(), _T("size"), output.c_str(), outputFile.c_str());
				output.Format(_T("%x"), filedata.crc);
				WritePrivateProfileString(filedata.name.c_str(), _T("crc"), output.c_str(), outputFile.c_str());
			}
			// xml output
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
				succeed = FALSE;
				TRACE_ERRORDTL(GLOBAL_LOGGER, _T("GenerateFilelist Error (3) [XML generation exception found]"));
			}
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
