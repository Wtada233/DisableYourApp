@echo off
color 02
echo 正在编译：主程序(disable.cpp)
g++ -static -mwindows src/disable.cpp -o out/disable.exe
echo 程序已输出为out/disable.exe
pause