#pragma once
#ifndef _NETWORKGLOBAL_H_
#define _NETWORKGLOBAL_H_

size_t getcontentlengthfunc(const char *ptr, size_t size, size_t nmemb, void *stream);
size_t wirtefunc(void *ptr, size_t size, size_t nmemb, void *stream);
size_t readfunc(void *ptr, size_t size, size_t nmemb, void *stream);

#endif // _NETWORKGLOBAL_H_
