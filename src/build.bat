@echo off

set CCFLAGS=-Od -nologo -MD -GR- -EHa- -Oi -W4 -wd4100 -wd4189 -wd4505 -wd4201 -wd4477 -wd4311 -wd4302 -FC -Z7 -D_CRT_SECURE_NO_WARNINGS
set CCFLAGS=-D__DEVELOPER=1 -D__MSVC %CCFLAGS%
set CLFLAGS=-incremental:no -opt:ref User32.lib Gdi32.lib Winmm.lib opengl32.lib

if not exist ..\build mkdir ..\build
pushd ..\build

if exist *.pdb del *.pdb

REM font builder
REM cl %CCFLAGS% ..\src\asset_builder.cpp /link %CLFLAGS%

REM assimp
cl ..\src\assimp.cpp assimp-vc143-mt.lib /std:c++17 /Od /nologo /MTd /GR /EHsc /W4 -wd4505 -wd4700 /wd4201 /wd4189 /wd4100 /FC /Z7 /I..\src\third_party\assimp\include /D_CRT_SECURE_NO_WARNINGS /link /LIBPATH:..\src\third_party\assimp\lib\x64 /incremental:no /opt:ref


REM Simple Preprocessor
cl %CCFLAGS% ..\src\simple_preprocessor.cpp /link %CLFLAGS%
pushd ..\src
..\build\simple_preprocessor.exe > generated.h
popd

REM game code
cl %CCFLAGS% ..\src\game.cpp -LD /link -incremental:no -opt:ref -PDB:game_%random%.pdb -EXPORT:game_update -EXPORT:debug_frame_end

REM windows layer
cl %CCFLAGS% ..\src\win32.cpp /link %CLFLAGS% 

popd
