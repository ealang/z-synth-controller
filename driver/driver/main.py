import argparse
import logging
import os
from queue import Queue, Empty
from typing import TypeVar, List

from .board_receiver import BoardReceiver
from .board_packet import BoardLogging, ActiveParameters
from .midi_delivery import MidiDelivery
from .param_mapping import load_param_mappings


logger = logging.getLogger(__name__)


T = TypeVar("T")

def _queue_get(queue: Queue) -> T:
    """
    Blocking get next message.
    Workaround for "uninterruptible wait" described in python queue docs.
    """
    while True:
        try:
            return queue.get(timeout=1)
        except Empty:
            pass


def _loop(board_queue: Queue, midi_delivery: MidiDelivery) -> None:
    while True:
        packet = _queue_get(board_queue)
        if isinstance(packet, ActiveParameters):
            midi_delivery(packet.values)
        if isinstance(packet, BoardLogging):
            logger.info(f"Board logging: {packet.message}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--mapping-config", required=True)
    parser.add_argument("--serial-dev", default="/dev/ttyUSB0")
    parser.add_argument("--midi-dev", default="z-synth", help="Midi device to connect to (first substring match)")
    parser.add_argument("--baudrate", default=50000, type=int, help="Serial device baudrate in Hz")
    parser.add_argument("-v", "--verbose", action="store_true", help="Verbose logging")

    args = parser.parse_args()

    logging.basicConfig(level="DEBUG" if args.verbose else "INFO")

    if not os.path.exists(args.mapping_config):
        raise Exception("Unable to open mapping config file")

    param_mappings = load_param_mappings(args.mapping_config)

    logger.debug(f"Using serial dev: {args.serial_dev}, midi dev: {args.midi_dev}")

    midi_delivery = MidiDelivery(args.midi_dev, param_mappings)

    board_queue = Queue()
    board = BoardReceiver(
        device=args.serial_dev,
        publish_queue=board_queue,
        baudrate=args.baudrate,
    )
    try:
        board.start()
        _loop(board_queue, midi_delivery)

    except KeyboardInterrupt:
        board.shutdown()
        board.join()


if __name__ == "__main__":
    main()
