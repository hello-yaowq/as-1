@echo off

set ASDISK=d:
set ASPATH=%ASDISK%\repository\as

%ASDISK%
cd %ASPATH%

if NOT EXIST "%ASPATH%\Console.bat" goto perror

set PATH=C:\Anaconda3;C:\Python27;C:\msys64\usr\bin;C:\msys64\mingw64\bin;%PATH%
set BOARD=posix

if NOT EXIST "%ASPATH%\release\download" mkdir %ASPATH%\release\download

set CZ=%ASPATH%\release\download\ConsoleZ\Console.exe

if EXIST %CZ% goto launchCZ

cd %ASPATH%\release\download
wget https://github.com/cbucher/console/releases/download/1.18.2/ConsoleZ.x64.1.18.2.17272.zip
mkdir ConsoleZ
cd ConsoleZ
unzip ..\ConsoleZ.x64.1.18.2.17272.zip

:launchCZ

cd %ASPATH%
start %CZ% -ws %ASPATH%\ConsoleZ.workspace

exit 0

:perror
echo Please fix the var "ASDISK" and "ASPATH" to the right path!
pause