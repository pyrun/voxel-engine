@echo off
echo             Selur-Skript
echo             =============
echo.
echo   [1] Windows32 Debug
echo   [2] Windows64 Release
echo   [3] Linux64 Debug
echo   [4] Linux64 Release
echo   [5] Alle Obj-dateien löschen
echo   [6] Exit
echo.

set asw=0
set /p asw="Bitte Auswahl eingeben: "

if %asw%==1 goto WIN64D
if %asw%==2 goto WIN64R
if %asw%==3 goto LINUX64D
if %asw%==4 goto LINUX64R
if %asw%==5 goto OBJDELETING
goto END

:WIN64D
del "game" /s /q
xcopy "bin/win/debug32" "game" /I
xcopy "res" "game" /E /C /Q /I /Y
goto END

:WIN64R
del "game" /s /q
xcopy "bin/win/release64" "game" /I
xcopy "res" "game" /E /C /Q /I /Y
goto END

:END