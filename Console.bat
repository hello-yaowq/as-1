@echo off

set ASPATH=%~dp0
set tmp="%ASPATH%"
set ASDISK=%tmp:~1,2%
set MSYS2="C:\msys64"

%ASDISK%
cd %ASPATH%

if NOT EXIST "%ASPATH%\Console.bat" goto perror
if NOT EXIST %MSYS2%\usr\bin goto install_msys2

set PATH=C:\Anaconda3;C:\Python27;%MSYS2%\mingw64\bin;%MSYS2%\usr\bin;%MSYS2%\mingw32\bin;%PATH%
set BOARD=posix

if NOT EXIST "%ASPATH%\release\download" mkdir %ASPATH%\release\download

set CZ=%ASPATH%\release\download\ConsoleZ\Console.exe

if EXIST %CZ% goto prepareEnv
cd %ASPATH%\release\download
wget https://github.com/cbucher/console/releases/download/1.18.2/ConsoleZ.x64.1.18.2.17272.zip
mkdir ConsoleZ
cd ConsoleZ
unzip ..\ConsoleZ.x64.1.18.2.17272.zip

:prepareEnv
if EXIST "%ASPATH%\scons.bat" goto launchCZ
pacman -Syuu
pacman -S unzip wget git mingw-w64-x86_64-gcc mingw-w64-x86_64-glib2 mingw-w64-x86_64-gtk3
pacman -S ncurses-devel gperf scons
REM TODO: use native scons of MSYS2 is not okay yet now
echo @echo off > scons.bat
echo @echo !!!SCONS on MSYS2!!! >> scons.bat
echo %MSYS2%\usr\bin\python2.exe %MSYS2%\usr\bin\scons %%* >> scons.bat

:launchCZ
cd %ASPATH%
start %CZ% -ws %ASPATH%\ConsoleZ.workspace

exit 0

:install_msys2
set msys2="www.msys2.org"
echo Please visit %msys2% and install msys2 as c:\msys64
pause
exit -1

:perror
echo Please fix the var "ASDISK" and "ASPATH" to the right path!
pause