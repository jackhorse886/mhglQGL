#include "Merger.h"
#include "GDFPackager\GDFPackageMaker.h"

Merger::Merger()
{
}

Merger::~Merger()
{
}

BOOL Merger::Merge(String& src1, String& src2, String& dest)
{
	String s1 = src1;
	String s2 = src2;
	String d1 = dest;
	GDFPackageMaker packageMaker;
	if( !packageMaker.ExecuteMerge(src1, src2, TRUE) )
	{
		TRACE_ERRORDTL_3(GLOBAL_LOGGER, _T("Merger::Merge Error (1) [Merge failed] src1=%s, src2=%s, dest=%s"), src1.c_str(), src2.c_str(), src1.c_str());
		return FALSE;
	}

	return TRUE;
}