#include "Configure.h"

#define CONFIGURATION_PATCHERUI_INI_FILE		_T("res\\patcherui.ini")

CConfigure::CConfigure()
{
}

CConfigure::~CConfigure()
{
}

BOOL CConfigure::LoadConfigure()
{
	String familyVar, accessVar;

	familyVar = _T("UI");
	{
		accessVar = _T("count");
		LoadConfigureInt(m_linkCount, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);

		for( Index i = C_INDEX(0); i < m_linkCount; ++i )
		{
			LinkInfo info;

			accessVar.Format(_T("GoLink_Name%d"), i+1);
			LoadConfigureString(info.name, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
			accessVar.Format(_T("GoLink_URL%d"), i+1);
			LoadConfigureString(info.url, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
			accessVar.Format(_T("GoLink_Button1_%d"), i+1);
			LoadConfigureString(info.btnImage1, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
			accessVar.Format(_T("GoLink_Button2_%d"), i+1);
			LoadConfigureString(info.btnImage2, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
			accessVar.Format(_T("GoLink_Button3_%d"), i+1);
			LoadConfigureString(info.btnImage3, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
			accessVar.Format(_T("GoLink_W%d"), i+1);
			LoadConfigureInt(info.w, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
			accessVar.Format(_T("GoLink_H%d"), i+1);
			LoadConfigureInt(info.h, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
			accessVar.Format(_T("GoLink_X%d"), i+1);
			LoadConfigureInt(info.x, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
			accessVar.Format(_T("GoLink_Y%d"), i+1);
			LoadConfigureInt(info.y, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);

			m_linkInfo.push_back(info);
		}
	}
	familyVar = _T("InitPage");
	{
		ReadGraphInfo(m_initBg, familyVar.c_str(), _T("InitPage"));
	}
	familyVar = _T("NewsPage");
	{
		ReadFunctionInfo(m_newsStart, familyVar.c_str(), _T("OK"));
		ReadFunctionInfo(m_newsExit, familyVar.c_str(), _T("Cancel"));
		ReadURLInfo(m_newsExplorer, familyVar.c_str(), _T("Explorer"));
	}
	familyVar = _T("ServerPage");
	{
		ReadFunctionInfo(m_serverPrev, familyVar.c_str(), _T("Back"));
		ReadFunctionInfo(m_serverDefault, familyVar.c_str(), _T("Default"));
		ReadFunctionInfo(m_serverNext, familyVar.c_str(), _T("Next"));
		ReadFunctionInfo(m_serverZoneUp, familyVar.c_str(), _T("ZoneUp"));
		ReadFunctionInfo(m_serverZoneDown, familyVar.c_str(), _T("ZoneDown"));
		ReadFunctionInfo(m_serverServerUp, familyVar.c_str(), _T("ServerUp"));
		ReadFunctionInfo(m_serverServerDown, familyVar.c_str(), _T("ServerDown"));
		ReadGraphInfo(m_serverDefaultContent, familyVar.c_str(), _T("DefaultContent"));

		String varString;
		String tempString;
		for( Index i = C_INDEX(0); i < C_INDEX(4); ++i )
		{
			varString.Format(_T("SmallS%d_Button1"), i+1);
			LoadConfigureString(tempString, familyVar.c_str(), varString.c_str(), _T(""), CONFIGURATION_PATCHERUI_INI_FILE);
			m_serverButtonTable.btnImage1[i].Format(_T("res\\%s"), tempString.c_str());
		}
		for( Index i = C_INDEX(0); i < C_INDEX(4); ++i )
		{
			varString.Format(_T("SmallS%d_Button2"), i+1);
			LoadConfigureString(tempString, familyVar.c_str(), varString.c_str(), _T(""), CONFIGURATION_PATCHERUI_INI_FILE);
			m_serverButtonTable.btnImage2[i].Format(_T("res\\%s"), tempString.c_str());
		}
		for( Index i = C_INDEX(0); i < C_INDEX(4); ++i )
		{
			varString.Format(_T("SmallS%d_Button3"), i+1);
			LoadConfigureString(tempString, familyVar.c_str(), varString.c_str(), _T(""), CONFIGURATION_PATCHERUI_INI_FILE);
			m_serverButtonTable.btnImage3[i].Format(_T("res\\%s"), tempString.c_str());
		}
		for( Index i = C_INDEX(0); i < C_INDEX(4); ++i )
		{
			varString.Format(_T("SmallS%d_Button4"), i+1);
			LoadConfigureString(tempString, familyVar.c_str(), varString.c_str(), _T(""), CONFIGURATION_PATCHERUI_INI_FILE);
			m_serverButtonTable.btnImage4[i].Format(_T("res\\%s"), tempString.c_str());
		}
		LoadConfigureInt(m_serverButtonTable.w, familyVar.c_str(), _T("Small_W"), 0, CONFIGURATION_PATCHERUI_INI_FILE);
		LoadConfigureInt(m_serverButtonTable.h, familyVar.c_str(), _T("Small_H"), 0, CONFIGURATION_PATCHERUI_INI_FILE);
		LoadConfigureString(tempString, familyVar.c_str(), _T("Big_Button1"), _T(""), CONFIGURATION_PATCHERUI_INI_FILE);
		m_zoneButtonTable.btnImage1.Format(_T("res\\%s"), tempString.c_str());
		LoadConfigureString(tempString, familyVar.c_str(), _T("Big_Button2"), _T(""), CONFIGURATION_PATCHERUI_INI_FILE);
		m_zoneButtonTable.btnImage2.Format(_T("res\\%s"), tempString.c_str());
		LoadConfigureString(tempString, familyVar.c_str(), _T("Big_Button3"), _T(""), CONFIGURATION_PATCHERUI_INI_FILE);
		m_zoneButtonTable.btnImage3.Format(_T("res\\%s"), tempString.c_str());
		LoadConfigureString(tempString, familyVar.c_str(), _T("Big_Button4"), _T(""), CONFIGURATION_PATCHERUI_INI_FILE);
		m_zoneButtonTable.btnImage4.Format(_T("res\\%s"), tempString.c_str());
		LoadConfigureInt(m_zoneButtonTable.w, familyVar.c_str(), _T("Big_W"), 0, CONFIGURATION_PATCHERUI_INI_FILE);
		LoadConfigureInt(m_zoneButtonTable.h, familyVar.c_str(), _T("Big_H"), 0, CONFIGURATION_PATCHERUI_INI_FILE);

		m_zoneinfo1.zoneType = ZONETYPE_SERVER;
		m_zoneinfo2.zoneType = ZONETYPE_ZONE;
		m_zoneinfo3.zoneType = ZONETYPE_SERVER;
		for( Index j = C_INDEX(0); j < C_INDEX(3); ++j )
		{
			String accessVar;
			UInt temprow = 0, tempcol = 0;
			accessVar.Format(_T("Zone%d_RowNum"), j+1);
			LoadConfigureInt(temprow, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
			accessVar.Format(_T("Zone%d_ColNum"), j+1);
			LoadConfigureInt(tempcol, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
			if( temprow && tempcol )
			{
				ZoneInfo* ptrzoneinfo = NULL;
				switch( j )
				{
				case 0:
					ptrzoneinfo = &m_zoneinfo1;
					break;
				case 1:
					ptrzoneinfo = &m_zoneinfo2;
					break;
				case 2:
					ptrzoneinfo = &m_zoneinfo3;
					break;
				}
				ZoneInfo& zoneinfo = *ptrzoneinfo;
				String varString;
				UInt temp = 0;
				for( Index i = C_INDEX(0); i < tempcol; ++i )
				{
					varString.Format(_T("Zone%d_X%d"), j+1, i+1);
					LoadConfigureInt(temp, familyVar.c_str(), varString.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
					zoneinfo.x.push_back(temp);
				}
				for( Index i = C_INDEX(0); i < temprow; ++i )
				{
					varString.Format(_T("Zone%d_Y%d"), j+1, i+1);
					LoadConfigureInt(temp, familyVar.c_str(), varString.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
					zoneinfo.y.push_back(temp);
				}
			}
		}
	}
	familyVar = _T("PatchPage");
	{
		ReadURLInfo(m_patchExplorer, familyVar.c_str(), _T("Explorer"));
		ReadURLInfo(m_eventExplorer, familyVar.c_str(), _T("Event"));
		ReadFunctionInfo(m_patchStart, familyVar.c_str(), _T("OK"));
		ReadFunctionInfo(m_patchExit, familyVar.c_str(), _T("Cancel"));
		ReadProgressInfo(m_patchProgress1, familyVar.c_str(), _T("Progress1"));
		ReadProgressInfo(m_patchProgress2, familyVar.c_str(), _T("Progress2"));

		String tempString;
		accessVar = _T("PatchDL_Name");
		LoadConfigureString(m_patchLink.name, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
		accessVar = _T("PatchDL_URL");
		LoadConfigureString(m_patchLink.url, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
		accessVar = _T("PatchDL_Button1");
		LoadConfigureString(tempString, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
		m_patchLink.btnImage1.Format(_T("res\\%s"), tempString.c_str());
		accessVar = _T("PatchDL_Button2");
		LoadConfigureString(tempString, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
		m_patchLink.btnImage2.Format(_T("res\\%s"), tempString.c_str());
		accessVar = _T("PatchDL_Button3");
		LoadConfigureString(tempString, familyVar.c_str(), accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
		m_patchLink.btnImage3.Format(_T("res\\%s"), tempString.c_str());
		accessVar = _T("PatchDL_W");
		LoadConfigureInt(m_patchLink.w, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
		accessVar = _T("PatchDL_H");
		LoadConfigureInt(m_patchLink.h, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
		accessVar = _T("PatchDL_X");
		LoadConfigureInt(m_patchLink.x, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
		accessVar = _T("PatchDL_Y");
		LoadConfigureInt(m_patchLink.y, familyVar.c_str(), accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);

		ReadGraphInfo(m_patchCurVer, familyVar.c_str(), _T("CurVersion"));
		ReadGraphInfo(m_patchTarVer, familyVar.c_str(), _T("TarVersion"));
		ReadGraphInfo(m_patchYear, familyVar.c_str(), _T("CalenderYYYY"));
		ReadGraphInfo(m_patchMonth, familyVar.c_str(), _T("CalenderMM"));
		ReadGraphInfo(m_patchDay, familyVar.c_str(), _T("CalenderDD"));
		ReadGraphInfo(m_patchWeekday, familyVar.c_str(), _T("CalenderW"));
		ReadGraphInfo(m_progressText1, familyVar.c_str(), _T("ProgressText1"));
		ReadGraphInfo(m_progressText2, familyVar.c_str(), _T("ProgressText2"));
	}
	return TRUE;
}

VOID CConfigure::PrintDebug()
{
	TRACE_VERBOSEDTL_1(GLOBAL_LOGGER, _T("CConfigure::LoadConfigure() Verbose (1) m_linkCount=%d"), m_linkCount);

	for( Index i = C_INDEX(0); i < m_linkInfo.size(); ++i )
	{
		LinkInfo& info = m_linkInfo[i];
		TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigure() Verbose i=%d name=%s"), i, info.name.c_str());
		TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigure() Verbose i=%d url=%s"), i, info.url.c_str());
		TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigure() Verbose i=%d btnImage1=%s"), i, info.btnImage1.c_str());
		TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigure() Verbose i=%d btnImage2=%s"), i, info.btnImage2.c_str());
		TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigure() Verbose i=%d btnImage3=%s"), i, info.btnImage3.c_str());
		TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigure() Verbose i=%d w=%d"), i, info.w);
		TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigure() Verbose i=%d h=%d"), i, info.h);
		TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigure() Verbose i=%d x=%d"), i, info.x);
		TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigure() Verbose i=%d y=%d"), i, info.y);
	}
}

VOID CConfigure::LoadConfigureString(String& out, LPCTSTR family, LPCTSTR variable, LPCTSTR defaultValue, LPCTSTR srcFilePath)
{
	TCHAR pBuf[MAX_PATH];
	GetPrivateProfileString(family, variable, defaultValue, pBuf, MAX_PATH, srcFilePath);
	out = pBuf;
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigureString() Verbose [load] %s=%s"), variable, pBuf);
}

VOID CConfigure::LoadConfigureInt(UInt32& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt32)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigure::LoadConfigureInt(UInt16& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt16)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigure::LoadConfigureInt(UInt8& out, LPCTSTR family, LPCTSTR variable, UInt defaultValue, LPCTSTR srcFilePath)
{
	out = (UInt8)GetPrivateProfileInt(family, variable, defaultValue, srcFilePath);
	TRACE_VERBOSEDTL_2(GLOBAL_LOGGER, _T("CConfigure::LoadConfigureInt() Verbose [load] %s=%d"), variable, out);
}

VOID CConfigure::ReadFunctionInfo(FunctionInfo& info, LPCTSTR family, LPCTSTR groupName)
{
	String accessVar;
	String tempString;
	accessVar.Format(_T("%sName"), groupName);
	LoadConfigureString(info.name, family, accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sButton1"), groupName);
	LoadConfigureString(tempString, family, accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
	info.btnImage1.Format(_T("res\\%s"), tempString.c_str());
	accessVar.Format(_T("%sButton2"), groupName);
	LoadConfigureString(tempString, family, accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
	info.btnImage2.Format(_T("res\\%s"), tempString.c_str());
	accessVar.Format(_T("%sButton3"), groupName);
	LoadConfigureString(tempString, family, accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
	info.btnImage3.Format(_T("res\\%s"), tempString.c_str());
	accessVar.Format(_T("%sButton4"), groupName);
	LoadConfigureString(tempString, family, accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
	info.btnImage4.Format(_T("res\\%s"), tempString.c_str());
	accessVar.Format(_T("%sW"), groupName);
	LoadConfigureInt(info.w, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sH"), groupName);
	LoadConfigureInt(info.h, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sX"), groupName);
	LoadConfigureInt(info.x, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sY"), groupName);
	LoadConfigureInt(info.y, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
}

VOID CConfigure::ReadURLInfo(URLInfo& info, LPCTSTR family, LPCTSTR groupName)
{
	String accessVar;
	accessVar.Format(_T("%sName"), groupName);
	LoadConfigureString(info.name, family, accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sURL"), groupName);
	LoadConfigureString(info.url, family, accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sW"), groupName);
	LoadConfigureInt(info.w, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sH"), groupName);
	LoadConfigureInt(info.h, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sX"), groupName);
	LoadConfigureInt(info.x, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sY"), groupName);
	LoadConfigureInt(info.y, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
}

VOID CConfigure::ReadProgressInfo(ProgressInfo& info, LPCTSTR family, LPCTSTR groupName)
{
	String accessVar;
	accessVar.Format(_T("%sName"), groupName);
	LoadConfigureString(info.name, family, accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sW"), groupName);
	LoadConfigureInt(info.w, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sH"), groupName);
	LoadConfigureInt(info.h, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sX"), groupName);
	LoadConfigureInt(info.x, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sY"), groupName);
	LoadConfigureInt(info.y, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
}

VOID CConfigure::ReadGraphInfo(GraphInfo& info, LPCTSTR family, LPCTSTR groupName)
{
	String accessVar;
	accessVar.Format(_T("%sName"), groupName);
	LoadConfigureString(info.name, family, accessVar.c_str(), _T("\0"), CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sW"), groupName);
	LoadConfigureInt(info.w, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sH"), groupName);
	LoadConfigureInt(info.h, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sX"), groupName);
	LoadConfigureInt(info.x, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
	accessVar.Format(_T("%sY"), groupName);
	LoadConfigureInt(info.y, family, accessVar.c_str(), 0, CONFIGURATION_PATCHERUI_INI_FILE);
}
