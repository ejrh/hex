Hex
===

[![Build Status](https://travis-ci.org/ejrh/hex.svg?branch=master)](https://travis-ci.org/ejrh/hex)

Hex is a turn-based strategy game engine, inspired by [Age of Wonders](https://en.wikipedia.org/?title=Age_of_Wonders).
The name 'Hex' is derived from the fact that the map tiles are hexagonal (remember, naming
things is one of the two hard problems of computer science).

Hex does not include any data files from Age of Wonders.  If you want to play that game (and I
highly recommend it), you should go buy it.

Project Aims
------------

The Hex engine should:

  * Be capable of emulating most of the features of Age of Wonders.
  * Make use of data defined in simple, user-editable text files.
  * Support using Age of Wonders data (e.g. graphics) where possible.
  * Avoid hardcoding game-specific settings where possible.

And less importantly:

  * Be reasonably efficient in the places that matter (mostly, graphics and AI).
  * Use a consistent, reasonably modern (circa C++11) code style.

Documentation
-------------

  - [INSTALL.md](INSTALL.md) contains instructions on how to compile Hex.

  - [LICENCE.txt](LICENCE.txt) contains the licence.

  - Documentation on the implementation can be found in [src/README.md](src/README.md).
