setlocal
call "%~dp0%\env.bat"


cd "%MF_BASE%"
if exist "lib\test\src\debug" rmdir /q/s "lib\test\src\debug"
if exist "lib\debug" rmdir /q/s "lib\debug"

echo ====================================
echo Generating MindForger unit test build files
echo ====================================
cd "%MF_BASE%\lib\test"
qmake -r mindforger-lib-unit-tests.pro "CONFIG+=debug" "CONFIG+=mfdebug" 
if "%ERRORLEVEL%" neq "0" goto :err

echo ====================================
echo Building MindForger unit tests 
echo ====================================
nmake
if "%ERRORLEVEL%" neq "0" goto :err

echo ====================================
echo MindForger unit tests has been built successfully 
echo ====================================
goto :end
:err
echo ====================================
echo MindForger unit tests build error! Check log above
echo ====================================
:end
endlocal
pause

