# Microsoft Developer Studio Project File - Name="B" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=B - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "B.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "B.mak" CFG="B - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "B - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "B - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "B - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin/v6/Release"
# PROP Intermediate_Dir "bin/v6/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Gr /Zp1 /W3 /Ot /Oa /Og /Oi /Oy /Gf /D "A32BITS" /D "WINDOWS" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /FAc /FD /Gs /QIfist /c
# SUBTRACT CPP /Os
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib opengl32.lib winmm.lib glu32.lib /nologo /base:"0x600000" /entry:"entrypoint" /subsystem:windows /pdb:none /map /machine:I386 /nodefaultlib /out:"exe\intro_rel_v6.exe" /OPT:NOWIN98
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "B - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin/v6/Debug"
# PROP Intermediate_Dir "bin/v6/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /WX /GX /Zi /Od /D "A32BITS" /D "WINDOWS" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "DEBUG" /FAc /FR /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib opengl32.lib winmm.lib glu32.lib /nologo /subsystem:windows /profile /map:"../exe/introexe_deb.map" /debug /machine:I386 /out:"exe\intro_deb_v6.exe"

!ENDIF 

# Begin Target

# Name "B - Win32 Release"
# Name "B - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;asm;bat"
# Begin Group "sys"

# PROP Default_Filter ""
# Begin Group "_windows"

# PROP Default_Filter ""
# Begin Group "x86"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\sys\_windows\x86\msys_libcOS.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\sys\_windows\main_deb.cpp

!IF  "$(CFG)" == "B - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "B - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\sys\_windows\main_rel.cpp

!IF  "$(CFG)" == "B - Win32 Release"

!ELSEIF  "$(CFG)" == "B - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\sys\_windows\msys_debugOS.cpp

!IF  "$(CFG)" == "B - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "B - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\sys\_windows\msys_fontOS.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sys\_windows\msys_mmallocOS.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sys\_windows\msys_soundOS.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sys\_windows\msys_threadOS.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sys\_windows\msys_timerOS.cpp
# End Source File
# End Group
# Begin Group "_linux"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\sys\_linux\main_deb.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\sys\_linux\main_rel.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\sys\_linux\main_video.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\sys\_linux\msys_fontOS.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\sys\_linux\msys_mallocOS.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\sys\_linux\msys_sound.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\sys\_linux\msys_thread.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\sys\_linux\msys_timer.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "_irix"

# PROP Default_Filter ""
# End Group
# Begin Group "macos"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\src\sys\events.h
# End Source File
# Begin Source File

SOURCE=.\src\sys\glext.h
# End Source File
# Begin Source File

SOURCE=.\src\sys\msys.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sys\msys.h
# End Source File
# Begin Source File

SOURCE=.\src\sys\msys_font.h
# End Source File
# Begin Source File

SOURCE=.\src\sys\msys_glext.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sys\msys_glext.h
# End Source File
# Begin Source File

SOURCE=.\src\sys\msys_libc.h
# End Source File
# Begin Source File

SOURCE=.\src\sys\msys_malloc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sys\msys_malloc.h
# End Source File
# Begin Source File

SOURCE=.\src\sys\msys_random.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sys\msys_random.h
# End Source File
# Begin Source File

SOURCE=.\src\sys\msys_sound.h
# End Source File
# Begin Source File

SOURCE=.\src\sys\msys_thread.h
# End Source File
# Begin Source File

SOURCE=.\src\sys\msys_timer.h
# End Source File
# Begin Source File

SOURCE=.\src\sys\msys_types.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\intro.cpp
# End Source File
# Begin Source File

SOURCE=.\src\intro.h
# End Source File
# End Group
# End Target
# End Project
