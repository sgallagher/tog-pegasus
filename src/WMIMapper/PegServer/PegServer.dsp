# Microsoft Developer Studio Project File - Name="PegServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=PegServer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PegServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PegServer.mak" CFG="PegServer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PegServer - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PegServer - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PegServer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PEGSERVER_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\..\src" /D "NDEBUG" /D "PEGASUS_SERVER_INTERNAL" /D "PEGASUS_PLATFORM_WIN32_IX86_MSVC" /D "PEGASUS_INTERNALONLY" /D _WIN32_WINNT=0x400 /FR"" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 pegwql.lib PegCommon.lib PegRepository.lib pegexportserver.lib pegconfig.lib peguser.lib pegauthentication.lib WmiProvider.lib ws2_32.lib advapi32.lib /nologo /dll /machine:I386 /out:"Release/pegwmiserver.dll" /libpath:"d:\PegasusRun\lib"
# Begin Special Build Tool
OutDir=.\Release
SOURCE="$(InputPath)"
PostBuild_Cmds=copy         $(Outdir)\MyServer.dll         d:\PegasusRun\bin        	copy         $(Outdir)\MyServer.lib        d:\PegasusRun\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "PegServer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PEGSERVER_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\src" /D "_DEBUG" /D "PEGASUS_SERVER_INTERNAL" /D "PEGASUS_PLATFORM_WIN32_IX86_MSVC" /D "PEGASUS_INTERNALONLY" /D _WIN32_WINNT=0x400 /FR"" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Wmiprovider.lib pegwql.lib PegCommon.lib PegRepository.lib pegexportserver.lib pegconfig.lib peguser.lib pegauthentication.lib WmiProvider.lib ws2_32.lib advapi32.lib /nologo /dll /debug /machine:I386 /out:"Debug/pegwmiserver.dll" /pdbtype:sept /libpath:"d:\PegasusRun\lib"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
OutDir=.\Debug
SOURCE="$(InputPath)"
PostBuild_Cmds=copy         $(Outdir)\pegwmiserver.dll         d:\PegasusRun\bin        	copy         $(Outdir)\pegwmiserver.lib        d:\PegasusRun\lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "PegServer - Win32 Release"
# Name "PegServer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CIMOperationRequestAuthorizer.cpp
# End Source File
# Begin Source File

SOURCE=.\CIMOperationRequestDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\CIMOperationRequestDispatcher.cpp
# End Source File
# Begin Source File

SOURCE=.\CIMOperationResponseEncoder.cpp
# End Source File
# Begin Source File

SOURCE=.\CIMServer.cpp
# End Source File
# Begin Source File

SOURCE=.\CIMServerState.cpp
# End Source File
# Begin Source File

SOURCE=.\HTTPAuthenticatorDelegator.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CIMOperationRequestAuthorizer.h
# End Source File
# Begin Source File

SOURCE=.\CIMOperationRequestDecoder.h
# End Source File
# Begin Source File

SOURCE=.\CIMOperationRequestDispatcher.h
# End Source File
# Begin Source File

SOURCE=.\CIMOperationResponseEncoder.h
# End Source File
# Begin Source File

SOURCE=.\CIMServer.h
# End Source File
# Begin Source File

SOURCE=.\CIMServerState.h
# End Source File
# Begin Source File

SOURCE=.\HTTPAuthenticatorDelegator.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
