import dolphin_memory_engine


def test_is_hooked_default():
    assert not dolphin_memory_engine.is_hooked()


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
    # Verify that searching for a non-existent game ID returns None
    pid = dolphin_memory_engine.get_process_id_by_game_id("NONEXIST")
    assert pid is None