import argparse
import json
import logging
import os
from queue import Queue

from .board_receiver import BoardReceiver
from .board_packet import BoardLogging, ActiveParameters
from .config import Config
from .midi_delivery import MidiDelivery
from .queue_get import queue_get


logger = logging.getLogger(__name__)


def _loop(board_queue: Queue, midi_delivery: MidiDelivery) -> None:
    while True:
        packet = queue_get(board_queue)
        if isinstance(packet, ActiveParameters):
            midi_delivery(packet.values)
        if isinstance(packet, BoardLogging):
            logger.info(f"Board logging: {packet.message}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--config", required=True)
    parser.add_argument("--serial-dev", default="/dev/ttyUSB0")
    parser.add_argument("--midi-dev", default="z-synth", help="Midi device to connect to (first substring match)")
    parser.add_argument("--baudrate", default=50000, type=int, help="Serial device baudrate in Hz")
    parser.add_argument("-v", "--verbose", action="store_true", help="Verbose logging")

    args = parser.parse_args()

    logging.basicConfig(level="DEBUG" if args.verbose else "INFO")

    if not os.path.exists(args.config):
        raise Exception("Unable to open mapping config file")

    with open(args.config) as fp:
        config = Config.from_dict(json.load(fp))

    logger.debug(f"Using serial dev: {args.serial_dev}, midi dev: {args.midi_dev}")

    midi_delivery = MidiDelivery(args.midi_dev, config.param_mappings)

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
        pass
    finally:
        board.shutdown()
        board.join()


if __name__ == "__main__":
    main()
