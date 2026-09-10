@echo off
set "PATH=D:\mingw64\bin;C:\Program Files\CMake\bin;%PATH%"
"C:\Program Files\CMake\bin\cmake.exe" -S . -B build -G "MinGW Makefiles"
"C:\Program Files\CMake\bin\cmake.exe" --build build
