@echo off
echo -- ������atom VC2010��Ŀ�����ļ� --
rem PAUSE
call "%VS100COMNTOOLS%\..\..\VC\bin\vcvars32.bat"
if not exist build_atom_vc10 md build_atom_vc10
pushd build_atom_vc10
..\tools\cmake\bin\cmake -G "Visual Studio 10" ..
echo -- ������ϣ�atom3d VC2010��Ŀ�ļ��ѱ�����Ŀ¼build_atom_vc10�� --
popd
rem PAUSE

