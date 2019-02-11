setlocal
call "%~dp0%\env.bat"

set "MF_TEST=%MF_BASE%\lib\test\src\debug\mindforger-lib-unit-tests.exe"

if not exist "%MF_TEST%" goto :err
call :normalize %MF_BASE%
set "M8R_GIT_PATH=%RET_V%"
"%MF_TEST%" %*
goto :end
:err
echo ====================================
echo MindForger unit tests cannot be found.
echo ====================================
:end
endlocal
pause

:normalize
  set RET_V=%~dpfn1%
