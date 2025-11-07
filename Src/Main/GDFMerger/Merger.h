#pragma once
#ifndef _MERGER_H_
#define _MERGER_H_

#include "GDFPackager\GDFcommon.h"

class Merger
{
public:
	Merger();
	~Merger();

public:
	BOOL Merge(PCSTRING src1, PCSTRING src2, PCSTRING dest);
};

#endif