import dolphin_memory_engine


def test_is_hooked_default():
    assert not dolphin_memory_engine.is_hooked()


def test_hook_unhook():
    dolphin_memory_engine.hook()
    dolphin_memory_engine.un_hook()
    assert not dolphin_memory_engine.is_hooked()
