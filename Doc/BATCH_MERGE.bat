set targetfile=_patch.gdf
rem "Used for merging multiple patch directories into ONE patch file"
if "%1"=="" goto end
if "%2"=="" goto end
md PackagingEnv
md PackagingEnv\Batch
md PackagingEnv\Batch\%1_%2
xcopy /Y PackagingEnv\Single\%1\*.* PackagingEnv\Batch\%1_%2\
xcopy /Y PackagingEnv\Single\%2\*.* PackagingEnv\Batch\%1_%2\
xcopy /Y PackagingEnv\Single\%1\%targetfile% PackagingEnv\Batch\%1_%2\
GDFPackagerU m PackagingEnv\Batch\%1_%2\%targetfile% PackagingEnv\Single\%2\%targetfile%
:end