@echo off
echo -- 将创建atom VC2010项目工程文件 --
rem PAUSE
call "%VS100COMNTOOLS%\..\..\VC\bin\vcvars32.bat"
if not exist build_atom_vc10 md build_atom_vc10
pushd build_atom_vc10
..\tools\cmake\bin\cmake -G "Visual Studio 10" ..
echo -- 创建完毕，atom3d VC2010项目文件已保存在目录build_atom_vc10中 --
popd
rem PAUSE

