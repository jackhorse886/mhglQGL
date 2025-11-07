#pragma once
#ifndef _HTTPCLIENT_H_
#define _HTTPCLIENT_H_

#include "common.h"
#include <stdio.h>
#include "NetworkGlobal.h"

struct HTTPFile { 
	const char *filename; 
	FILE *stream; 
};

class CHTTPClient
{
public:
	CHTTPClient();
	~CHTTPClient();

public:
	BOOL Download(LPCTSTR cszFilename, LPCTSTR cszUrlPath, LPCTSTR cszUsername, LPCTSTR cszPassword);
};

#endif // _HTTPCLIENT_H_
