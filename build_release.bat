@echo off

REM Setup visual studio build tools
if not defined DevEnvDir (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
)

REM -GR- turns off c++ runtime type info
REM -EHa turns off c++ exception handling
REM -Oi turn on compiler intrinsics
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
set CommonCompilerFlags=-nologo -MTd -fp:fast -FC -GR- -EHa- -Oi -WX -W4 -wd4201 -wd4100 -wd4505 -wd4189
set CommonLinkerFlags=-incremental:no -opt:ref
set PlatformLinkerLibs=user32.lib Gdi32.lib winmm.lib opengl32.lib ..\..\..\dependencies\GLEW\lib\glew32s.lib
set GameLinkerLibs=opengl32.lib ..\..\..\dependencies\GLEW\lib\glew32s.lib

set ReleaseCompilerFlags=-Ox
set ReleaseCompilerDefinitions=-DDEBUG_BUILD=0 -DPLATFORM_WIN32=1 

REM 64-bit Release build
if not exist .\build\release\x64 mkdir .\build\release\x64
pushd .\build\release\x64
cl %ReleaseCompilerDefinitions% %ReleaseCompilerFlags% %CommonCompilerFlags% -Fegame ..\..\..\src\game\fn_game.cpp -LD /link %CommonLinkerFlags% %GameLinkerLibs%
cl %ReleaseCompilerDefinitions% %ReleaseCompilerFlags% %CommonCompilerFlags% -FeWin64Game ..\..\..\src\platform\win32\win32_main.cpp /link %CommonLinkerFlags% %PlatformLinkerLibs%
del *.obj
del *.exp
del *.lib 
popd

