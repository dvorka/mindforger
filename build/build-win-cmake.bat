@echo off
rem MindForger thinking notebook

rem Copyright (C) 2016-2023

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

:: build cmark
cd "%MF_BASE%\deps\cmark-gfm"
if exist build rmdir /q/s build
mkdir build
cd build

echo ====================================
echo Cmake: Generating build files
echo ====================================
cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_CONFIGURATION_TYPES=Debug;Release -DCMARK_TESTS=OFF -DCMARK_SHARED=OFF ..
if "%ERRORLEVEL%" neq "0" goto :err

echo ====================================
echo Cmake: Building Release
echo ====================================
cmake --build . --config Release -- /m
if "%ERRORLEVEL%" neq "0" goto :err

echo ====================================
echo Cmake: Building Debug
echo ====================================
cmake --build . --config Debug -- /m
if "%ERRORLEVEL%" neq "0" goto :err
echo ====================================
echo cmark-gfm has been built  successfully 
echo ====================================
goto :end
:err
echo ====================================
echo cmark-gfm build error! Check log above
echo ====================================
:end
endlocal
pause
