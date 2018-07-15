@echo off
echo             selur-script
echo             =============
echo.
echo   [1] window debug
echo   [2] window server(not work)
echo   [3] window release
echo   [4] void
echo   [5] delete oll ".obj" files
echo   [6] quit
echo.

set asw=0
set /p asw="Please enter number to choose: "

SET folder="win_%DATE%\"

if %asw%==1 goto WIN
if %asw%==2 goto END
if %asw%==3 goto WINR
if %asw%==4 goto END
if %asw%==5 goto OBJDELETING
goto END

:WIN
del %folder% /s /q
xcopy "../bin/win/debug32" %folder% /I
xcopy "../res" %folder% /E /C /Q /I /Y
goto END

:WINR
del %folder% /s /q
xcopy "../bin/win/release32" %folder% /I
xcopy "../res" %folder% /E /C /Q /I /Y
goto END

:OBJDELETING
rmdir /s /q "../obj"
echo "folder is deleted"
goto END

:END

PAUSE