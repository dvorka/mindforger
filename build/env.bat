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
