setlocal
call "%~dp0%\env.bat"
echo on
set "MF_APP=%MF_BASE%\app\release\mindforger.exe"
if not exist "%MF_APP%" goto :err
start "" "%MF_APP%"
goto :end
:err
echo cannot find '%MF_APP%'
pause
:end

