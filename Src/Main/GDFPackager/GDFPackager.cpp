#include "GDFPackageMaker.h"

#define CONFIGURATION_INI_SYS_FILE			_T("Config\\patchcfg.ini")

// GDFPatcher.cpp : Defines the entry point for the console application.
//
void InitLogger();
void CloseLogger();
void DisplayUsage();

int _tmain(int argc, _TCHAR* argv[])
{
	if( argc < 3 )
	{
		DisplayUsage();
		return 0;
	}

	InitLogger();
	String command(argv[1]);
	String param1(argv[2]);
	String param2;
	String param3;
	String param4;
	String param5;
	if( argc > 3 )
		param2 = argv[3];
	if( argc > 4 )
		param3 = argv[4];
	if( argc > 5 )
		param4 = argv[5];

	switch( (TCHAR)command.GetAt(0) )
	{
	case 'a':
		{
			if( param1.Compare(_T("-master"))==0 )
			{
				if( !param2.IsEmpty() && !param3.IsEmpty() )
				{
					GDFPackageMaker packageMaker;
					packageMaker.Add(param3.c_str(), NULL);
					packageMaker.ExecuteFileUpdate(param2.c_str(), 0, TRUE);
				}else
				{
					DisplayUsage();
				}
			}else if( param1.GetAt(0)=='-' && param1.Left(8).Compare(_T("-version"))==0 )	// patch step is recorded
			{
				if( !param2.IsEmpty() && !param3.IsEmpty() )
				{
					UInt scriptVersion = _ttoi(param1.Right(param1.GetLength()-8).c_str());
					if( scriptVersion==0 )
					{
						DisplayUsage();
					}else
					{
						GDFPackageMaker packageMaker;
						packageMaker.Add(param3.c_str(), NULL);
						packageMaker.ExecuteFileUpdate(param2.c_str(), NULL, scriptVersion, FALSE);
					}
				}else
				{
					DisplayUsage();
				}
			}else
			{
				if( !param1.IsEmpty() && !param2.IsEmpty() )
				{
					GDFPackageMaker packageMaker;
					packageMaker.Add(param2.c_str(), NULL);
					packageMaker.ExecuteFileUpdate(param1.c_str(), 0, FALSE);
				}else
				{
					DisplayUsage();
				}
			}
		}
		break;
	case 'd':
		{
			if( param1.GetAt(0)=='-' && param1.Left(8).Compare(_T("-version"))==0 )
			{
				UInt scriptVersion = _ttoi(param1.Right(param1.GetLength()-8).c_str());
				if( scriptVersion==0 )
				{
					DisplayUsage();
				}else
				{
					if( !param2.IsEmpty() && !param3.IsEmpty() )
					{
						GDFPackageMaker packageMaker;
						packageMaker.Delete(param3.c_str(), FALSE);
						packageMaker.ExecuteFileUpdate(param2.c_str(), NULL, scriptVersion);
					}else
					{
						DisplayUsage();
					}
				}
			}else
			{
				if( !param1.IsEmpty() && !param2.IsEmpty() )
				{
					GDFPackageMaker packageMaker;
					packageMaker.Delete(param2.c_str(), FALSE);
					packageMaker.ExecuteFileUpdate(param1.c_str(), NULL, 0);
				}else
				{
					DisplayUsage();
				}
			}
		}
		break;
	case 'e':
		{
			if( param1.Compare(_T("-all"))==0 )
			{
				if( !param2.IsEmpty() )
				{
					GDFPackageMaker packageMaker;
					packageMaker.ExtractAll(param2.c_str());
				}else
				{
					DisplayUsage();
				}
			}else
			{
				if( !param1.IsEmpty() && !param2.IsEmpty() )
				{
					GDFPackageMaker packageMaker;
					packageMaker.Extract(param1.c_str(), param2.c_str());
				}else
				{
					DisplayUsage();
				}
			}
		}
		break;
	case 'm':
		{
			if( param1.GetAt(0)=='-' && param1.Left(6).Compare(_T("-patch"))==0 )
			{
				// Patch merge: Patch steps should be merged
				if( !param2.IsEmpty() && !param3.IsEmpty() )
				{
					GDFPackageMaker packageMaker;
					packageMaker.ExecuteMerge(param2.c_str(), param3.c_str(), TRUE);	// patch file merge -> patch steps must be merged
				}else
				{
					DisplayUsage();
				}
			}else
			{
				// Normal merge: Copy/overwrite every files
				if( !param1.IsEmpty() && !param2.IsEmpty() )
				{
					GDFPackageMaker packageMaker;
					packageMaker.ExecuteMerge(param1.c_str(), param2.c_str(), FALSE);	// patch-to-master file merge -> patch steps no use after merge
				}else
				{
					DisplayUsage();
				}
			}
		}
		break;
	case 'r':
		{
			if( !param1.IsEmpty() )
			{
				GDFPackageMaker packageMaker;
				if( !packageMaker.Repatch(param1.c_str()) )
				{
					_tprintf(_T("Repatch failed, path=%s\n"), param1.c_str());
				}
			}else
			{
				DisplayUsage();
			}
		}
		break;
	case 'u':
		{
			if( param1.Compare(_T("-master"))==0 )
			{
				if( param2.Compare(_T("-f"))==0 )
				{
					if( !param3.IsEmpty() && !param4.IsEmpty() )
					{
						GDFPackageMaker packageMaker;
						packageMaker.UpdatePatch(param4.c_str(), TRUE);
						packageMaker.ExecuteFileUpdate(param3.c_str(), 0, TRUE);
						packageMaker.WriteHistoryFile();
					}else
					{
						DisplayUsage();
					}
				}else if( !param2.IsEmpty() && !param3.IsEmpty() )
				{
					GDFPackageMaker packageMaker;
					packageMaker.UpdatePatch(param3.c_str(), FALSE);
					packageMaker.ExecuteFileUpdate(param2.c_str(), NULL, 0, TRUE);
					packageMaker.WriteHistoryFile();
				}else
				{
					DisplayUsage();
				}
			}else if( param1.Compare(_T("-f"))==0 )
			{
				if( param2.GetAt(0)=='-' && param2.Left(8).Compare(_T("-version"))==0 )	// patch step is recorded
				{
					if( !param3.IsEmpty() && !param4.IsEmpty() )
					{
						UInt scriptVersion = _ttoi(param2.Right(param1.GetLength()-8).c_str());
						if( scriptVersion==0 )
						{
							DisplayUsage();
						}else
						{
							GDFPackageMaker packageMaker;
							packageMaker.UpdatePatch(param4.c_str(), TRUE);
							packageMaker.ExecuteFileUpdate(param3.c_str(), NULL, scriptVersion, FALSE);
							packageMaker.WriteHistoryFile();
						}
					}else
					{
						DisplayUsage();
					}
				}else
				{
					if( !param2.IsEmpty() && !param3.IsEmpty() )
					{
						GDFPackageMaker packageMaker;
						packageMaker.UpdatePatch(param3.c_str(), FALSE);
						packageMaker.ExecuteFileUpdate(param2.c_str(), 0, FALSE);
						packageMaker.WriteHistoryFile();
					}else
					{
						DisplayUsage();
					}
				}
			}else if( param1.GetAt(0)=='-' && param1.Left(8).Compare(_T("-version"))==0 )	// patch step is recorded
			{
				if( !param2.IsEmpty() && !param3.IsEmpty() )
				{
					UInt scriptVersion = _ttoi(param1.Right(param1.GetLength()-8).c_str());
					if( scriptVersion==0 )
					{
						DisplayUsage();
					}else
					{
						GDFPackageMaker packageMaker;
						packageMaker.UpdatePatch(param3.c_str(), FALSE);
						packageMaker.ExecuteFileUpdate(param2.c_str(), NULL, scriptVersion, FALSE);
						packageMaker.WriteHistoryFile();
					}
				}else
				{
					DisplayUsage();
				}
			}else
			{
				if( !param1.IsEmpty() && !param2.IsEmpty() )
				{
					GDFPackageMaker packageMaker;
					packageMaker.UpdatePatch(param2.c_str(), FALSE);
					packageMaker.ExecuteFileUpdate(param1.c_str(), NULL, 0, FALSE);
					packageMaker.WriteHistoryFile();
				}else
				{
					DisplayUsage();
				}
			}
		}
		break;
	case 't':
		{
			if( !param1.IsEmpty() )
			{
				GDFPackageMaker packageMaker;
				if( !packageMaker.Test(param1.c_str()) )
				{
					_tprintf(_T("Test failed, path=%s\n"), param1.c_str());
				}
			}else
			{
				DisplayUsage();
			}
		}
		break;
	case 'v':
		{
			if( !param1.IsEmpty() )
			{
				if( param1.GetAt(0)=='-' && param1.Compare(_T("-patchstep"))==0 )
				{
					GDFPackageMaker packageMaker;
					if( !packageMaker.ViewPatchStep(param2.c_str()) )
					{
						_tprintf(_T("File not found, path=%s\n"), param2.c_str());
					}
				}else
				{
					GDFPackageMaker packageMaker;
					if( !packageMaker.View(param1.c_str()) )
					{
						_tprintf(_T("File not found, path=%s\n"), param1.c_str());
					}
				}
			}else
			{
				DisplayUsage();
			}
		}
		break;
	}
	CloseLogger();
	return 0;
}

