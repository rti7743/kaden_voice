# Microsoft Developer Studio Project File - Name="naichichi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=naichichi - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "naichichi.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "naichichi.mak" CFG="naichichi - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "naichichi - Win32 Release" ("Win32 (x86) Console Application" 用)
!MESSAGE "naichichi - Win32 Debug" ("Win32 (x86) Console Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "naichichi - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "C:\Program Files\Microsoft Speech SDK 5.1\Include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"C:\Program Files\Microsoft Speech SDK 5.1\Lib\i386"

!ELSEIF  "$(CFG)" == "naichichi - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "C:\Program Files\Microsoft Speech SDK 5.1\Include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"C:\Program Files\Microsoft Speech SDK 5.1\Lib\i386"

!ENDIF 

# Begin Target

# Name "naichichi - Win32 Release"
# Name "naichichi - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\comm.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\RAlreadyException.cpp
# End Source File
# Begin Source File

SOURCE=.\RAutoLibraryWithException.cpp
# End Source File
# Begin Source File

SOURCE=.\RComException.cpp
# End Source File
# Begin Source File

SOURCE=.\RConferException.cpp
# End Source File
# Begin Source File

SOURCE=.\RDoNotInitaliseException.cpp
# End Source File
# Begin Source File

SOURCE=.\RException.cpp
# End Source File
# Begin Source File

SOURCE=.\RFileNotFoundException.cpp
# End Source File
# Begin Source File

SOURCE=.\RIOCannotCloseException.cpp
# End Source File
# Begin Source File

SOURCE=.\RIOCannotOpenException.cpp
# End Source File
# Begin Source File

SOURCE=.\RIOCannotReadException.cpp
# End Source File
# Begin Source File

SOURCE=.\RIOCannotWriteException.cpp
# End Source File
# Begin Source File

SOURCE=.\RIOException.cpp
# End Source File
# Begin Source File

SOURCE=.\RNoComplateException.cpp
# End Source File
# Begin Source File

SOURCE=.\RNotFoundException.cpp
# End Source File
# Begin Source File

SOURCE=.\RNotSupportException.cpp
# End Source File
# Begin Source File

SOURCE=.\ROutOfRangeException.cpp
# End Source File
# Begin Source File

SOURCE=.\RParseException.cpp
# End Source File
# Begin Source File

SOURCE=.\RSpeechRecognition.cpp
# End Source File
# Begin Source File

SOURCE=.\RStdioFile.cpp
# End Source File
# Begin Source File

SOURCE=.\RWin32Exception.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\comm.h
# End Source File
# Begin Source File

SOURCE=.\RAlreadyException.h
# End Source File
# Begin Source File

SOURCE=.\RAutoLibraryWithException.h
# End Source File
# Begin Source File

SOURCE=.\RComException.h
# End Source File
# Begin Source File

SOURCE=.\RConferException.h
# End Source File
# Begin Source File

SOURCE=.\RDoNotInitaliseException.h
# End Source File
# Begin Source File

SOURCE=.\RException.h
# End Source File
# Begin Source File

SOURCE=.\RFileNotFoundException.h
# End Source File
# Begin Source File

SOURCE=.\RIOCannotCloseException.h
# End Source File
# Begin Source File

SOURCE=.\RIOCannotOpenException.h
# End Source File
# Begin Source File

SOURCE=.\RIOCannotReadException.h
# End Source File
# Begin Source File

SOURCE=.\RIOCannotWriteException.h
# End Source File
# Begin Source File

SOURCE=.\RIOException.h
# End Source File
# Begin Source File

SOURCE=.\RNoComplateException.h
# End Source File
# Begin Source File

SOURCE=.\RNotFoundException.h
# End Source File
# Begin Source File

SOURCE=.\RNotSupportException.h
# End Source File
# Begin Source File

SOURCE=.\ROutOfRangeException.h
# End Source File
# Begin Source File

SOURCE=.\RParseException.h
# End Source File
# Begin Source File

SOURCE=.\RSpeechRecognition.h
# End Source File
# Begin Source File

SOURCE=.\RStdioFile.h
# End Source File
# Begin Source File

SOURCE=.\RWin32Exception.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
