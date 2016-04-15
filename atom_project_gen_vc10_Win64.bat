@echo off
echo -- 将创建atom VC2010项目工程文件 --
rem PAUSE
if not exist build_atom_vc10_win64 md build_atom_vc10_win64
pushd build_atom_vc10_win64
..\tools\cmake\bin\cmake -DBUILD_WIN64=ON -G "Visual Studio 10 Win64" ..
echo -- 创建完毕，atom3d VC2010项目文件已保存在目录build_atom_vc10_win64中 --
popd
rem PAUSE

