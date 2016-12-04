Installing Hex
==============

Hex is developed in these environments:

  * Windows 7, compiled with Visual C++ 2010 in x86 (i.e. 32 bit) mode.
  * Ubuntu, compiled with Clang or GCC in x64 mode.

On Windows, this is what I do:

  - Open Visual Sudio command prompt and change to "hex" directory
  - Make a subdirectory "build" and change to it
  - cmake ..\ -G "Visual Studio 10 2010"
  - Open hex.sln, and compile
  - Fix the inevitable errors and repeat

I use Visual Studio 10 because I find the new features of successive versions to not be
worth the pain of setting up a development environment on Windows.  There is no reason why it
should not compile with other versions of Visual C++.

On Linux:

  - Open a terminal and change to "hex" directory
  - Make a subdirectory "build" and change to it
  - cmake ../
  - make
  - Fix the (noticeably fewer) errors and repeat

To make things a bit faster with [cotire][cotire github], make a subdirectory called `CMake`,
put `cotire.cmake` in it, and rerun cmake.

The executable and tests should be run from the main project directory.

Dependencies
------------

These are the packages required on Linux.  A similar set is required for Windows but must be installed and configured manually.

  * cmake
  * libboost-dev
  * libboost-program-options-dev
  * libboost-system-dev
  * libboost-thread-dev
  * libboost-filesystem-dev
  * libboost-log-dev
  * libsdl2-dev
  * libsdl2-image-dev
  * libsdl2-mixer-dev
  * libsdl2-ttf-dev
  * msttcorefonts

[cotire github]: https://github.com/sakra/cotire
