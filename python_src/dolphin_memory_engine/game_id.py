from typing import List, Optional

from . import DolphinProcess, assert_hooked, get_process_ids, hook, read_bytes


def get_game_id(length: int = 6, dolphin_process: Optional[DolphinProcess] = None) -> str:
    """
    Get the Game ID of the currently hooked Dolphin instance (or provided DolphinProcess)
    Supports custom length to support extended IDs from mods.
    """

    # Read the specified number of bytes from the start of MEM1 (0x80000000)
    if dolphin_process is not None:
        dolphin_process.assert_hooked()
        data = dolphin_process.read_bytes(0x80000000, length)
    else:
        assert_hooked()
        data = read_bytes(0x80000000, length)
    return data.decode("ascii", errors="replace")


def get_process_ids_by_game_id(game_id: str, dolphin_names: Optional[List[str]] = None) -> List[int]:
    """
    Finds the process ID of the running Dolphin instance playing the game with the given Game ID.
    Supports dynamic Game ID lengths and custom process names.
    Returns the first matching PID, or None if no matching process is found.
    """
    matched_pids: List[int] = []
    for pid in get_process_ids(dolphin_names):
        proc = DolphinProcess(pid)
        if not proc.is_hooked():
            continue

        try:
            proc_game_id = get_game_id(len(game_id), proc)
        except RuntimeError:
            continue

        if proc_game_id == game_id:
            matched_pids.append(pid)

    return matched_pids


def hook_by_game_id(game_id: str, dolphin_names: Optional[List[str]] = None) -> bool:
    """
    Automatically search for and hook to the Dolphin process playing the specified Game ID.
    Returns True if successfully hooked, False otherwise.
    """
    pid = get_process_ids_by_game_id(game_id, dolphin_names)
    if pid is not None:
        hook(pid[0])
        return True
    return False
