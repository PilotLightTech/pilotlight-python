"""
CPU profiling helpers built on plProfileI

    from pilotlight.profile import profile_sample, profiled

    with profile_sample("update scene"):
        ...

    @profiled                 # sample name: the function's qualified name
    def draw_grid(self): ...

Samples show up in the profiler tool window (PL_STARTER_FLAGS_PROFILE_EXT).
"""

import functools
from contextlib import contextmanager

from pilotlight.pilotlight import plProfileI


@contextmanager
def profile_sample(name: str, threadIndex: int = 0):
    """Times the enclosed block; the sample ends even if the block raises."""
    plProfileI.begin_sample(threadIndex, name)
    try:
        yield
    finally:
        plProfileI.end_sample(threadIndex)


def profiled(func=None, *, name: str | None = None, threadIndex: int = 0):
    """Decorator form of profile_sample (optionally @profiled(name="..."))."""

    def decorate(f):
        sample_name = name or f.__qualname__

        @functools.wraps(f)
        def wrapper(*args, **kwargs):
            plProfileI.begin_sample(threadIndex, sample_name)
            try:
                return f(*args, **kwargs)
            finally:
                plProfileI.end_sample(threadIndex)

        return wrapper

    return decorate(func) if func is not None else decorate
