@echo off
echo -- ������atom VC2008��Ŀ�����ļ� --
rem PAUSE
call "%VS90COMNTOOLS%\..\..\VC\bin\vcvars32.bat"
if exist build_atom_vc9 rd /s /q build_atom_vc9
md build_atom_vc9
pushd build_atom_vc9
..\tools\cmake\bin\cmake -G "Visual Studio 9 2008" ..
echo -- ������ϣ�atom3d VC2008��Ŀ�ļ��ѱ�����Ŀ¼build_atom_vc9�� --
popd
rem PAUSE

