A simple pentago player
=======================

A simple pentago player using (essentially) alpha-beta search, a transposition
table, and various precomputed lookup tables for move evaluating, win detection,
etc.  For now it's extremely weak.

For the rules of pentago, see http://en.wikipedia.org/wiki/Pentago.

### Dependencies

The core engine is written in C++ and exposed to Python as an extension module.
The rest of the code (tests, interface, etc.) is in Python.  The dependencies are

* [python](http://python.org)
* [numpy](http://numpy.scipy.org)
* The Otherlab code base

On a Mac, the first two can be obtained through [MacPorts](http://www.macports.org) via

    sudo port install py26-numpy

Unfortunately, while the used portion of the Otherlab code base may be open source at
some point, it's not publically available at this time.

### Usage

To build the engine, run

    make

Currently the Makefile is specific to Mac OS X, but this can easily be fixed.
To run the unit tests, instead [py.test](http://pytest.org) and run

    py.test

To run a fully automated game (computer vs. itself), run

    ./pentago

More to follow.