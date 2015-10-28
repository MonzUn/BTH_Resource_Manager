if exist build/ rd /q /s build
mkdir build
cd build
cmake.exe -G "Visual Studio 14 2015" ..
pause