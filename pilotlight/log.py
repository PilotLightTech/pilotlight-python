"""
Python logging -> Pilot Light log channels (plLogI)

    import logging
    from pilotlight.log import PilotLightLogHandler

    logger = logging.getLogger("my_app")
    logger.addHandler(PilotLightLogHandler("My App"))
    logger.setLevel(logging.DEBUG)
    logger.info("loaded %d assets", 12)   # shows in the engine's log window

The handler creates (or reuses) a channel with that name. The channel's own
level (plLogI.set_level) still filters after python's logging levels.
"""

import logging

from pilotlight.pilotlight import plLogI
from pilotlight.enums import plLogChannelType
from pilotlight.types import plLogExtChannelInit


def get_or_add_channel(name: str, init: plLogExtChannelInit | None = None) -> int:
    """Returns the id of the channel called name, creating it if needed."""
    channel = plLogI.get_channel_id(name)
    if channel is None:
        if init is None:
            init = plLogExtChannelInit(
                plLogChannelType.PL_LOG_CHANNEL_TYPE_CONSOLE | plLogChannelType.PL_LOG_CHANNEL_TYPE_CYCLIC_BUFFER)
        channel = plLogI.add_channel(name, init)
    return channel


class PilotLightLogHandler(logging.Handler):
    """Forwards python log records to a Pilot Light log channel."""

    def __init__(self, channel: str | int, level: int = logging.NOTSET):
        super().__init__(level)
        self.channel = channel if isinstance(channel, int) else get_or_add_channel(channel)

    def emit(self, record: logging.LogRecord):
        try:
            message = self.format(record)
            if record.levelno >= logging.CRITICAL:
                plLogI.fatal(self.channel, message)
            elif record.levelno >= logging.ERROR:
                plLogI.error(self.channel, message)
            elif record.levelno >= logging.WARNING:
                plLogI.warn(self.channel, message)
            elif record.levelno >= logging.INFO:
                plLogI.info(self.channel, message)
            elif record.levelno >= logging.DEBUG:
                plLogI.debug(self.channel, message)
            else:
                plLogI.trace(self.channel, message)
        except Exception:
            self.handleError(record)
