@echo off
color 02
mkdir out
echo Compiling disable.cpp.
g++ -static -mwindows src/disable.cpp -o out/disable.exe
echo Output: out/disable.exe
pause
