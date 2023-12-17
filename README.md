# Python Dolphin Memory Engine

A python library designed to read and write the emulated memory of [the Dolphin emulator](https://github.com/dolphin-emu/dolphin) during runtime. 

Binary wheels are available on pypi for Python 3.8 to 3.12. Use `python -m pip install dolphin-memory-engine`.


## System requirements
Any x86_64 based system should work, however, Mac OS is _not_ supported. Additionally, 32-bit x86 based systems are unsupported since Dolphin dropped their support.

You need to have Dolphin running ***and*** _have the emulation started_ for this program to be useful. As such, the system must meet Dolphin's [system requirements](https://github.com/dolphin-emu/dolphin#system-requirements). Additionally, at least 250 MB of free memory is required.


If it doesn't work, verify that you do not have the `nosuid` mount flag on your `/etc/fstab` as it can cause this command to silently fail.

## License
This program is licensed under the MIT license which grants you the permission to do  anything you wish to with the software, as long as you preserve all copyright notices. (See the file LICENSE for the legal text.)


## Development Help

To compile the extension in debug:
```bash
python3 -m venv venv
```
```bash
source venv/bin/activate
```
```bash
python -m pip install Cython pytest
```
```bash
python setup.py build_ext -g --inplace --force
```
```bash
python -m pytest
```
