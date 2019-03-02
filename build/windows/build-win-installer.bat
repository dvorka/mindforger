@echo off
rem MindForger thinking notebook

rem Copyright (C) 2016-2019

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

cls
cd ..\..

rem Gather libraries...
rem C:\Qt\5.12.1\msvc2017_64\bin\windeployqt app\release\mindforger.exe  --dir app\release\bin --no-compiler-runtime

rem Build installer
"C:\Program Files (x86)\Inno Setup 5\ISCC.exe" /Qp /DVcRedistPath="c:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.16.27012\vcredist_x64.exe" build\windows\installer\mindforger-setup.iss
