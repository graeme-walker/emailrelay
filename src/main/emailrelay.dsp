# Microsoft Developer Studio Project File - Name="emailrelay" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=emailrelay - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "emailrelay.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "emailrelay.mak" CFG="emailrelay - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "emailrelay - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "emailrelay - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "emailrelay - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "emailrelay - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "../../src/glib" /I "../../src/gnet" /I "../../lib/msvc6.0" /I "../../src/win32" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "G_WIN32" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "emailrelay - Win32 Release"
# Name "emailrelay - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\gnet\gaddress_ipv4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gadminserver.cpp
# End Source File
# Begin Source File

SOURCE=..\win32\gappinst.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\garg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\garg_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gclient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gclient_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gclientprotocol.cpp
# End Source File
# Begin Source File

SOURCE=..\win32\gcracker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gdaemon_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gdate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gdatetime.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gdatetime_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gdescriptor_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gdirectory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gdirectory_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gevent.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gevent_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\geventserver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gexception.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gfile.cpp
# End Source File
# Begin Source File

SOURCE=..\glib\gfile_win32.cpp
# End Source File
# Begin Source File

SOURCE=.\gfilestore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gfs_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\ggetopt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\glinebuffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\glocal_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\glog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\glogoutput.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\glogoutput_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gmessagestore.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gmessagestore_win32.cpp
# End Source File
# Begin Source File

SOURCE=.\gnewfile.cpp
# End Source File
# Begin Source File

SOURCE=.\gnewmessage.cpp
# End Source File
# Begin Source File

SOURCE=..\glib\gnumber.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gpath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gprocess_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gprotocolmessage.cpp
# End Source File
# Begin Source File

SOURCE=.\gprotocolmessageforward.cpp
# End Source File
# Begin Source File

SOURCE=.\gprotocolmessagestore.cpp
# End Source File
# Begin Source File

SOURCE=..\win32\gpump.cpp
# End Source File
# Begin Source File

SOURCE=..\win32\gpump_nodialog.cpp
# End Source File
# Begin Source File

SOURCE=..\gnet\grequest.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gresolve.cpp
# End Source File
# Begin Source File

SOURCE=..\gnet\gresolve_ipv4.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gresolve_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\gnet\gserver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gserverprotocol.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gsmtpclient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gsmtpserver.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gsocket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gsocket_win32.cpp
# End Source File
# Begin Source File

SOURCE=.\gstoredfile.cpp
# End Source File
# Begin Source File

SOURCE=.\gstoredmessage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gstr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gtime.cpp
# End Source File
# Begin Source File

SOURCE=.\gverifier.cpp
# End Source File
# Begin Source File

SOURCE=..\win32\gwinbase.cpp
# End Source File
# Begin Source File

SOURCE=..\win32\gwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\win32\gwinhid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gwinsock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\main\mailrelay.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\gnet\gaddress.h
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gadminserver.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\garg.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gassert.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gclient.h
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gclientprotocol.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gconvert.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gdaemon.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gdate.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gdatetime.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gdebug.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gdef.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gdescriptor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gdirectory.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gevent.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\geventserver.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gexception.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gfile.h
# End Source File
# Begin Source File

SOURCE=.\gfilestore.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gfs.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\ggetopt.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\glinebuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\glocal.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\glog.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\glogoutput.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gmemory.h
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gmessagestore.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gnet.h
# End Source File
# Begin Source File

SOURCE=.\gnewfile.h
# End Source File
# Begin Source File

SOURCE=.\gnewmessage.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gpath.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gprocess.h
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gprotocolmessage.h
# End Source File
# Begin Source File

SOURCE=.\gprotocolmessageforward.h
# End Source File
# Begin Source File

SOURCE=.\gprotocolmessagestore.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\grequest.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gresolve.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gselect.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gserver.h
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gserverprotocol.h
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gsmtp.h
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gsmtpclient.h
# End Source File
# Begin Source File

SOURCE=..\..\src\main\gsmtpserver.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gsocket.h
# End Source File
# Begin Source File

SOURCE=.\gstoredfile.h
# End Source File
# Begin Source File

SOURCE=.\gstoredmessage.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gstr.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gstrings.h
# End Source File
# Begin Source File

SOURCE=..\..\src\glib\gtime.h
# End Source File
# Begin Source File

SOURCE=.\gverifier.h
# End Source File
# Begin Source File

SOURCE=..\..\src\gnet\gwinsock.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\emailrelay.rc
# End Source File
# Begin Source File

SOURCE=".\icon-32.ico"
# End Source File
# End Group
# End Target
# End Project