void InitLogger()
{
	Reuben::Utility::ConfigFile config(CONFIGURATION_INI_SYS_FILE);
	gGlobalLogger = SafeCreateObject2(Logger, _T("QGLPatcher"), CONFIGURATION_INI_SYS_FILE);
	gGlobalLogger->Open(config);

	// set object manager logger
	Reuben::Simulation::SetObjectLogger(gGlobalLogger);
}

void CloseLogger()
{
	SafeDeleteObject(gGlobalLogger);
}

void DisplayUsage()
{
	_tprintf( _T("Patch or Unpatch files to or fro GDF format\n") );
	_tprintf( _T("\n") );
	_tprintf( _T("GDFPATCHER [a | d | e | m | u | v | t] [switches] destination.GDF [source]\n") );
	_tprintf( _T("\n") );
	_tprintf( _T("  source: source file or directory\n") );
	_tprintf( _T("  destination.GDF: patch file in GDF format\n") );
	_tprintf( _T("  a: Add/Replace file\n") );
	_tprintf( _T("     -master: Hash Size fixed to 2^19.\n") );
	_tprintf( _T("              Act as game master file to support MAX 524288 files.\n") );
	_tprintf( _T("     -versionX: X is an integer.  This is for adding record for versioning.\n") );
	_tprintf( _T("  d: Delete file\n") );
	_tprintf( _T("     -versionX: X is an integer.  This is for adding record for versioning.\n") );
	_tprintf( _T("  e: Extract file\n") );
	_tprintf( _T("     -all: Extract all files from patch to current folder.\n") );
	_tprintf( _T("  m: Merge the 2nd file to the 1st file, ignore master versioning record\n") );
	_tprintf( _T("     -patch: Merge patch file steps for new patch\n") );
	_tprintf( _T("  r: Repatch file\n") );
	_tprintf( _T("  u: Update file, HistoryData\\history.dat is generated to record what is previously updated\n") );
	_tprintf( _T("     -master: Hash Size fixed to 2^19.\n") );
	_tprintf( _T("              Act as game master file to support MAX 524288 files.\n") );
	_tprintf( _T("     -f: include file for update check, if filesize the same and filedate is different\n") );
	_tprintf( _T("     -versionX: X is an integer.  This is for adding record for versioning.\n") );
	_tprintf( _T("  v: View file directory list\n") );
	_tprintf( _T("     -patchstep: Patch steps stored for versioning\n") );
	_tprintf( _T("  t: Test file\n") );
	_tprintf( _T("\n") );
	_tprintf( _T("Example:\n") );
	_tprintf( _T("    Add/Replace\t\tGDFPatcher a Data.gdf Data\n") );
	_tprintf( _T("    Add Master\t\tGDFPatcher a -master Data.gdf Data\n") );
	_tprintf( _T("    Extract All\t\tGDFPatcher e -all Data.gdf\n") );
	_tprintf( _T("    Extract\t\tGDFPatcher e Data.gdf TargetFile.dat\n") );
	_tprintf( _T("    Merge\t\tGDFPatcher m Data.gdf patch.gdf\n") );
	_tprintf( _T("    Delete\t\tGDFPatcher d Data.gdf Data/test.dat\n") );
	_tprintf( _T("    Repatch\t\tGDFPatcher r Data.gdf\n") );
	_tprintf( _T("    Test\t\tGDFPatcher t Data.gdf\n") );
	_tprintf( _T("    View\t\tGDFPatcher v Data.gdf\n") );
	_tprintf( _T("    Update\t\tGDFPatcher u Data.gdf Data\n") );
	_tprintf( _T("    Update\t\tGDFPatcher u -master Data.gdf Data\n") );
	_tprintf( _T("    Update\t\tGDFPatcher u -master -f Data.gdf Data\n") );
	_tprintf( _T("  Versioning patch specific:\n") );
	_tprintf( _T("    Add/Replace\t\tGDFPatcher a -version357 Data.gdf Data\n") );
	_tprintf( _T("    Update\t\tGDFPatcher u -version357 Data.gdf Data\n") );
	_tprintf( _T("    Merge\t\tGDFPatcher m -patch patch_parent.gdf patch_trans.gdf\n") );
	_tprintf( _T("    Delete\t\tGDFPatcher d -version357 Data.gdf Data/test.dat\n") );
	_tprintf( _T("    View Steps\t\tGDFPatcher v -patchstep Data.gdf\n") );
}
