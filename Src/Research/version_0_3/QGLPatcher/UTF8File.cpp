#include <stdio.h>
#include "UTF8File.h"

UTF8File::UTF8File()
{
	f0 = NULL;
	m_Data = NULL;
	m_DataSize = 0;
	m_CurPos = 0;
	HeaderWritten = false;
	buf = SafeAllocate(char, 4);
	m_readmode = UTF8_DATA_MODE_NULL;
}

UTF8File::~UTF8File()
{
	Close();
	SafeDeallocate(buf);
}

int UTF8File::Read(TCHAR &out)
{
	switch(m_readmode)
	{
	case UTF8_DATA_MODE_FILE_R:
	case UTF8_DATA_MODE_FILE_RW:
		{
			out = '\0';
			memset( buf, 4, sizeof(char) );
			if( fread(&buf[0], 1, sizeof(char), f0) )
				if( (BYTE)buf[0] >> 3 == 0x1E )
					if( fread(&buf[1], 1, sizeof(char), f0) )
						if( (BYTE)buf[1] >> 6 == 0x02 )
							if( fread(&buf[2], 1, sizeof(char), f0) )
								if( (BYTE)buf[2] >> 6 == 0x02 )
									if( fread(&buf[3], 1, sizeof(char), f0) )
										if( (BYTE)buf[3] >> 6 == 0x02 )
										{
										}else
											return 0;
									else
										return 0;
								else
									return 0;
							else
								return 0;
						else
							return 0;
					else
						return 0;
				else if( (BYTE)buf[0] >> 4 == 0x0E )
					if( fread(&buf[1], 1, sizeof(char), f0) )
						if( (BYTE)buf[1] >> 6 == 0x02 )
							if( fread(&buf[2], 1, sizeof(char), f0) )
								if( (BYTE)buf[2] >> 6 == 0x02 )
								{
								}else
									return 0;
							else
								return 0;
						else
							return 0;
					else
						return 0;
				else if( (BYTE)buf[0] >> 5 == 0x06 )
					if( fread(&buf[1], 1, sizeof(char), f0) )
						if( (BYTE)buf[1] >> 6 == 0x02 )
						{
						}else
							return 0;
					else
						return 0;
				else if( (BYTE)buf[0] >> 7 == 0x00 )
				{
				}else
					return 0;
			else
				return 0;
		}
		break;
	case UTF8_DATA_MODE_DATA_R:
		{
			out = '\0';
			memset( buf, 4, sizeof(char) );
			buf[0] = m_Data[m_CurPos++];
			if( (BYTE)buf[0] >> 3 == 0x1E )
			{
				buf[1] = m_Data[m_CurPos++];
				if( (BYTE)buf[1] >> 6 == 0x02 )
				{
					buf[2] = m_Data[m_CurPos++];
					if( (BYTE)buf[2] >> 6 == 0x02 )
					{
						buf[3] = m_Data[m_CurPos++];
						if( (BYTE)buf[3] >> 6 == 0x02 )
						{
						}else
							return 0;
					}
				}else
					return 0;
			}else if( (BYTE)buf[0] >> 4 == 0x0E )
			{
				buf[1] = m_Data[m_CurPos++];
				if( (BYTE)buf[1] >> 6 == 0x02 )
				{
					buf[2] = m_Data[m_CurPos++];
                    if( (BYTE)buf[2] >> 6 == 0x02 )
					{
					}else
						return 0;
				}else
					return 0;
			}else if( (BYTE)buf[0] >> 5 == 0x06 )
			{
				buf[1] = m_Data[m_CurPos++];
				if( (BYTE)buf[1] >> 6 == 0x02 )
				{
				}else
					return 0;
			}else if( (BYTE)buf[0] >> 7 == 0x00 )
			{
			}else
				return 0;
		}
		break;
	}

	TCHAR* t_deststr;
	int len = MultiByteToWideChar(CP_UTF8, 0, buf, -1, NULL, 0);
	t_deststr = SafeAllocate(TCHAR, len);
	MultiByteToWideChar(CP_UTF8, 0, buf, -1, t_deststr, len);
	out = t_deststr[0];
	SafeDeallocate(t_deststr);
	return 1;
}

int UTF8File::Write(TCHAR* src)
{
	if( m_readmode == UTF8_DATA_MODE_FILE_RW )
	{
		if( !HeaderWritten )
		{
			fprintf(f0, "%c%c%c", 0xEF, 0xBB, 0xBF); // UTF-8
			HeaderWritten = true;
		}
		int len = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
		char* buffer = SafeAllocate(char, len);
		WideCharToMultiByte(CP_UTF8, 0, src, -1, buffer, len, NULL, NULL);
		fwrite( buffer, len-1, sizeof(char), f0 );
		SafeDeallocate(buffer);
		return 1;
	}
	return 0;
}

