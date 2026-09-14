# Python Dolphin Memory Engine

A python library designed to read and write the emulated memory of [the Dolphin emulator](https://github.com/dolphin-emu/dolphin) during runtime. 

Binary wheels are available on pypi for Python 3.8 to 3.12. Use `python -m pip install dolphin-memory-engine`.


## System requirements
Any x86_64 based system should work. To run on macOS (ARM and x86_64), a custom code signature is required. Instructions can be found on the Dolphin Memory Engine repository [here](https://github.com/aldelaro5/dolphin-memory-engine#macos-code-signing). Additionally, 32-bit x86 based systems are unsupported since Dolphin dropped their support.

You need to have Dolphin running ***and*** _have the emulation started_ for this program to be useful. As such, the system must meet Dolphin's [system requirements](https://github.com/dolphin-emu/dolphin#system-requirements). Additionally, at least 250 MB of free memory is required.


If it doesn't work, verify that you do not have the `nosuid` mount flag on your `/etc/fstab` as it can cause this command to silently fail.

## Usage

To hook into a single, global Dolphin instance:
```python
import dolphhin_memory engine as dme

dme.hook()
if dme.is_hooked():
    value = dme.read_word(0x80000000)
```

### Multiple Dolphin instances

Use the `DolphinProcess` to hook and read/write into several Dolphin instances at the same time, independently of the global Dolphin instance.:
```python
import dolphhin_memory engine as dme

pids = dme.get_process_ids() # all running Dolphin PIDs
process_a = dme.DolphinProcess(pids[0])
process_b = dme.DolphinProcess(pids[1])

if process_a.is_hooked():
    value = process_a.read_word(0x80000000)
```

`get_process_ids()` looks for the OS platforms default Dolphin process names or the name set in the `DME_DOLPHIN_PROCESS_NAME` environment variable, if available. A list of other Dolphin process names can be provided to override.

A `DolphinProcess` can also be given to `MemWatch` so a watch entry tracks a specific instance instead of the global hook:
```python
watch = dme.MemWatch("my_value", 0x80000000, False, process_a)
```

### Connecting by Game ID

`dolphin_memory_engine.game_id` finds and hooks Dolphin instances that are running specific games, which is useful when you have more than one Dolphin instance open or you want to validate that one of your instances is playing a specific game:
```python
from dolphin_memory_engine import game_id

if game_id.hook_by_game_id("GALE01"):
    value = dme.read_world(0x8000000)
```

`get_process_ids_by_game_id()` and `get_game_id()` are also available directly available if you need to get matching PIDs or the game ID of an already-hooked instance.


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
