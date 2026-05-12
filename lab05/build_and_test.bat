@echo off
set CMAKE="C:\Program Files\JetBrains\CLion 2020.3.2\bin\cmake\win\bin\cmake.exe"
set LAB=%~dp0

echo === Configuring ===
%CMAKE% -B "%LAB%build" -G Ninja -S "%LAB%"
if errorlevel 1 goto fail

echo === Building ===
%CMAKE% --build "%LAB%build"
if errorlevel 1 goto fail

echo === Tests ===
"%LAB%build\lab05_tests.exe"

echo.
echo === Main program ===
"%LAB%build\lab05_main.exe"
goto end

:fail
echo BUILD FAILED
:end
pause
