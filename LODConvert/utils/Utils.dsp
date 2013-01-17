# Microsoft Developer Studio Project File - Name="Utils" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Utils - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Utils.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Utils.mak" CFG="Utils - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Utils - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Utils - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Utils - Win32 NoDebug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "..\.."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Utils - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Falcon4___Win32_Release\Graphics\Utils"
# PROP Intermediate_Dir "..\..\Falcon4___Win32_Release\Graphics\Utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G5 /Zp4 /MT /W3 /GX /Zi /Ox /Ot /Oa /Ow /Og /Oi /Op /Ob2 /I "..\..\Codelib\Include" /I "..\..\\" /I "..\Include" /D "NDEBUG" /D "_LIB" /D TARGET=m_i486 /D "WIN32" /D "_MBCS" /D "STRICT" /D "WIN32_LEAN_AND_MEAN" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Utils - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\Falcon4___Win32_Debug\Graphics\Utils"
# PROP BASE Intermediate_Dir "..\..\Falcon4___Win32_Debug\Graphics\Utils"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Falcon4___Win32_Debug\Graphics\Utils"
# PROP Intermediate_Dir "..\..\Falcon4___Win32_Debug\Graphics\Utils"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\Codelib\Include" /I "..\..\\" /I "..\Include" /D "_DEBUG" /D "_LIB" /D TARGET=m_i486 /D "WIN32" /D "_MBCS" /D "STRICT" /D "WIN32_LEAN_AND_MEAN" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Utils - Win32 NoDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Utils___Win32_NoDebug"
# PROP BASE Intermediate_Dir "Utils___Win32_NoDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Utils___Win32_NoDebug"
# PROP Intermediate_Dir "Utils___Win32_NoDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Zp4 /MT /W3 /Zi /Ox /Ot /Oa /Ow /Og /Oi /Op /Ob2 /I "..\..\Codelib\Include" /I "..\..\\" /D "NDEBUG" /D "_LIB" /D TARGET=m_i486 /D "USE_SH_POOLS" /D "USE_SMART_HEAP" /D "WIN32" /D "_MBCS" /D "STRICT" /D "WIN32_LEAN_AND_MEAN" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G5 /Zp4 /MT /W3 /Ox /Ot /Oa /Ow /Og /Oi /Op /Ob2 /I "..\..\Codelib\Include" /I "..\..\\" /I "..\Include" /D "NDEBUG" /D "_LIB" /D TARGET=m_i486 /D "USE_SH_POOLS" /D "USE_SMART_HEAP" /D "WIN32" /D "_MBCS" /D "STRICT" /D "WIN32_LEAN_AND_MEAN" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Utils - Win32 Release"
# Name "Utils - Win32 Debug"
# Name "Utils - Win32 NoDebug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Loader.cpp
# End Source File
# Begin Source File

SOURCE=.\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Setup.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeMgr.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
