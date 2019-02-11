setlocal
call "%~dp0%\env.bat"


cd "%MF_BASE%"

echo ====================================
echo Gathering MindForger dependencies
echo ====================================
windeployqt app\release\mindforger.exe  --dir app\release\bin --no-compiler-runtime
if "%ERRORLEVEL%" neq "0" goto :err

echo ====================================
echo Building MindForger installer
echo ====================================
"%MF_ICSS%" /Qp build\windows\installer\mindforger-setup.iss
if "%ERRORLEVEL%" neq "0" goto :err

:end
echo ================================================
echo MindForger installer has been built successfully
echo ===============================================
goto :end
:err
echo ====================================
echo MindForger installer build error! Check log above
echo ====================================
:end
endlocal
pause

