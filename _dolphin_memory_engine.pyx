import os
import sys
from typing import List, Optional
from libc.stdint cimport uint8_t, uint32_t, uint64_t
from libcpp cimport bool as c_bool
from libcpp.string cimport string
from libcpp.vector cimport vector


_DEFAULT_DOLPHIN_NAMES = {
    "win32": ["Dolphin.exe", "DolphinQt2.exe", "DolphinWx.exe"],
    "linux": ["dolphin-emu", "dolphin-emu-qt2", "dolphin-emu-wx"],
    "darwin": ["Dolphin", "dolphin-emu"],
}


def _default_dolphin_names() -> List[str]:
    env_name = os.environ.get("DME_DOLPHIN_PROCESS_NAME")
    if env_name:
        return [env_name]
    return _DEFAULT_DOLPHIN_NAMES.get(sys.platform, [])


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


cdef extern from "DolphinProcess/DolphinStatus.h" namespace "DolphinComm":
    cpdef enum class DolphinStatus:
        HOOKED
        EMULATION_NOT_STARTED
        NOT_FOUND
        UNHOOKED


cdef extern from "DolphinProcess/DolphinAccessor.h" namespace "DolphinComm":
    cdef cppclass DolphinAccessor:
        @staticmethod
        void init()

        @staticmethod
        void free()

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
        DolphinStatus getStatus()

        @staticmethod
        string getLastErrorMessage()
        
        @staticmethod
        c_bool isValidConsoleAddress(uint32_t)

        @staticmethod
        c_bool isARAMAccessible()


cdef extern from "DolphinProcess/DolphinInstance.h" namespace "DolphinComm":
    vector[int] GetProcessIDs(vector[string])

    cdef cppclass DolphinInstance:
        void init()
        void free()
        void hook(int)
        void unHook()
        c_bool readFromRAM(uint32_t, char*, const size_t, c_bool)
        c_bool writeToRAM(uint32_t, const char*, const size_t, c_bool)
        int getPID()
        DolphinStatus getStatus()
        string getLastErrorMessage()
        c_bool isARAMAccessible()
        c_bool isValidConsoleAddress(uint32_t)
        c_bool isARAMAccessible()


cdef extern from "MemoryWatch/MemWatchEntry.h":
    cdef cppclass MemWatchEntry:
        MemWatchEntry()
        MemWatchEntry(string, uint32_t, MemType, MemBase, c_bool, size_t, c_bool)

        char* getMemory()

        void addOffset(int)
        void setTargetProcess(DolphinInstance*)
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


