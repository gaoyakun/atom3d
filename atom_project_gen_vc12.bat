@echo off
echo -- ������atom VC2013��Ŀ�����ļ� --
rem PAUSE
call "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat"
if not exist build_atom_vc12 md build_atom_vc12
pushd build_atom_vc12
..\tools\cmake\bin\cmake -G "Visual Studio 12 2013" ..
echo -- ������ϣ�atom3d VC2013��Ŀ�ļ��ѱ�����Ŀ¼build_atom_vc12�� --
popd
PAUSE

