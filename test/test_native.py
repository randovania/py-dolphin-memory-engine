import dolphin_memory_engine


def test_is_hooked_default():
    assert not dolphin_memory_engine.is_hooked()
