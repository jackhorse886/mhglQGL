#pragma once
#ifndef _PROGRESSMONITOR_H_
#define _PROGRESSMONITOR_H_

#include "common.h"

enum STATE
{
	STATE_STOP = 0,
	STATE_DOWNLOAD,
	STATE_UPDATE,
	STATE_MAX_NUM,
};

class CProgressMonitor
{
public:
	CProgressMonitor();
	~CProgressMonitor();

public:
	VOID SetStateStop();
	VOID SetStateDownload();
	VOID SetStateUpdate();

	VOID SetNewDownload(PCSTRING filename, PCSTRING fullFilePath);
	
	CONST String& GetFilename()			{return m_filename;}
	UInt32 GetFileTotalSize()			{return m_fileTotalSize;}
	UInt32 GetFileCurrentSize()			{return m_fileCurSize;}
	CONST String& GetFullFilePath()		{return m_fullFilePath;}
	UInt32 GetOverallTotal()			{return m_overallSize;}
	UInt32 GetOverallCur()				{return m_overallCur + m_fileCurSize;}
	
	VOID SetFileTotalSize(UInt32 filesize)	{m_fileTotalSize = filesize;}
	VOID SetFileCurrentSize(UInt32 curSize)	{m_fileCurSize = curSize;}
	VOID ChangeOverallFinished(UInt32 lastFinishedSize)	{m_overallCur += lastFinishedSize;}
	VOID SetOverallSize(UInt32 overallSize)	{m_overallSize = overallSize;}

private:
	VOID SetFilename(PCSTRING filename)	{m_filename = filename;}
	VOID SetFullFilePath(PCSTRING fullFilePath)	{m_fullFilePath = fullFilePath;}

	STATE m_startState;

	UInt32 m_overallSize;
	UInt32 m_overallCur;

	UInt32 m_fileTotalSize;
	UInt32 m_fileCurSize;

	String m_filename;
	String m_fullFilePath;
};

#endif // _PROGRESSMONITOR_H_
