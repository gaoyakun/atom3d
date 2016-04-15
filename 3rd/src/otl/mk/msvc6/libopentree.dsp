# Microsoft Developer Studio Project File - Name="libopentree" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libopentree - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libopentree.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libopentree.mak" CFG="libopentree - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libopentree - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "libopentree - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libopentree - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\libopentree\"
# PROP Intermediate_Dir "..\..\out\release\build\libopentree\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_LIB" /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\.\src" /I "..\..\.\src\opentree" /I "..\..\.\include" /I "..\..\src" /I "..\..\include"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_LIB" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\libopentree\libopentree.res" /D "NDEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\.\src" /i "..\..\.\src\opentree" /i "..\..\.\include" /i "..\..\src" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\release\libs\libopentree.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /OPT:NOREF /subsystem:windows  
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "libopentree - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\libopentree\"
# PROP Intermediate_Dir "..\..\out\debug\build\libopentree\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_LIB" /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\.\src" /I "..\..\.\src\opentree" /I "..\..\.\include" /I "..\..\src" /I "..\..\include"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\libopentree\libopentree.res" /D "_DEBUG" /D "CS_DEBUG" /D "_LIB" /D "_WINDOWS" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\.\src" /i "..\..\.\src\opentree" /i "..\..\.\include" /i "..\..\src" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\out\debug\libs\libopentree_d.lib"
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib   /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /subsystem:windows  
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "libopentree - Win32 Release"
# Name "libopentree - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\mesher\cylmesher.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\mesher\fixedevenstemmesher.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\mesher\leafmesher.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\mesher\stemmesher.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\mesher\treemesher.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\otpolylinespline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utils\otpolylinespline3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\clonefactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\stemfactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\tree.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\weber.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\weber\clonefactory.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\collector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\helper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\leaf.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\leaffactory.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\pruner.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\segment.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\segmentfactory.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\stem.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\stemfactory.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\stemhelper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\stemrelations.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\substemfactory.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\tree.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\trunkfactory.h
# End Source File
# Begin Source File

SOURCE=..\..\src\weber\weber.h
# End Source File
# End Group
# End Target
# End Project
