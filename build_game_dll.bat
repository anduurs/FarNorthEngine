@echo off

REM Setup visual studio build tools
if not defined DevEnvDir (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)

REM -WX, -W4: enable warning level 4 and treat warnings as errors
REM -wd: turn off some warnings
REM -MT: static link C runtime library
REM -Oi: generates intrinsic functions.
REM -Od: disable optimization
REM -GR-: disable run-time type information, we don't need this
REM -Gm-: disable minimal rebuild
REM -EHa-: disable exception-handling
REM -nologo: don't print compiler info
REM -FC: full Path of Source Code File in Diagnostics
set CommonCompilerFlags=-nologo -MTd -fp:fast -FC -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4189
set CommonLinkerFlags=-incremental:no -opt:ref

set DebugCompilerFlags=-Od -Z7
set DebugCompilerDefinitions=-DDEBUG_BUILD=1 -DPLATFORM_WIN32=1 

set ReleaseCompilerFlags=-Ox
set ReleaseCompilerDefinitions=-DDEBUG_BUILD=0 -DPLATFORM_WIN32=1 

REM 64-bit Debug build
if not exist .\build\debug\x64 mkdir .\build\debug\x64
pushd .\build\debug\x64
del *.pdb > NUL 2> NUL
REM compile the game specific code as a DLL,  (-PDB:game_%RANDOM%.pdb is needed for hot reload to work)
cl %DebugCompilerDefinitions% %DebugCompilerFlags% %CommonCompilerFlags% -Fegame ..\..\..\src\game\fn_game.cpp -LD /link -PDB:game_%RANDOM%.pdb %CommonLinkerFlags%
popd

REM 64-bit Release build
REM if not exist .\build\release\x64 mkdir .\build\release\x64
REM pushd .\build\release\x64
REM cl %ReleaseCompilerDefinitions% %ReleaseCompilerFlags% %CommonCompilerFlags% -FeGame ..\..\..\src\game\fn_game.cpp -LD /link %CommonLinkerFlags%
REM popd

