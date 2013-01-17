# Microsoft Developer Studio Project File - Name="FalcLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=FalcLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FalcLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FalcLib.mak" CFG="FalcLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FalcLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FalcLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FalcLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Falcon4___Win32_Release\Falclib"
# PROP Intermediate_Dir "..\Falcon4___Win32_Release\Falclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G6 /Zp4 /MT /W3 /GX /Zi /O1 /Op /Ob2 /I "..\\" /I "..\codelib\include" /I "..\graphics\include" /I "..\Falclib\Include" /I "..\vu2\include" /I "..\Campaign\Include" /I "..\UI\Include" /I "..\Sim\Include" /D "NDEBUG" /D "_LIB" /D TARGET=m_i486 /D "WIN32" /D "_MBCS" /D "STRICT" /D "WIN32_LEAN_AND_MEAN" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "FalcLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Falcon4___Win32_Debug\Falclib"
# PROP Intermediate_Dir "..\Falcon4___Win32_Debug\Falclib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MTd /W3 /Gm /GX /ZI /Od /I "..\\" /I "..\codelib\include" /I "..\graphics\include" /I "..\Falclib\Include" /I "..\vu2\include" /I "..\Campaign\Include" /I "..\UI\Include" /I "..\Sim\Include" /D "_DEBUG" /D "_LIB" /D TARGET=m_i486 /D "WIN32" /D "_MBCS" /D "STRICT" /D "WIN32_LEAN_AND_MEAN" /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "FalcLib - Win32 Release"
# Name "FalcLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AList.cpp
# End Source File
# Begin Source File

SOURCE=..\utils\bitio.cpp
# End Source File
# Begin Source File

SOURCE=.\classtbl.cpp

!IF  "$(CFG)" == "FalcLib - Win32 Release"

# ADD CPP /Od /Ob1

!ELSEIF  "$(CFG)" == "FalcLib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dispcfg.cpp
# End Source File
# Begin Source File

SOURCE=.\dispopts.cpp
# End Source File
# Begin Source File

SOURCE=..\voicecomunication\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=.\ehandler.cpp
# ADD CPP /I "..\crashhandler"
# End Source File
# Begin Source File

SOURCE=.\Entity.cpp
# End Source File
# Begin Source File

SOURCE=.\F4Comms.cpp
# End Source File
# Begin Source File

SOURCE=.\F4find.cpp
# End Source File
# Begin Source File

SOURCE=.\F4VU.cpp
# End Source File
# Begin Source File

SOURCE=.\falcent.cpp
# End Source File
# Begin Source File

SOURCE=.\FalcGame.cpp
# End Source File
# Begin Source File

SOURCE=.\FalcList.cpp
# End Source File
# Begin Source File

SOURCE=.\falcmem.cpp
# End Source File
# Begin Source File

SOURCE=.\Falcmesg.cpp
# End Source File
# Begin Source File

SOURCE=.\FalcSess.cpp
# End Source File
# Begin Source File

SOURCE=.\FileMap.cpp
# End Source File
# Begin Source File

SOURCE=.\FileMemMap.cpp
# End Source File
# Begin Source File

SOURCE=.\LookupTable.cpp
# End Source File
# Begin Source File

SOURCE=..\utils\lzss.cpp
# End Source File
# Begin Source File

SOURCE=.\mddriver.cpp
# End Source File
# Begin Source File

SOURCE=.\mesg.cpp
# End Source File
# Begin Source File

SOURCE=.\mesgstr.cpp
# End Source File
# Begin Source File

SOURCE=.\messages.cpp
# End Source File
# Begin Source File

SOURCE=.\openfile.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerOp.cpp
# End Source File
# Begin Source File

SOURCE=.\prof_gather.c
# End Source File
# Begin Source File

SOURCE=.\prof_process.c
# End Source File
# Begin Source File

SOURCE=.\prof_win32.c
# End Source File
# Begin Source File

SOURCE=.\rules.cpp
# End Source File
# Begin Source File

SOURCE=.\TheaterDef.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreadMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Threads.cpp
# End Source File
# Begin Source File

SOURCE=.\TimerThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Token.cpp
# End Source File
# Begin Source File

SOURCE=.\Twoddraw.cpp
# End Source File
# Begin Source File

SOURCE=..\voicecomunication\Voice.cpp
# End Source File
# Begin Source File

SOURCE=.\VRInput.cpp
# End Source File
# Begin Source File

SOURCE=.\vuxcreat.cpp
# End Source File
# Begin Source File

SOURCE=.\Weapon.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Include\MsgInc\AddSFXMessage.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\AirAIModeChange.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\AirTaskingMsg.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\AList.h
# End Source File
# Begin Source File

SOURCE=.\include\apitypes.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\ATCCmdMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\ATCMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\AWACSMsg.h
# End Source File
# Begin Source File

SOURCE=.\include\camp2sim.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\CampDataMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\CampDirtyDataMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\CampEventDataMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\CampEventMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\CampMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\CampTaskingMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\CampWeaponFireMsg.h
# End Source File
# Begin Source File

SOURCE=.\include\classtbl.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\ControlSurfaceMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\DamageMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\DeathMessage.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\DivertMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\DLinkMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\ehandler.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\EjectMsg.h
# End Source File
# Begin Source File

SOURCE=.\include\entity.h
# End Source File
# Begin Source File

SOURCE=.\include\f4error.h
# End Source File
# Begin Source File

SOURCE=.\include\f4thread.h
# End Source File
# Begin Source File

SOURCE=.\Include\f4VerNum.h
# End Source File
# Begin Source File

SOURCE=.\Include\f4version.h
# End Source File
# Begin Source File

SOURCE=.\include\f4vu.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\FACMsg.h
# End Source File
# Begin Source File

SOURCE=.\include\fakerand.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\FalcEnt.h
# End Source File
# Begin Source File

SOURCE=.\Include\Falclib.h
# End Source File
# Begin Source File

SOURCE=.\Include\Falcmesg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\FalconFlightPlanMsg.h
# End Source File
# Begin Source File

SOURCE=.\include\falcsess.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\FileMemMap.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\GndTaskingMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\GraphicsTextDisplayMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\LandingMessage.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\LaserDesignateMsg.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\LookupTable.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\MissileEndMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\MissionRequestMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\mlTrig.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\NavalTaskingMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\ObjectiveMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\omni.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\PlayerOp.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\PlayerOpDef.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\PlayerStatusMsg.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\prof_gather.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\prof_internal.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\prof_win32.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\profiler.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\RadioChatterMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\RegenerationMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\RequestAircraftSlot.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\RequestCampaignData.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\RequestDogfightInfo.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\RequestLogbook.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\RequestObject.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\SendAircraftSlot.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\SendCampaignMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\SendChatMessage.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\SendDogfightInfo.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\SendEvalMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\SendImage.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\SendLogbook.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\SendObjData.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\SendPersistantList.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\senduimsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\SendUnitData.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\SendVCMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\SimCampMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\SimDataToggle.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\SimDirtyDataMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\TankerMsg.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\TheaterDef.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\TimingMsg.h
# End Source File
# Begin Source File

SOURCE=.\INCLUDE\Token.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\TrackMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\UnitAssignmentMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\UnitMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\VoiceDataMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\vu2.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\WeaponFireMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\WeatherMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\WindChangeMsg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MsgInc\WingmanMsg.h
# End Source File
# End Group
# End Target
# End Project
