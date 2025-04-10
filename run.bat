@echo off
echo Deleting old executable...
del main.exe 2> nul

echo Compiling...
g++ main.cpp src/*.cpp -o main.exe -Iinclude

if %errorlevel% neq 0 (
    echo Compilation failed!
    pause
    exit /b
)

echo Running program...
main.exe
pause