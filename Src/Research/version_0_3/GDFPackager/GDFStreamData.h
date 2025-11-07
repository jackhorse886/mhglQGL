#pragma once
#ifndef _GDFSTREAMDATA_H_
#define _GDFSTREAMDATA_H_

#include "common.h"

class GDFStreamData
{
public:
	UInt32 m_blockIndex;
	UInt32 m_managerBlockOffset;
	UInt32 m_fileLength;
};

#endif