cdef class DolphinProcess:
    """
    An independent, hookable Dolphin process that allows multiple Dolphin
    instances to be hooked and accessed at the same time.
    """

    cdef DolphinInstance* _instance

    def __cinit__(self, pid: Optional[int] = None):
        self._instance = new DolphinInstance()
        self.hook(pid)

    def __dealloc__(self):
        if self._instance != NULL:
            del self._instance

    def hook(self, pid: Optional[int] = None):
        if pid is None:
            pids = get_process_ids()
            pid = pids[0] if pids else -1
        self._instance.hook(pid)
    
    def un_hook(self):
        self._instance.unHook()

    def is_hooked(self) -> bool:
        return self.get_status() == DolphinStatus.HOOKED

    def get_status(self) -> DolphinStatus:
        return self._instance.getStatus()

    def assert_hooked(self):
        if not self.is_hooked():
            raise RuntimeError("not hooked")

    def get_pid(self) -> int:
        return self._instance.getPID()
    
    def follow_pointers(self, console_address: int, pointer_offsets: List[int]) -> int:
        self.assert_hooked()
        real_console_address = console_address

        is_aram_accessible = self._instance.isARAMAccessible()

        cdef char memory_buffer[4]
        for offset in pointer_offsets:
            if self._instance.readFromRAM(dolphinAddrToOffset(real_console_address, is_aram_accessible), memory_buffer, 4, True):
                real_console_address = buffer_to_word(memory_buffer)
                if self._instance.isValidConsoleAddress(real_console_address):
                    real_console_address += offset
                else:
                    raise RuntimeError(f"Address {real_console_address} is not valid")
            else:
                raise RuntimeError(f"Could not read memory at {real_console_address}: {self._instance.getLastErrorMessage()}")

        return real_console_address

    cdef _read_memory(self, console_address, char* memory_buffer, int size):
        self.assert_hooked()
        if not self._instance.readFromRAM(dolphinAddrToOffset(console_address, self._instance.isARAMAccessible()), memory_buffer, size, True):
            raise RuntimeError(f"Could not read memory at {console_address}: {self._instance.getLastErrorMessage()}")

    def read_byte(self, console_address: int) -> int:
        cdef char memory_buffer[1]
        self._read_memory(console_address, memory_buffer, 1)
        return (<uint8_t*> memory_buffer)[0]

    def read_word(self, console_address: int) -> int:
        cdef char memory_buffer[4]
        self._read_memory(console_address, memory_buffer, 4)
        return (<uint32_t*> memory_buffer)[0]
    
    def read_float(self, console_address: int) -> float:
        cdef char memory_buffer[4]
        self._read_memory(console_address, memory_buffer, 4)
        return (<float*> memory_buffer)[0]

    def read_double(self, console_address: int) -> double:
        cdef char memory_buffer[8]
        self._read_memory(console_address, memory_buffer, 8)    
        return (<double*> memory_buffer)[0]

    def read_bytes(self, console_address: int, size: int) -> bytes:
        memory = bytearray(size)
        if not self._instance.readFromRAM(dolphinAddrToOffset(console_address, self._instance.isARAMAccessible()), memory, size, False):
            raise RuntimeError(f"Could not read memory at {console_address}: {self._instance.getLastErrorMessage()}")
        return bytes(memory)

    cdef _write_memory(self, console_address, char* memory_buffer, int size):
        self.assert_hooked()
        if not self._instance.writeToRAM(dolphinAddrToOffset(console_address, self._instance.isARAMAccessible()), memory_buffer, size, True):
            raise RuntimeError(f"Could not write memory at {console_address}: {self._instance.getLastErrorMessage()}")

    def write_byte(self, console_address: int, value: int):
        cdef char memory_buffer[1]
        (<uint8_t*> memory_buffer)[0] = value
        self._write_memory(console_address, memory_buffer, 1)

    def write_word(self, console_address: int, value: int):
        cdef char memory_buffer[4]
        (<uint32_t*> memory_buffer)[0] = value
        self._write_memory(console_address, memory_buffer, 4)

    def write_float(self, console_address: int, value: float):
        cdef char memory_buffer[4]
        (<float*> memory_buffer)[0] = value
        self._write_memory(console_address, memory_buffer, 4)

    def write_double(self, console_address: int, value: double):
        cdef char memory_buffer[8]
        (<double*> memory_buffer)[0] = value
        self._write_memory(console_address, memory_buffer, 8)

    def write_bytes(self, console_address: int, memory: bytes):
        self.assert_hooked()
        if not self._instance.writeToRAM(dolphinAddrToOffset(console_address, self._instance.isARAMAccessible()), memory, len(memory), False):
            raise RuntimeError(f"Could not write memory at {console_address}: {self._instance.getLastErrorMessage()}")


cdef class MemWatch:
    cdef MemWatchEntry c_entry
    cdef object _dolphin_process

    def __cinit__(self, label: str, console_address: int, is_pointer: bool, dolphin_process: Optional[DolphinProcess] = None):
        self.c_entry = MemWatchEntry(label.encode("utf-8"), console_address, MemType.type_word, MemBase.base_decimal, False, 1, is_pointer)
        self._dolphin_process = dolphin_process
        if dolphin_process:
            self.c_entry.setTargetProcess((<DolphinProcess> dolphin_process)._instance)

    def add_offset(self, offset: int):
        self.c_entry.addOffset(offset)

    def get_value(self):
        return buffer_to_word(self.c_entry.getMemory())
        
    def read_memory_from_ram(self):
        return self.c_entry.readMemoryFromRAM() == MemOperationReturnCode.OK

    def write_memory_from_string(self, value: str):
        return self.c_entry.writeMemoryFromString(value.encode("utf-8")) == MemOperationReturnCode.OK


def hook(pid: Optional[int] = None):
    if pid is None:
        pids = get_process_ids()
        pid = pids[0] if pids else -1
    return DolphinAccessor.hook(pid)


def un_hook():
    return DolphinAccessor.unHook()


def is_hooked() -> bool:
    if DolphinAccessor.getStatus() == DolphinStatus.HOOKED:
        return True
    else:
        return False


def get_process_ids(dolphin_names: Optional[List[str]] = None) -> list[int]:
    """
    Get all process IDs of running Dolphin instances.
    If dolphin_names is omitted, resolves DME_DOLPHIN_PROCESS_NAME or the platform's default names.
    """
    if dolphin_names is None:
        dolphin_names = _default_dolphin_names()
    return GetProcessIDs([name.encode("utf-8") for name in dolphin_names])


def get_pid() -> int:
    return DolphinAccessor.getPID();


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
