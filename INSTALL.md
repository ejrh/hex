Installing Hex
==============

Hex is developed in these environments:

  * Windows 7, compiled with Visual C++ 2010 in x86 (i.e. 32 bit) mode.
  * Ubuntu (Linux Mint 17.3), compiled with Clang or GCC in x64 mode.

It is also compiled and automatically tested on [Travis CI][travisci]; see `.travis.yml` for
settings.

[travisci]: https://travis-ci.org/ejrh/hex

Windows
-------

On Windows, this is what I do:

  - Open Visual Sudio command prompt and change to "hex" directory
  - Make a subdirectory "build" and change to it
  - cmake ..\ -G "Visual Studio 10 2010"
  - Open hex.sln, and compile
  - Fix the inevitable errors and repeat

The build type can be changed in Visual Studio from Debug (default) to Release.

I use Visual Studio 10 because I find the new features of successive versions to not be
worth the pain of setting up a development environment on Windows.  There is no reason why it
should not compile with other versions of Visual C++.

Linux
-----

On Linux:

  - Open a terminal and change to "hex" directory
  - Make a subdirectory "build" and change to it
  - cmake ../
  - make
  - Fix the (noticeably fewer) errors and repeat

To make things a bit faster with [cotire][cotire github], make a subdirectory called `CMake`,
put `cotire.cmake` in it, and rerun cmake.

The executable and tests should be run from the main project directory.

To set the build type, use one of these commands:

    (cd build; rm -f CMakeCache.txt; cmake -D CMAKE_BUILD_TYPE=Debug ..)
    (cd build; rm -f CMakeCache.txt; cmake -D CMAKE_BUILD_TYPE=Release ..)

[cotire github]: https://github.com/sakra/cotire

Dependencies
------------

These are the packages required on Linux.  A similar set is required for Windows but must be
installed and configured manually.

  * cmake
  * libboost-dev
  * libboost-program-options-dev
  * libboost-system-dev
  * libboost-thread-dev
  * libboost-filesystem-dev
  * libboost-log-dev
  * libboost-regex-dev
  * libsdl2-dev
  * libsdl2-image-dev
  * libsdl2-mixer-dev
  * libsdl2-ttf-dev
  * msttcorefonts

Game data
---------

Hex generally uses some data from the original Age of Wonders game.  This needs to be placed in
the `data/aow_view/` subdirectory.  The required directories from AOW are:

  * images
  * int
  * release
  * songs
  * sounds

On Linux, it is convenient to make symlinks for these.  For instance, starting in the Hex's main
directory:

    cd data/aow_view
    ln -s /media/cdrive/games/Age\ of\ Wonders/images images
    ln -s /media/cdrive/games/Age\ of\ Wonders/int int
    ln -s /media/cdrive/games/Age\ of\ Wonders/release release
    ln -s /media/cdrive/games/Age\ of\ Wonders/songs songs
    ln -s /media/cdrive/games/Age\ of\ Wonders/sounds sounds

Age of Wonders comes from a Windows environment with case-insensitive file names, which means
some of the file names in the game data may have the wrong case to be found by Hex.  You may
find that you have to rename some of the files to all-lowercase names.

On Windows, you can try making directory symlinks using `mklink /D`, or just copy the
directories.  You shouldn't have to rename any of the files.

Running it
----------

Hex and its tests should be run from Hex's main directory.

On Windows:

    build/src/Debug/hexmain

On Linux:

    build/src/hexmain
