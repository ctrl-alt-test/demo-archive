@echo off

set letter=F
set name=%letter%-Felix
set dir=cat-%name%


echo Preparation de %dir%

if exist %dir% rmdir /S /Q %dir%
mkdir %dir%

cd exe

rem Compare la date des deux binaires
set fdebug=intro_deb_v8_x86.exe
set frelease=intro_rel_v8_x86.exe

for /f %%i in ('dir /b /o:d %fdebug% %frelease%') do set newest=%%i
if %newest% == %fdebug% (
  echo Erreur: le binaire debug est plus recent que le binaire release.
  cd ..
  goto end
)

rem Compression du binaire
copy %frelease% intro_compressed.exe
kkrunchy_023a4_asm07.exe --refsize 64 --brute intro_compressed.exe
cd ..

rem Copie des fichiers
copy exe\%frelease% %dir%\%dir%_safe.exe
copy exe\intro_compressed.exe %dir%\%dir%.exe
copy %letter%.nfo %dir%\

if exist screenshot.jpg (
  copy screenshot.jpg %dir%\
) else (
  echo Screenshot manquant.
)

:end
pause
