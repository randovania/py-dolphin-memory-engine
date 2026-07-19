from typing import List
from libc.stdint cimport uint8_t, uint32_t, uint64_t
from libcpp cimport bool as c_bool
from libcpp.string cimport string
from libcpp.vector cimport vector


cdef extern from "Common/MemoryCommon.h" namespace "Common::MemType":
    cdef enum MemType:
        type_word


cdef extern from "Common/MemoryCommon.h" namespace "Common::MemBase":
    cdef enum MemBase:
        base_decimal


cdef extern from "Common/MemoryCommon.h" namespace "Common::MemOperationReturnCode":
    cdef enum MemOperationReturnCode:
        invalidInput
        operationFailed
        inputTooLong
        invalidPointer
        OK

cdef extern from "Common/CommonUtils.h" namespace "Common":
    uint32_t dolphinAddrToOffset(uint32_t, c_bool)
    uint32_t offsetToDolphinAddr(uint32_t, c_bool)


cdef extern from "DolphinProcess/DolphinAccessor.h" namespace "DolphinComm::DolphinAccessor":
    cpdef enum class DolphinStatus:
        hooked
        notRunning
        noEmu
        unHooked


cdef extern from "DolphinProcess/DolphinAccessor.h" namespace "DolphinComm":
    cdef cppclass DolphinAccessor:
        @staticmethod
        void init()

        @staticmethod
        void free()

        @staticmethod
        void hook()

        @staticmethod
        void hook(int)

        @staticmethod
        void unHook()

        @staticmethod
        c_bool readFromRAM(uint32_t, char*, const size_t, c_bool)
        
        @staticmethod
        c_bool writeToRAM(uint32_t, const char*, const size_t, c_bool)

        @staticmethod
        int getPID()

        @staticmethod
        vector[int] getProcessIDs(string)

        @staticmethod
        int getProcessIDByGameID(string, string)

        @staticmethod
        DolphinStatus getStatus()

        @staticmethod
        string getLastErrorMessage()
        
        @staticmethod
        c_bool isValidConsoleAddress(uint32_t)

        @staticmethod
        c_bool isARAMAccessible()


cdef extern from "MemoryWatch/MemWatchEntry.h":
    cdef cppclass MemWatchEntry:
        MemWatchEntry()
        MemWatchEntry(string, uint32_t, MemType, MemBase, c_bool, size_t, c_bool)

        char* getMemory()

        void addOffset(int)
        MemOperationReturnCode readMemoryFromRAM()
        MemOperationReturnCode writeMemoryFromString(string)


cdef buffer_to_word(char* buffer):
    cdef uint32_t* value = <uint32_t*> buffer
    return value[0]


cdef buffer_to_float(char* buffer):
    cdef float* value = <float*> buffer
    return value[0]


cdef buffer_to_double(char* buffer):
    cdef double* value = <double*> buffer
    return value[0]


cdef class MemWatch:
    cdef MemWatchEntry c_entry

    def __cinit__(self, label: str, console_address: int, is_pointer: bool):
        self.c_entry = MemWatchEntry(label.encode("utf-8"), console_address, MemType.type_word, MemBase.base_decimal, False, 1, is_pointer)

    def add_offset(self, offset: int):
        self.c_entry.addOffset(offset)

    def get_value(self):
        return buffer_to_word(self.c_entry.getMemory())
        
    def read_memory_from_ram(self):
        return self.c_entry.readMemoryFromRAM() == MemOperationReturnCode.OK

    def write_memory_from_string(self, value: str):
        return self.c_entry.writeMemoryFromString(value.encode("utf-8")) == MemOperationReturnCode.OK


def hook():
    return DolphinAccessor.hook()


def hook(pid=None):
    if pid is None:
        return DolphinAccessor.hook()
    return DolphinAccessor.hook(pid)


def un_hook():
    return DolphinAccessor.unHook()


def is_hooked() -> bool:
    if DolphinAccessor.getStatus() == DolphinStatus.hooked:
        return True
    else:
        return False


def get_process_ids(dolphin_name: str = "") -> list:
    """
    Get all process IDs of running Dolphin instances.
    If dolphin_name is specified, it dynamically filters by that process name.
    """
    return DolphinAccessor.getProcessIDs(dolphin_name.encode("utf-8"))


def get_process_id_by_game_id(game_id: str, dolphin_name: str = "") -> int:
    """
    Find the process ID of the running Dolphin instance playing the game with the given Game ID.
    Supports dynamic Game ID lengths and custom process names.
    Returns None if no matching process is found.
    """
    pid = DolphinAccessor.getProcessIDByGameID(game_id.encode("utf-8"), dolphin_name.encode("utf-8"))
    return pid if pid != -1 else None


