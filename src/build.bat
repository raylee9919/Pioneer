@echo off

set CCFLAGS=-Od -nologo -MD -GR- -EHa- -Oi -W4 -wd4100 -wd4189 -wd4505 -wd4201 -FC -Z7 -D_CRT_SECURE_NO_WARNINGS
set CCFLAGS=-D__INTERNAL=1 -D__MSVC %CCFLAGS%
set CLFLAGS=-incremental:no -opt:ref User32.lib Gdi32.lib Winmm.lib opengl32.lib

if not exist ..\build mkdir ..\build
pushd ..\build

if exist *.pdb del *.pdb

REM asset builder
cl %CCFLAGS% ..\src\asset_builder.cpp /link %CLFLAGS%

REM assimp
cl ..\src\assimp.cpp assimp-vc143-mt.lib /std:c++17 /Od /nologo /MTd /GR /EHsc /W4 /wd4201 /wd4189 /wd4100 /FC /Z7 /I..\src\third_party\assimp\include /D_CRT_SECURE_NO_WARNINGS /link /LIBPATH:..\src\third_party\assimp\lib\x64 /incremental:no /opt:ref

REM game code
cl %CCFLAGS% -DTRANSLATION_UNIT_IDX=0 ..\src\game.cpp -LD /link -incremental:no -opt:ref -PDB:game_%random%.pdb -EXPORT:game_update -EXPORT:debug_frame_end

REM windows layer
cl %CCFLAGS% -DTRANSLATION_UNIT_IDX=1 ..\src\win32.cpp /link %CLFLAGS% 

popd
