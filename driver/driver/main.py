import argparse
import json
import logging
import os
from queue import Queue
from typing import Optional

from .board_receiver import BoardReceiver
from .board_packet import BoardLogging, ActiveParameters
from .config import Config
from .midi_delivery import MidiDelivery, MidiData
from .param_mapping import execute_mapping
from .queue_get import queue_get


logger = logging.getLogger(__name__)


def _process_packet_loop(board_queue: Queue, midi_delivery: MidiDelivery) -> None:
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

    def sensor_to_midi_data(sensor_id: int, sensor_value: int) -> Optional[MidiData]:
        if not sensor_id in config.sensor_mappings:
            return None

        sensor_mapping = config.sensor_mappings[sensor_id]
        if sensor_mapping.nrpn < 0:  # supporting as a placeholder for unimplemented params
            return None

        param_mapping = config.param_mappings[sensor_mapping.param_mapping]
        calibration = config.adc_calibrations[sensor_id]

        midi_value = execute_mapping(
            sensor_value,
            calibration,
            param_mapping,
        )
        if midi_value is None:
            # Note: usually this is a 8-way switch that hasn't settled on a valid value
            logger.debug("Discarding unmappable value: sensor_id=%d, sensor_value=%d", sensor_id, sensor_value)
            return None

        return MidiData(
            nrpn=sensor_mapping.nrpn,
            value=midi_value,
        )

    midi_delivery = MidiDelivery(
        args.midi_dev,
        sensor_to_midi_data,
    )

    board_queue = Queue()
    board = BoardReceiver(
        device=args.serial_dev,
        publish_queue=board_queue,
        baudrate=args.baudrate,
    )
    try:
        board.start()
        _process_packet_loop(board_queue, midi_delivery)
    except KeyboardInterrupt:
        pass
    finally:
        board.shutdown()
        board.join()


if __name__ == "__main__":
    main()
