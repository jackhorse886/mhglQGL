#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include "HTTPClient.h"

CHTTPClient::CHTTPClient()
{
}

CHTTPClient::~CHTTPClient()
{
}

BOOL CHTTPClient::Download(LPCTSTR cszFilename, LPCTSTR cszUrlPath, LPCTSTR cszUsername, LPCTSTR cszPassword)
{
	BOOL bSuccess = FALSE;

	UInt uLenL = WideCharToMultiByte(CP_UTF8, 0, cszFilename, -1, NULL, 0, NULL, NULL);
	char* szbufferL = SafeAllocate(char, uLenL);
	WideCharToMultiByte(CP_UTF8, 0, cszFilename, -1, szbufferL, uLenL, NULL, NULL);

	UInt uLenR = WideCharToMultiByte(CP_UTF8, 0, cszUrlPath, -1, NULL, 0, NULL, NULL);
	char* szbufferR = SafeAllocate(char, uLenR);
	WideCharToMultiByte(CP_UTF8, 0, cszUrlPath, -1, szbufferR, uLenR, NULL, NULL);

	UInt uLenU = WideCharToMultiByte(CP_UTF8, 0, cszUsername, -1, NULL, 0, NULL, NULL);
	char* szbufferU = SafeAllocate(char, uLenU);
	WideCharToMultiByte(CP_UTF8, 0, cszUsername, -1, szbufferU, uLenU, NULL, NULL);

	UInt uLenP = WideCharToMultiByte(CP_UTF8, 0, cszPassword, -1, NULL, 0, NULL, NULL);
	char* szbufferP = SafeAllocate(char, uLenP);
	WideCharToMultiByte(CP_UTF8, 0, cszPassword, -1, szbufferP, uLenP, NULL, NULL);

	char szUserPass[260];
	if( cszUsername && cszPassword )
		sprintf(szUserPass, "%s:%s", szbufferU, szbufferP);

	struct HTTPFile HTTPfile={
		szbufferL,
		NULL
	};

	unsigned int timeout = 10;	// in second
	CURL *curlhandle = NULL;

	curl_global_init(CURL_GLOBAL_ALL);
	curlhandle = curl_easy_init();

	curl_off_t local_file_len = -1 ;
	long filesize = 0 ;

	CURLcode r = CURLE_GOT_NOTHING;
	struct stat file_info;
	int use_resume = 0;
	//if(access(localpath,F_OK) ==0)

	if(stat(szbufferL, &file_info) == 0) 
	{
		local_file_len =  file_info.st_size;
		use_resume = 1;
	}

	FILE *f;
	if (f = fopen(szbufferL, "ab+"))
	{
		curl_easy_setopt(curlhandle, CURLOPT_URL, szbufferR);

		curl_easy_setopt(curlhandle, CURLOPT_CONNECTTIMEOUT, timeout);
		curl_easy_setopt(curlhandle, CURLOPT_HEADERFUNCTION, getcontentlengthfunc);
		curl_easy_setopt(curlhandle, CURLOPT_HEADERDATA, &filesize);
		curl_easy_setopt(curlhandle, CURLOPT_RESUME_FROM_LARGE, use_resume?local_file_len:0);

		curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, f);
		curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, wirtefunc);

		//curl_easy_setopt(curlhandle, CURLOPT_READFUNCTION, readfunc);
		//curl_easy_setopt(curlhandle, CURLOPT_READDATA, f);
		curl_easy_setopt(curlhandle, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1L);

		if( cszUsername && cszPassword )
			curl_easy_setopt(curlhandle, CURLOPT_USERPWD, szUserPass);

		r = curl_easy_perform(curlhandle);

		fclose(f);

		if (r == CURLE_OK)
			bSuccess = TRUE;
		else
		{
			fprintf(stderr, "%s\n", curl_easy_strerror(r));
			bSuccess = FALSE;
		}
		curl_easy_cleanup(curlhandle);
		curl_global_cleanup();	
	}else
	{
		perror(NULL);
	}

	SafeDeallocate(szbufferL);
	SafeDeallocate(szbufferR);
	SafeDeallocate(szbufferU);
	SafeDeallocate(szbufferP);

	return bSuccess;
}
