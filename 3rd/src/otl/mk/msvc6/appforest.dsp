# Microsoft Developer Studio Project File - Name="appforest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=appforest - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "appforest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "appforest.mak" CFG="appforest - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "appforest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "appforest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "appforest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "release"
# PROP BASE Intermediate_Dir "release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\out\release\build\appforest\"
# PROP Intermediate_Dir "..\..\out\release\build\appforest\"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /Gy /GF /MD /Ob2 /Og /Oi /Ot /Oy /D "_MT" /D "_MBCS" /D "NDEBUG" /D "_WINDOWS" /D "HAS_CG" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\.\src" /I "..\..\.\src\opentree" /I "..\..\.\include" /I "..\..\src" /I "..\..\include"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "NDEBUG" /D "_WINDOWS" /D "HAS_CG"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\release\build\appforest\appforest.res" /D "NDEBUG" /D "_WINDOWS" /D "HAS_CG" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\.\src" /i "..\..\.\src\opentree" /i "..\..\.\include" /i "..\..\src" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib advapi32.lib user32.lib gdi32.lib shell32.lib sdl.lib sdlmain.lib sdl_image.lib opengl32.lib glu32.lib cg.lib cgGL.lib  /nologo /version:4.0 /machine:I386 /OPT:NOREF /out:"..\..\forest.exe" /subsystem:windows  
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "appforest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\out\debug\build\appforest\"
# PROP Intermediate_Dir "..\..\out\debug\build\appforest\"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /vmb /vms /W3 /Gm /G5 /D "WIN32" /FD /c
# ADD CPP /nologo /vmb /vms /W3 /Gm /GX /G5 /FD /c /GR /MDd /ZI /Od /D "_MT" /D "_MBCS" /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS" /D "HAS_CG" /D "WIN32" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__"  /I "." /I "..\.." /I "..\..\.\src" /I "..\..\.\src\opentree" /I "..\..\.\include" /I "..\..\src" /I "..\..\include"
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32 /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS" /D "HAS_CG"
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /fo".\..\..\out\debug\build\appforest\appforest.res" /D "_DEBUG" /D "CS_DEBUG" /D "_WINDOWS" /D "HAS_CG" /D "CS_WIN32_CSCONFIG" /D "__CRYSTAL_SPACE__" /i "." /i "..\.." /i "..\..\.\src" /i "..\..\.\src\opentree" /i "..\..\.\include" /i "..\..\src" /i "..\..\include"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo 
LINK32=link.exe
# ADD BASE LINK32 user32.lib gdi32.lib advapi32.lib /nologo /machine:I386
# ADD LINK32 shell32.lib user32.lib gdi32.lib advapi32.lib advapi32.lib user32.lib gdi32.lib shell32.lib sdl.lib sdlmain.lib sdl_image.lib opengl32.lib glu32.lib cg.lib cgGL.lib  /nologo /version:4.0 /machine:I386 /debug /pdbtype:sept /out:"..\..\forest.exe" /subsystem:console  
# Begin Special Build Tool
SOURCE="$(InputPath)"
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "appforest - Win32 Release"
# Name "appforest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\examples\forest\forestgamelet.cpp
# End Source File
# Begin Source File

SOURCE=..\..\examples\forest\main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\examples\forest\treefactory.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\examples\forest\forest.h
# End Source File
# Begin Source File

SOURCE=..\..\examples\forest\forestgamelet.h
# End Source File
# Begin Source File

SOURCE=..\..\examples\forest\generator.h
# End Source File
# Begin Source File

SOURCE=..\..\examples\forest\imposter.h
# End Source File
# Begin Source File

SOURCE=..\..\examples\forest\loader.h
# End Source File
# Begin Source File

SOURCE=..\..\examples\forest\lodlevel.h
# End Source File
# Begin Source File

SOURCE=..\..\examples\forest\treefactory.h
# End Source File
# Begin Source File

SOURCE=..\..\examples\forest\treeobject.h
# End Source File
# Begin Source File

SOURCE=..\..\examples\forest\treeparams.h
# End Source File
# Begin Source File

SOURCE=..\..\examples\forest\xmlparserforest.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mk\msvc6\appforest.rc
# End Source File
# End Group
# End Target
# End Project
