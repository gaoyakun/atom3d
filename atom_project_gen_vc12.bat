@echo off
echo -- 将创建atom VC2013项目工程文件 --
rem PAUSE
call "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat"
if not exist build_atom_vc12 md build_atom_vc12
pushd build_atom_vc12
..\tools\cmake\bin\cmake -G "Visual Studio 12 2013" ..
echo -- 创建完毕，atom3d VC2013项目文件已保存在目录build_atom_vc12中 --
popd
PAUSE

