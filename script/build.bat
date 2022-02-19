REM usage build.bat <config>
REM example build.bat Release
REM current config: Release, RelWithDebInfo, MinSizeRel

mkdir ..\%1
cd ..\%1

cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=%1 ..
cmake --build . --config %1 --target INSTALL