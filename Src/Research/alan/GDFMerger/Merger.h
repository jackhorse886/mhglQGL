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
	BOOL Merge(String& src1, String& src2, String& dest);
};

#endif