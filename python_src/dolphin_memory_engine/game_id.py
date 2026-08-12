from typing import List, Optional
from . import assert_hooked, get_process_ids, hook, read_bytes, read_bytes_from_process

def get_process_id_by_game_id(game_id: str, dolphin_names: Optional[List[str]] = None) -> int:
    """
    Find the process ID of the running Dolphin instance playing the game with the given Game ID.
    Supports dynamic Game ID lengths and custom process names.
    Returns None if no matching process is found.
    """
    length = len(game_id)
    for pid in get_process_ids(dolphin_names):
        try:
            data = read_bytes_from_process(pid, length)
        except RuntimeError:
            continue
        if data.decode("ascii", errors="replace") == game_id:
            return pid
    return None


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
