@echo off
echo -- ������atom VC2010��Ŀ�����ļ� --
rem PAUSE
if not exist build_atom_vc10_win64 md build_atom_vc10_win64
pushd build_atom_vc10_win64
..\tools\cmake\bin\cmake -DBUILD_WIN64=ON -G "Visual Studio 10 Win64" ..
echo -- ������ϣ�atom3d VC2010��Ŀ�ļ��ѱ�����Ŀ¼build_atom_vc10_win64�� --
popd
rem PAUSE

