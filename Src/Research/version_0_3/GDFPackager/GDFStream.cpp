#include "GDFStream.h"
#include "GDFFile.h"

GDFStream::GDFStream(GDFFile* gdffile, UInt32 blockIndex, UInt32 size)
{
	TRACE_ENSURE(gdffile);
    m_gdffile = gdffile;

	m_sd.m_blockIndex			= blockIndex;
	m_sd.m_managerBlockOffset	= 0;
	m_sd.m_fileLength			= size;

	m_BeginOffset	= 0;
	m_FileSize		= size;
	m_EndOffset		= m_BeginOffset + size;
	m_CurPos		= 0;
}

GDFStream::~GDFStream()
{
}

BOOL GDFStream::Read(BYTE* buf, UInt32 readlen, UInt32& outlen, UInt32 maxLength)
{
	if( !m_gdffile->ReadStream(&m_sd, buf, readlen, outlen, maxLength) )
	{
		outlen = 0;
		return FALSE;
	}
	m_CurPos += outlen;
	return TRUE;
}

VOID GDFStream::MoveFirst()
{
	MovePos(GDF_BEGIN, 0);
}

VOID GDFStream::MovePos(GDFPos relativePos, UInt32 pos)
{
	switch( relativePos )
	{
	case GDF_BEGIN:
		m_CurPos = m_BeginOffset + pos;
		m_sd.m_managerBlockOffset = 4 + pos;
		break;
	case GDF_CURRENT:
		m_CurPos += pos;
		if( m_CurPos > m_EndOffset ) m_CurPos = m_EndOffset;
		m_sd.m_managerBlockOffset += pos;
		break;
	case GDF_END:
		m_CurPos = m_EndOffset;
		m_sd.m_managerBlockOffset = 4 + m_FileSize;
		break;
	}
}

VOID GDFStream::MoveLast()
{
	MovePos(GDF_END, 0);
}

BOOL GDFStream::IsEnd()
{
	return (m_CurPos == m_FileSize);
}

BOOL GDFStream::IsBegin()
{
	return (m_CurPos == 0);
}