def get_game_id(length: int = 6) -> str:
    """
    Get the Game ID of the currently hooked Dolphin instance.
    Supports custom length to support extended IDs from mods.
    """
    assert_hooked()
    # Read the specified number of bytes from the start of MEM1 (0x80000000)
    data = read_bytes(0x80000000, length)
    return data.decode("ascii", errors="replace")


def hook_by_game_id(game_id: str, dolphin_name: str = "") -> bool:
    """
    Automatically search for and hook to the Dolphin process playing the specified Game ID.
    Returns True if successfully hooked, False otherwise.
    """
    pid = get_process_id_by_game_id(game_id, dolphin_name)
    if pid is not None:
        hook(pid)
        return True
    return False


def assert_hooked():
    if not is_hooked():
        raise RuntimeError("not hooked")

def get_status() -> DolphinStatus:
    return DolphinAccessor.getStatus()


def follow_pointers(console_address: int, pointer_offsets: List[int]) -> int:
    assert_hooked()
    real_console_address = console_address

    is_aram_accessible = DolphinAccessor.isARAMAccessible()

    cdef char memory_buffer[4]
    for offset in pointer_offsets:
        if DolphinAccessor.readFromRAM(dolphinAddrToOffset(real_console_address, is_aram_accessible), memory_buffer, 4, True):
            real_console_address = buffer_to_word(memory_buffer)
            if DolphinAccessor.isValidConsoleAddress(real_console_address):
                real_console_address += offset
            else:
                raise RuntimeError(f"Address {real_console_address} is not valid")
        else:
            raise RuntimeError(f"Could not read memory at {real_console_address}: {DolphinAccessor.getLastErrorMessage()}")

    return real_console_address


cdef _read_memory(console_address, char* memory_buffer, int size):
    assert_hooked()
    if not DolphinAccessor.readFromRAM(dolphinAddrToOffset(console_address, DolphinAccessor.isARAMAccessible()), memory_buffer, size, True):
        raise RuntimeError(f"Could not read memory at {console_address}: {DolphinAccessor.getLastErrorMessage()}")


def read_byte(console_address: int) -> int:
    cdef char memory_buffer[1]
    _read_memory(console_address, memory_buffer, 1)
    return (<uint8_t*> memory_buffer)[0]


def read_word(console_address: int) -> int:
    cdef char memory_buffer[4]
    _read_memory(console_address, memory_buffer, 4)
    return (<uint32_t*> memory_buffer)[0]


def read_float(console_address: int) -> float:
    cdef char memory_buffer[4]
    _read_memory(console_address, memory_buffer, 4)
    return (<float*> memory_buffer)[0]


def read_double(console_address: int) -> double:
    cdef char memory_buffer[8]
    _read_memory(console_address, memory_buffer, 8)    
    return (<double*> memory_buffer)[0]


def read_bytes(console_address: int, size: int) -> bytes:
    memory = bytearray(size)
    if not DolphinAccessor.readFromRAM(dolphinAddrToOffset(console_address, DolphinAccessor.isARAMAccessible()), memory, size, False):
        raise RuntimeError(f"Could not read memory at {console_address}: {DolphinAccessor.getLastErrorMessage()}")
    return bytes(memory)


cdef _write_memory(console_address, char* memory_buffer, int size):
    assert_hooked()
    if not DolphinAccessor.writeToRAM(dolphinAddrToOffset(console_address, DolphinAccessor.isARAMAccessible()), memory_buffer, size, True):
        raise RuntimeError(f"Could not write memory at {console_address}: {DolphinAccessor.getLastErrorMessage()}")


def write_byte(console_address: int, value: int):
    cdef char memory_buffer[1]
    (<uint8_t*> memory_buffer)[0] = value
    _write_memory(console_address, memory_buffer, 1)


def write_word(console_address: int, value: int):
    cdef char memory_buffer[4]
    (<uint32_t*> memory_buffer)[0] = value
    _write_memory(console_address, memory_buffer, 4)


def write_float(console_address: int, value: float):
    cdef char memory_buffer[4]
    (<float*> memory_buffer)[0] = value
    _write_memory(console_address, memory_buffer, 4)


def write_double(console_address: int, value: double):
    cdef char memory_buffer[8]
    (<double*> memory_buffer)[0] = value
    _write_memory(console_address, memory_buffer, 8)


def write_bytes(console_address: int, memory: bytes):
    assert_hooked()
    if not DolphinAccessor.writeToRAM(dolphinAddrToOffset(console_address, DolphinAccessor.isARAMAccessible()), memory, len(memory), False):
        raise RuntimeError(f"Could not write memory at {console_address}: {DolphinAccessor.getLastErrorMessage()}")
