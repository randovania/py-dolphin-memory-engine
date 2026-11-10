import dolphin_memory_engine
from dolphin_memory_engine import game_id


def test_is_hooked_default():
    assert not dolphin_memory_engine.is_hooked()
    assert dolphin_memory_engine.get_pid() == -1


def test_hook_unhook():
    dolphin_memory_engine.hook()
    dolphin_memory_engine.un_hook()
    assert not dolphin_memory_engine.is_hooked()


def test_get_process_ids():
    # Verify that get_process_ids returns a list (empty if Dolphin is not running, or containing PIDs)
    pids = dolphin_memory_engine.get_process_ids()
    assert isinstance(pids, list)
    for pid in pids:
        assert isinstance(pid, int)


def test_get_process_id_by_game_id_not_found():
    # Verify that searching for a non-existent game ID returns [] for multiple
    pids = game_id.get_process_ids_by_game_id("NONEXIST")
    assert pids == []


def test_dolphin_process_nothing_running():
    # Verify if no Dolphin running, pid=None should resolve to nothing and leave DolphinProcess unhooked.
    process = dolphin_memory_engine.DolphinProcess()
    assert not process.is_hooked()
    assert dolphin_memory_engine.get_pid() == -1
    process.un_hook()


def test_dolphin_process_no_singleton():
    # Verifies DolphinProcess not sharing same hook state as legacy Singleton.
    process = dolphin_memory_engine.DolphinProcess()
    assert not dolphin_memory_engine.is_hooked()
    assert not process.is_hooked()


def test_memwatch_with_dolphin_process():
    # Verify MemWatch accepts a DolphinProcess target
    process = dolphin_memory_engine.DolphinProcess()
    watch = dolphin_memory_engine.MemWatch("test", 0x80000000, False, process)
    assert isinstance(watch.read_memory_from_ram(), bool)
