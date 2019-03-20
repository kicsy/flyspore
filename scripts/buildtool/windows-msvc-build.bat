

@echo off

echo "source:" %1

echo "target:" %2

echo "build type:" %3

echo "platform:" %4
set BUILD_BAT="C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" 
if exist %BUILD_BAT% call %BUILD_BAT% %4
set BUILD_BAT="C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" 
if exist %BUILD_BAT% call %BUILD_BAT% %4
set BUILD_BAT="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvarsall.bat" 
if exist %BUILD_BAT% call %BUILD_BAT% %4

cd /d "%2"

echo "current path:" %cd%

cmake %1 -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=%3

cmake --build .