int UTF8File::Open(const char* filename, const char* mode)
{
	if( f0 )
		throw;
	else
	{
		if( strcmp(mode, "rt")==0 )
		{
			if( f0 = fopen( filename, "rb" ) )
			{
				char* temp = SafeAllocate(char, 4);
				temp[3] = '\0';
				if( fread(&temp[0], 1, sizeof(char), f0) && 
					fread(&temp[1], 1, sizeof(char), f0) && 
					fread(&temp[2], 1, sizeof(char), f0) )
				{
					char* abc = SafeAllocate(char, 4);
					abc[0] = (char)0xef;
					abc[1] = (char)0xbb;
					abc[2] = (char)0xbf;
					abc[3] = '\0';
					if( strcmp(temp, abc)==0 )
					{
						m_readmode = UTF8_DATA_MODE_FILE_R;
						SafeDeallocate(abc);
						SafeDeallocate(temp);
						return 1;
					}
					SafeDeallocate(abc);
				}
				SafeDeallocate(temp);
			}
		}else if( strcmp(mode, "wt")==0 )
		{
			if( f0 = fopen( filename, "wb" ) )
			{
				m_readmode = UTF8_DATA_MODE_FILE_R;
				return 1;
			}
		}
	}
	return 0;
}

int UTF8File::Open(const TCHAR* filename, const char* mode)
{
	if( f0 )
		throw;
	else
	{
		if( strcmp(mode, "rt")==0 )
		{
			int len = WideCharToMultiByte(CP_ACP, 0, filename, -1, NULL, 0, NULL, NULL);
			char* ansifilename = SafeAllocate(char, len+1);
			WideCharToMultiByte(CP_ACP, 0, filename, -1, ansifilename, len, NULL, NULL);
			ansifilename[len] = 0;

			if( ansifilename[0]!=0 && (f0 = fopen( ansifilename, "rb" )) )
			{
				char* temp = SafeAllocate(char, 4);
				temp[3] = '\0';
				if( fread(&temp[0], 1, sizeof(char), f0) && 
					fread(&temp[1], 1, sizeof(char), f0) && 
					fread(&temp[2], 1, sizeof(char), f0) )
				{
					char* abc = SafeAllocate(char, 4);
					abc[0] = (char)0xef;
					abc[1] = (char)0xbb;
					abc[2] = (char)0xbf;
					abc[3] = '\0';
					if( strcmp(temp, abc)==0 )
					{
						m_readmode = UTF8_DATA_MODE_FILE_R;
						SafeDeallocate(abc);
						SafeDeallocate(temp);
						SafeDeallocate(ansifilename);
						return 1;
					}
					SafeDeallocate(abc);
				}
				SafeDeallocate(temp);
			}
			SafeDeallocate(ansifilename);
		}else if( strcmp(mode, "wt")==0 )
		{
			int len = WideCharToMultiByte(CP_ACP, 0, filename, -1, NULL, 0, NULL, NULL);
			char* ansifilename = SafeAllocate(char, len+1);
			WideCharToMultiByte(CP_ACP, 0, filename, -1, ansifilename, len, NULL, NULL);
			ansifilename[len] = 0;

			if( f0 = fopen( ansifilename, "wb" ) )
			{
				m_readmode = UTF8_DATA_MODE_FILE_RW;
				SafeDeallocate(ansifilename);
				return 1;
			}
			SafeDeallocate(ansifilename);
		}
	}
	return 0;
}

int UTF8File::Close()
{
	if( f0 )
	{
		fflush(f0);
		int succeed = fclose(f0);
		f0 = NULL;
		m_readmode = UTF8_DATA_MODE_NULL;
		return succeed;
	}else
	{
		return 0;
	}
}

int UTF8File::Rewind()
{
	switch(m_readmode)
	{
	case UTF8_DATA_MODE_FILE_R:
	case UTF8_DATA_MODE_FILE_RW:
		{
			rewind(f0);
			char* temp = SafeAllocate(char, 3);
			if( fread(&temp[0], 1, sizeof(char), f0) && 
				fread(&temp[1], 1, sizeof(char), f0) && 
				fread(&temp[2], 1, sizeof(char), f0) )
			{
				SafeDeallocate(temp);
				return true;
			}
			SafeDeallocate(temp);
			return false;
		}
		break;
	case UTF8_DATA_MODE_DATA_R:
		{
			m_CurPos = 3;
			return 1;
		}
		break;
	}
	return false;
}

int UTF8File::OpenBuf(const BYTE* pData, unsigned int pDataSize)
{
	m_Data = const_cast<BYTE*>(pData);
	m_DataSize = pDataSize;

	if( m_Data[0]==0xef && m_Data[1]==0xbb && m_Data[2]==0xbf )
	{
		m_CurPos = 3;
		m_readmode = UTF8_DATA_MODE_DATA_R;
		return 1;
	}
	return 0;
}
