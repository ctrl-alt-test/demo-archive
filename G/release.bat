@echo off

set letter=G
set title="Level One"
set dir=cat-%letter%-%title%
set zipfile=%dir%.zip

set exedir=exe
set exedebug=%exedir%\intro_deb_v8_x86.exe
set exerelease=%exedir%\intro_rel_v8_x86.exe
set execompressed=%exedir%\intro_compressed.exe

set include_mp3=1
set musicdir="data\sound"
set musicfile="%musicdir%\hand - Level One.mp3"

set use_kkrunchy=0
set kkrunchy=%exedir%\kkrunchy_023a4_asm07.exe


set pauseAtEnd=1
:: IF "%cmdcmdline:~0,3%"=="cmd" ( set pauseAtEnd=1 ) ELSE ( set pauseAtEnd=0 )


::
::  Check and compress binary
::

echo Preparation du binaire

IF NOT EXIST %exedebug% (
   echo Erreur : pas de binaire DEBUG.
   goto fail
)

IF NOT EXIST %exerelease% (
   echo Erreur : pas de binaire RELEASE.
   goto fail
)

:: Compare la date des deux binaires
for /f %%i in ('dir /b /o:d %exedebug% %exerelease%') do set newest=%exedir%\%%i
if %newest% == %exedebug% (
   echo Erreur : le binaire DEBUG est plus recent que le binaire RELEASE.
   goto fail
)

IF %use_kkrunchy% NEQ 0 (
   copy %exerelease% %execompressed%
   %kkrunchy% --refsize 64 --brute %execompressed%
)


::
::  Copy stuff
::

echo Preparation de %dir%/

if exist %dir% rmdir /S /Q %dir%
mkdir %dir%

:: Copie du/des binaires
IF %use_kkrunchy% NEQ 0 (
   copy %execompressed% %dir%\%dir%_safe.exe
   copy %exerelease% %dir%\%dir%.exe
) else (
   copy %exerelease% %dir%\%dir%.exe
)

:: Fichiers en plus pour G
IF %include_mp3% NEQ 0 (
   copy bass.dll %dir%\
   mkdir %dir%\%musicdir%
   copy %musicfile% %dir%\%musicdir%
)

:: Images pour G
copy stbimage.dll %dir%\
copy "data\title-screen.png" %dir%\data
copy "data\title-screen-glow.png" %dir%\data


:: Touche finale
set infofile=%letter%.nfo
set screenshot=%letter%_preview.jpg
set thumbnail=%letter%_small-preview.jpg

if exist %infofile% (
   copy %infofile% %dir%\
) else (
   echo.
   echo  /!\  Attention : .nfo manquant.
)

if exist %screenshot% (
   copy %screenshot% %dir%\
) else (
   echo.
   echo  /!\  Attention : screenshot manquant.
)

if exist %thumbnail% (
   copy %thumbnail% %dir%\
)

::
::  Zip it.
::
:: See:
:: http://superuser.com/a/112094
:: http://stackoverflow.com/a/149996
::

IF EXIST %zipfile% DEL %zipfile%

set script=_zipIt.vbs

echo Set objArgs = WScript.Arguments> %script%
echo Set fso = CreateObject("Scripting.FileSystemObject")>> %script%
echo inputFolder = fso.GetAbsolutePathName(objArgs(0))>> %script%
echo zipFileName = fso.GetAbsolutePathName(objArgs(1))>> %script%
echo WScript.Echo "Input: " ^& inputFolder>> %script%
echo WScript.Echo "Output: " ^& zipFileName>> %script%
echo CreateObject("Scripting.FileSystemObject").CreateTextFile(zipFileName, True).Write "PK" ^& Chr(5) ^& Chr(6) ^& String(18, vbNullChar)>> %script%
echo Set sa = CreateObject("Shell.Application")>> %script%
echo Set source = sa.NameSpace(inputFolder)>> %script%
echo Set zipFile = sa.NameSpace(zipFileName)>> %script%
echo zipFile.CopyHere(source.Items)>> %script%
echo Do Until zipFile.Items.Count = source.Items.Count>> %script%
echo     WScript.Sleep 500>> %script%
echo Loop>> %script%

echo.
CScript %script% %dir% %zipfile%
if %errorlevel% neq 0 goto fail


::
::  The End
::

:end
RMDIR /S /Q %dir%
DEL %script%
echo.
echo Normalement le package %zipfile% est pret. ^<3
echo.
IF %pauseAtEnd$ NEQ 0 PAUSE
exit /b %errorlevel%


:fail
IF %pauseAtEnd$ NEQ 0 PAUSE
echo exit 1
if %errorlevel% neq 0 exit /B %errorlevel%
exit /B 1
