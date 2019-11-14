@echo off
echo -- 将创建atom VC2019项目工程文件 --
rem PAUSE
if not exist build_atom_vc16 md build_atom_vc16
pushd build_atom_vc16
..\tools\cmake\bin\cmake -G "Visual Studio 16 2019" -A win32 ..
echo -- 创建完毕，atom3d VC2019项目文件已保存在目录build_atom_vc16中 --
popd
PAUSE

