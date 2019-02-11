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
