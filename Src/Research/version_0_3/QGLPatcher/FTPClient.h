#pragma once
#ifndef _FTPCLIENT_H_
#define _FTPCLIENT_H_

#include "common.h"
#include <stdio.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

struct FtpFile { 
	const char *filename; 
	FILE *stream; 
};

class CFTPClient
{
public:
	CFTPClient();
	~CFTPClient();

public:
	BOOL Download(LPCTSTR cszFilename, LPCTSTR cszUrlPath, LPCTSTR cszUsername, LPCTSTR cszPassword);
};

#endif // _FTPCLIENT_H_
