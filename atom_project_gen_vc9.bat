@echo off
echo -- 将创建atom VC2008项目工程文件 --
rem PAUSE
call "%VS90COMNTOOLS%\..\..\VC\bin\vcvars32.bat"
if exist build_atom_vc9 rd /s /q build_atom_vc9
md build_atom_vc9
pushd build_atom_vc9
..\tools\cmake\bin\cmake -G "Visual Studio 9 2008" ..
echo -- 创建完毕，atom3d VC2008项目文件已保存在目录build_atom_vc9中 --
popd
rem PAUSE

