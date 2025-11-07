if "%1"=="" goto end
md CompletePatch
md CompletePatch\PatchHistory
xcopy /Y GDFPackagerU.exe CompletePatch\
xcopy /Y PackagingEnv\Single\%1\PatchHistory\history.dat CompletePatch\PatchHistory\
xcopy /Y PackagingEnv\Single\%1\_Data.gdf.patch CompletePatch\
cd CompletePatch
GDFPackagerU u _Data.gdf.patch Data\*.ent
GDFPackagerU u _Data.gdf.patch Data\*.map
GDFPackagerU u _Data.gdf.patch Data\*.lua
GDFPackagerU u _Data.gdf.patch Data\*.csv
GDFPackagerU u _Data.gdf.patch Data\*.tga
GDFPackagerU u _Data.gdf.patch Data\*.ttf
GDFPackagerU u _Data.gdf.patch Data\*.ttc
GDFPackagerU u _Data.gdf.patch Data\*.xml
GDFPackagerU u _Data.gdf.patch Data\Buff\*.lua
GDFPackagerU u _Data.gdf.patch Data\ChatIcon\*.ent
GDFPackagerU u _Data.gdf.patch Data\CSV\*.csv
GDFPackagerU u _Data.gdf.patch Data\DAT\*.dat
GDFPackagerU u _Data.gdf.patch Data\DAT\OBJ\*.blk
GDFPackagerU u _Data.gdf.patch Data\Dialogue\*.lua
GDFPackagerU u _Data.gdf.patch Data\EmotionIcon\*.ent
GDFPackagerU u _Data.gdf.patch Data\NPCFACE\*.tga
GDFPackagerU u _Data.gdf.patch Data\Skill\*.lua
cd..
md PackagingEnv
md PackagingEnv\Single
md PackagingEnv\Single\%1
md PackagingEnv\Single\%1\PatchHistory\
xcopy /Y CompletePatch\_Data.gdf.patch PackagingEnv\Single\%1\
xcopy /Y CompletePatch\PatchHistory\*.* PackagingEnv\Single\%1\PatchHistory\
del CompletePatch\_Data.gdf.patch
del CompletePatch\GDFPackagerU.exe
xcopy /Y UpdatePatchU.exe PackagingEnv\Single\%1\
xcopy /Y QGLPatchTemplate.nsi PackagingEnv\Single\%1\
xcopy /Y per-build.bat PackagingEnv\Single\%1\
:end