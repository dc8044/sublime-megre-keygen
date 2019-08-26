# Before build process starts

* Install Visual Studio 2019, cmake, git for windows
* Make `git clone --recursive` to download dependencies


# How to build the project.

* `cd build`
* `cmake .. -G "Visual Studio 16 2019" -A Win32`
* `cmake --build . --target ALL_BUILD --config RelWithDebInfo`