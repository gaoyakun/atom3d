@echo off
echo -- ������atom VC2019��Ŀ�����ļ� --
rem PAUSE
if not exist build_atom_vc16 md build_atom_vc16
pushd build_atom_vc16
..\tools\cmake\bin\cmake -G "Visual Studio 16 2019" -A win32 ..
echo -- ������ϣ�atom3d VC2019��Ŀ�ļ��ѱ�����Ŀ¼build_atom_vc16�� --
popd
PAUSE

