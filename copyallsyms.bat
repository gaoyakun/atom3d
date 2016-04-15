if exist sym rd /s /q sym

md sym

md sym\Debug
call copysym.bat Debug

md sym\Release
call copysym.bat Release

md sym\MinSizeRel
call copysym.bat MinSizeRel

md sym\RelWithDebInfo 
call copysym.bat RelWithDebInfo
