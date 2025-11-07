// curl support disabled due to missing curl library - curl-7.30.0 requires separate build
/*
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
*/
#include <stdio.h>
#include "NetworkGlobal.h"

// Define CURL_READFUNC_ABORT since curl headers are disabled
#define CURL_READFUNC_ABORT 0x00000001

size_t getcontentlengthfunc(const char *ptr, size_t size, size_t nmemb, void *stream) {
	int r;
	long len = 0;

	/* _snscanf() is Win32 specific */
	// r = _snscanf(ptr, size * nmemb, "Content-Length: %ld\n", &len);
	r = sscanf(ptr, "Content-Length: %ld\n", &len);
	if (r)
		*((long *) stream) = len;

	return size * nmemb;
};

size_t wirtefunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
	return fwrite(ptr, size, nmemb, (FILE*)stream);
};

size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream)
{
	FILE *f = (FILE*)stream;
	size_t n;

	if (ferror(f))
		return CURL_READFUNC_ABORT;

	n = fread(ptr, size, nmemb, (FILE*)f) * size;

	return n;
};
