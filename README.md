# Python Dolphin Memory Engine

[![Travis CI Build Status](https://travis-ci.org/henriquegemignani/py-dolphin-memory-engine.svg?branch=master)](https://travis-ci.org/henriquegemignani/py-dolphin-memory-engine)
[![AppVeyor CI Build Status](https://ci.appveyor.com/api/projects/status/i5rb9s0w1l4ahbgj?svg=true)](https://ci.appveyor.com/project/henriquegemignani/py-dolphin-memory-engine)

A python library designed to read and write the emulated memory of [the Dolphin emulator](https://github.com/dolphin-emu/dolphin) during runtime. 

Binary wheels are available on pypi for Python 3.6, 3.7 and 3.8. Use `python -m pip install dolphin-memory-engine` with a modern enough version.


## System requirements
Any x86_64 based system should work, however, Mac OS is _not_ supported. Additionally, 32-bit x86 based systems are unsupported since Dolphin dropped their support.

You need to have Dolphin running ***and*** _have the emulation started_ for this program to be useful. As such, the system must meet Dolphin's [system requirements](https://github.com/dolphin-emu/dolphin#system-requirements). Additionally, at least 250 MB of free memory is required.


## License
This program is licensed under the MIT license which grants you the permission to do  anything you wish to with the software, as long as you preserve all copyright notices. (See the file LICENSE for the legal text.)


## Development Help

To compile the extension in debug:

$ python3 -m venv venv
$ source venv/bin/activate
$ python -m pip install Cython pytest
$ python setup.py build_ext -g --inplace --force
$ python -m pytest