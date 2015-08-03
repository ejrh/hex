Installing Hex
==============

On Windows, this is what I do:

  - Open Visual Sudio command prompt and change to "hex" directory
  - Make a subdirectory "build" and change to it
  - cmake ..\ -G "Visual Studio 10 2010"
  - Open hex.sln, and compile
  - Fix the inevitable errors and repeat

On Linux:

  - Open a termainl and change to "hex" directory
  - Make a subdirectory "build" and change to it
  - cmake ..\
  - make
  - Fix the (noticeably fewer) errors and repeat

To make things a bit faster with [cotire][cotire github], make a subdirectory called `CMake`,
put `cotire.cmake` in it, and rerun cmake.

[cotire github]: https://github.com/sakra/cotire

