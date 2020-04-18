@echo off

REM Setup visual studio build tools
if not defined DevEnvDir (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)

REM -GR- turns off c++ runtime type info
REM -EHa turns off c++ exception handling
REM -Oi turn on compiler intrinsics
set CommonCompilerFlags=-nologo -fp:fast -FC -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4189 -DPLATFORM_WIN32=1 
set DebugCompilerFlags=-DDEBUG_BUILD=1 -Od -Z7
set ReleaseCompilerFlags=-DDEBUG_BUILD=0 -Ox
set CommonLinkerFlags=-incremental:no -opt:ref user32.lib Gdi32.lib

REM Debug build
if not exist .\bin\debug\x64 mkdir .\bin\debug\x64
pushd .\bin\debug\x64
REM compile the game specific code as a DLL
cl %DebugCompilerFlags% %CommonCompilerFlags% -Fegame -Fmgame.map ..\..\..\src\game\fn_game.cpp /LD /link %CommonLinkerFlags%
REM compile the platform specific code as an EXE
cl %DebugCompilerFlags% %CommonCompilerFlags% -FeWin64Game -FmWin64Game.map ..\..\..\src\platform\win32\win32_main.cpp /link %CommonLinkerFlags%
popd

REM Release build
REM if not exist .\bin\release\x64 mkdir .\bin\release\x64
REM pushd .\bin\release\x64
REM cl %ReleaseCompilerFlags% %CommonCompilerFlags% -FeGame -FmGame.map ..\..\..\src\platform\win32\win32_main.cpp /LD /link %CommonLinkerFlags%
REM cl %ReleaseCompilerFlags% %CommonCompilerFlags% -FeWin64Game -FmWin64Game.map ..\..\..\src\platform\win32\win32_main.cpp /link %CommonLinkerFlags%
REM popd

