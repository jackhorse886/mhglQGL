#include "ProgressMonitor.h"

CProgressMonitor::CProgressMonitor()
{
	m_startState = STATE_STOP;
	m_filename = _T("");
	m_fileTotalSize = 0;
	m_fileCurSize = 0;
	m_overallSize = 0;
	m_overallCur = 0;
}

CProgressMonitor::~CProgressMonitor()
{
}

VOID CProgressMonitor::SetStateStop()
{
	m_startState = STATE_STOP;
}

VOID CProgressMonitor::SetStateDownload()
{
	m_startState = STATE_DOWNLOAD;
}

VOID CProgressMonitor::SetStateUpdate()
{
	m_startState = STATE_UPDATE;
}

VOID CProgressMonitor::SetNewDownload(PCSTRING filename, PCSTRING fullFilePath)
{
	SetFilename(filename);
	SetFullFilePath(fullFilePath);
	m_fileTotalSize = 0;
	m_fileCurSize = 0;
}
