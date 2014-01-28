# Microsoft Developer Studio Project File - Name="Gdl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Gdl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Gdl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Gdl.mak" CFG="Gdl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Gdl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Gdl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Gdl - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Gdl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /GX /O1 /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Gdl - Win32 Release"
# Name "Gdl - Win32 Debug"
# Begin Group "sound"

# PROP Default_Filter ""
# Begin Group "minifmod"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\Fmusic.c
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\Fsound.c
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\minifmod.h
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\minifmodio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\Mixer.h
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\mixer_clipcopy.c
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\mixer_clipcopy.h
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\mixer_fpu_ramp.c
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\mixer_fpu_ramp.h
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\Music.h
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\music_formatxm.c
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\music_formatxm.h
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\Sound.h
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\system_file.c
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\system_file.h
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\system_memory.h
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\minifmod\xmeffects.h
# End Source File
# End Group
# Begin Group "mymix'r"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Gdl\output\sound\mymixr\myMixr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\sound\mymixr\myMixr.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\Gdl\output\sound\sound.cpp
# End Source File
# End Group
# Begin Group "graphic"

# PROP Default_Filter ""
# Begin Group "map"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\map\map.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\map\map.h
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\map\omap.cpp
# End Source File
# End Group
# Begin Group "gfm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\gfm\Gfm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\gfm\Gfm.h
# End Source File
# End Group
# Begin Group "fbuffer"

# PROP Default_Filter ""
# Begin Group "rlyeh"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\fbuffer\rlyeh\minimal.c
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\fbuffer\rlyeh\minimal.h
# End Source File
# End Group
# Begin Group "tinyPtc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\fbuffer\ptc\convert.c
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\fbuffer\ptc\convert.h
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\fbuffer\ptc\ddraw.c
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\fbuffer\ptc\ddraw.h
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\fbuffer\ptc\tinyptc.c
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\fbuffer\ptc\tinyptc.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\fbuffer\fbuffer.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\anim.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\miscgfx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\output\graphic\text.cpp
# End Source File
# End Group
# Begin Group "input"

# PROP Default_Filter ""
# Begin Group "rar"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Gdl\input\rar\unrarlib.c
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\input\rar\unrarlib.h
# End Source File
# End Group
# Begin Group "xml"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Gdl\input\xml\xml.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\input\xml\xml.h
# End Source File
# End Group
# Begin Group "mod"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Gdl\input\mod\mdpl.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\input\mod\mdpl.h
# End Source File
# End Group
# Begin Group "s3m"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Gdl\input\s3m\s3m.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\input\s3m\s3m.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\Gdl\input\key.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\input\pcx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\input\tga.cpp
# End Source File
# End Group
# Begin Group "misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Gdl\misc\fileio.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\misc\freelist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\misc\log.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\misc\misc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\misc\tablesMath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\misc\tablesMath.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\Gdl\config.h
# End Source File
# Begin Source File

SOURCE=..\..\Gdl\Gdl.h

!IF  "$(CFG)" == "Gdl - Win32 Release"

# PROP Intermediate_Dir "/tmp/release/"

!ELSEIF  "$(CFG)" == "Gdl - Win32 Debug"

# PROP Intermediate_Dir "/tmp/debug/"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Gdl\ini.h
# End Source File
# End Target
# End Project
