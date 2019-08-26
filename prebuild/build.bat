copy ..\..\..\prebuild\rsa_export.c ..\..\..\libtomcrypt\src\pk\rsa /y
copy ..\..\..\prebuild\ResourceDirectory.h ..\..\..\pelib\include\pelib /y
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
echo %cd%
cd ..\..\..\libtommath
copy ..\prebuild\CMakeLists_libtommath.txt CMakeLists.txt /y
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A Win32
cmake --build . --target ALL_BUILD --config RelWithDebInfo
cd ..
copy build\Product\RelWithDebInfo\libtommath.lib tommath.lib /y
cd ..\libtomcrypt
nmake -f makefile.msvc all