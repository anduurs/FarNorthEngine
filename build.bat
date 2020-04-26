@echo off

REM Setup visual studio build tools
if not defined DevEnvDir (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)

REM -GR- turns off c++ runtime type info
REM -EHa turns off c++ exception handling
REM -Oi turn on compiler intrinsics
set CommonCompilerFlags=-nologo -MTd -fp:fast -FC -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4189
set CommonLinkerFlags=-incremental:no -opt:ref
set PlatformLinkerLibs=user32.lib Gdi32.lib opengl32.lib ..\..\..\dependencies\GLEW\lib\glew32s.lib

set DebugCompilerFlags=-Od -Z7
set DebugCompilerDefinitions=-DDEBUG_BUILD=1 -DPLATFORM_WIN32=1 

set ReleaseCompilerFlags=-Ox
set ReleaseCompilerDefinitions=-DDEBUG_BUILD=0 -DPLATFORM_WIN32=1 

set SharedLib=-LD

REM 64-bit Debug build
if not exist .\build\debug\x64 mkdir .\build\debug\x64
pushd .\build\debug\x64
del *.pdb > NUL 2> NUL
REM compile the game specific code as a DLL
cl %DebugCompilerDefinitions% %DebugCompilerFlags% %CommonCompilerFlags% -Fegame ..\..\..\src\game\fn_game.cpp %SharedLib% /link -PDB:game_%RANDOM%.pdb %CommonLinkerFlags%
REM compile the platform specific code as an EXE
cl %DebugCompilerDefinitions% %DebugCompilerFlags% %CommonCompilerFlags% -FeWin64Game ..\..\..\src\platform\win32\win32_main.cpp /link %CommonLinkerFlags% %PlatformLinkerLibs%
popd

REM 64-bit Release build
REM if not exist .\build\release\x64 mkdir .\build\release\x64
REM pushd .\build\release\x64
REM cl %ReleaseCompilerDefinitions% %ReleaseCompilerFlags% %CommonCompilerFlags% -FeGame ..\..\..\src\platform\win32\win32_main.cpp %SharedLib% /link %CommonLinkerFlags%
REM cl %ReleaseCompilerDefinitions% %ReleaseCompilerFlags% %CommonCompilerFlags% -FeWin64Game ..\..\..\src\platform\win32\win32_main.cpp /link %CommonLinkerFlags% %PlatformLinkerLibs%
REM popd

