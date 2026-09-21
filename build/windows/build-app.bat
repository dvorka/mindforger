@echo off
rem MindForger thinking notebook

rem Copyright (C) 2016-2026

rem This program is free software; you can redistribute it and/or
rem modify it under the terms of the GNU General Public License
rem as published by the Free Software Foundation; either version 2
rem of the License, or (at your option) any later version.

rem This program is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
rem GNU General Public License for more details.

rem You should have received a copy of the GNU General Public License
rem along with this program. If not, see <http://www.gnu.org/licenses/>.

rem This script copies nice and safe Outlines from personal repository
rem to demo repository used for creation of screenshots and videos.
@echo on

setlocal
call "%~dp0%\env.bat"


cd "%MF_BASE%"
if exist "app\release" rmdir /q/s "app\release"
if exist "lib\release" rmdir /q/s "lib\release"

echo ====================================
echo Generating MindForger build files
echo ====================================
qmake -r mindforger.pro
if "%ERRORLEVEL%" neq "0" goto :err

echo ====================================
echo Building MindForger application
echo ====================================
nmake
if "%ERRORLEVEL%" neq "0" goto :err

echo ====================================
echo Deploying Qt DLLs next to mindforger.exe (so it can be run in place)
echo ====================================
windeployqt app\release\mindforger.exe --no-compiler-runtime
if "%ERRORLEVEL%" neq "0" goto :err

echo ====================================
echo Copying additional runtime DLLs (zlib, OpenSSL) next to mindforger.exe
echo ====================================
copy /y "deps\zlib-win\lib\zlibwapi.dll" "app\release\"
copy /y "deps\openssl-win\ms-vs-2017\libeay32.dll" "app\release\"
copy /y "deps\openssl-win\ms-vs-2017\ssleay32.dll" "app\release\"
copy /y "deps\openssl-win\openssl-1.1.1\capi.dll" "app\release\"
copy /y "deps\openssl-win\openssl-1.1.1\dasync.dll" "app\release\"
copy /y "deps\openssl-win\openssl-1.1.1\libcrypto-1_1-x64.dll" "app\release\"
copy /y "deps\openssl-win\openssl-1.1.1\libssl-1_1-x64.dll" "app\release\"
if "%ERRORLEVEL%" neq "0" goto :err

echo ===================================================
echo MindForger application has been built successfully
echo ==================================================
goto :end
:err
echo ====================================
echo MindForger application build error! Check log above
echo ====================================
:end
endlocal

