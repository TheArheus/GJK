@echo off
if not defined DevEnvDir (
    call vcvarsall x64
)

set CompileFlags=-MTd -nologo -fp:fast -EHa -Od -WX- -W4 -Oi -GR- -Gm- -GS -wd4100 -wd4201 -FC -Z7
set LinkFlags=-opt:ref -incremental:no /SUBSYSTEM:console

if not exist ..\build mkdir ..\build
pushd ..\build
cl %CompileFlags% ..\code\GJKmain.cpp /link %LinkFlags%
popd
