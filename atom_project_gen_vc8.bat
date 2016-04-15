@echo off
echo -- 将创建atom3d VC2005项目工程文件 --
PAUSE
call "%VS80COMNTOOLS%\..\..\VC\bin\vcvars32.bat"
if exist build_atom_vc8 rd /s /q build_atom_vc8
md build_atom_vc8
cd build_atom_vc8
..\tools\cmake\bin\cmake -G "Visual Studio 8 2005" ..
echo -- 创建完毕，atom3d VC2005项目文件已保存在目录build_atom_vc8中 --
PAUSE

