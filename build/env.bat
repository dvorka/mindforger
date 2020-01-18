@echo off
rem MindForger thinking notebook

rem Copyright (C) 2016-2020

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

:: auxiliary var
set "MF_BASE=%~dp0%.."
:: call MSVC 2017 Development Environment Setup
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
:: call Qt Development Environment Setup
call "C:\software\Qt\5.12.0\msvc2017_64\bin\qtenv2.bat"
:: set PATH to cmake and zlib 
set "PATH=%PATH%;c:\Program Files\CMake\bin;%MF_BASE%\deps\zlib-win\lib"
:: set path to Inno Setup 5 script compiler
set "MF_ICSS=c:\Program Files (x86)\Inno Setup 5\ISCC.exe"
set "VC_REDIST_PATH=c:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.14.26405\vcredist_x64.exe"
