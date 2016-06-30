Installing Hex
==============

On Windows, this is what I do:

  - Open Visual Sudio command prompt and change to "hex" directory
  - Make a subdirectory "build" and change to it
  - cmake ..\ -G "Visual Studio 10 2010"
  - Open hex.sln, and compile
  - Fix the inevitable errors and repeat

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
  * libsdl2-ttf-dev
  * libmikmod2-dev
  * msttcorefonts

[cotire github]: https://github.com/sakra/cotire
