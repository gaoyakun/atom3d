@echo off
echo -- ������atom3d VC2005��Ŀ�����ļ� --
PAUSE
call "%VS80COMNTOOLS%\..\..\VC\bin\vcvars32.bat"
if exist build_atom_vc8 rd /s /q build_atom_vc8
md build_atom_vc8
cd build_atom_vc8
..\tools\cmake\bin\cmake -G "Visual Studio 8 2005" ..
echo -- ������ϣ�atom3d VC2005��Ŀ�ļ��ѱ�����Ŀ¼build_atom_vc8�� --
PAUSE

