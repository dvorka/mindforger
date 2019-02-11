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
pause

