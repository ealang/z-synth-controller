import argparse
import json
from queue import Queue
from typing import List, Iterable

from .board_receiver import BoardReceiver
from .board_packet import ActiveParameters
from .queue_get import queue_get


IGNORE_BELOW_VAL = 100


def _receive_values(board_queue: Queue) -> Iterable[List[int]]:
    while True:
        packet = queue_get(board_queue)
        if not isinstance(packet, ActiveParameters):
            continue
        yield packet.values


def _replace_nullable(func, val1, val2):
    if val1 is None:
        return val2
    if val2 is None:
        return val1
    return func(val1, val2)


def _loop(board_queue: Queue) -> None:
    min_values = None
    max_values = None

    try:
        for values in _receive_values(board_queue):
            values = [(_ if _ >= IGNORE_BELOW_VAL else None) for _ in values]

            if min_values is None:
                min_values = values
            if max_values is None:
                max_values = values

            min_values = [_replace_nullable(min, prev, cur) for prev, cur in zip(min_values, values)]
            max_values = [_replace_nullable(max, prev, cur) for prev, cur in zip(max_values, values)]

            print(", ".join(f"{i}: {format(v, 'x')}" for i, v in enumerate(values) if v))
    except KeyboardInterrupt:
        return min_values, max_values


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("outfile")
    parser.add_argument("--serial-dev", default="/dev/ttyUSB0")
    parser.add_argument("--baudrate", default=50000, type=int, help="Serial device baudrate in Hz")

    args = parser.parse_args()

    board_queue = Queue()
    board = BoardReceiver(
        device=args.serial_dev,
        publish_queue=board_queue,
        baudrate=args.baudrate,
    )

    board.start()

    min_values, max_values = _loop(board_queue)
    with open(args.outfile, "w") as fp:
        json.dump(
            {
                i: {
                    "adc_range": [min_value, max_value],
                    "adc_ignore_below": IGNORE_BELOW_VAL,
                }
                for i, (min_value, max_value) in enumerate(zip(min_values, max_values))
                if None not in (min_value, max_value)
            },
            fp,
            indent=4,
        )

    board.shutdown()
    board.join()


if __name__ == "__main__":
    main()
