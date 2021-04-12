from queue import Queue, Empty
from typing import TypeVar


T = TypeVar("T")

def queue_get(queue: Queue) -> T:
    """
    Blocking get next message.
    Workaround for "uninterruptible wait" described in python queue docs.
    """
    while True:
        try:
            return queue.get(timeout=1)
        except Empty:
            pass
