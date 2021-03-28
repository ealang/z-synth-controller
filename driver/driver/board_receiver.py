import logging
import time
from queue import Queue
from threading import Thread
from typing import Optional

import serial

from .board_packet import BoardPacket, ActiveParameters, BoardLogging


TIMEOUT = 0.005
PACKET_SIZE = 34
DEVICE_POLL_TIME = 1

PACKET_HEADER_ACTIVE_PARAMS = 1
PACKET_HEADER_LOGGING       = 2

logger = logging.getLogger(__name__)


def _parse_packet(data: bytes) -> Optional[BoardPacket]:
    csum = (1 + sum(data[:PACKET_SIZE - 1])) & 0xFF
    if csum != data[-1]:
        return None

    header = data[0]
    body = data[1: -1]

    if header == PACKET_HEADER_ACTIVE_PARAMS:
        return ActiveParameters(
            values=[int(_) for _ in body],
        )
    if header == PACKET_HEADER_LOGGING:
        return BoardLogging(
            message=body.decode('ascii').strip(),
        )

    return None


def _forever_device_reader(device_name: str, baudrate: int):
    """ Auto-reconnecting serial device reader. """
    cur_device = None

    def _open_device() -> Optional[serial.Serial]:
        try:
            logger.debug("Opening serial device %s", device_name)
            return serial.Serial(
                device_name,
                baudrate=baudrate,
                stopbits=serial.STOPBITS_TWO,
                timeout=TIMEOUT,
            )
        except serial.SerialException:
            logger.debug("Failed to open serial device %s", device_name)
            return None

    def read(num_bytes) -> Optional[bytes]:
        nonlocal cur_device

        if cur_device is None:
            cur_device = _open_device()
            if cur_device is None:
                time.sleep(DEVICE_POLL_TIME)
                return None

        try:
            return cur_device.read(num_bytes)
        except serial.SerialException:
            logger.debug("Failed to read from serial device %s", device_name)
            cur_device = None
            return None

    return read


class BoardReceiver(Thread):
    def __init__(self, device: str, publish_queue: Queue, baudrate: int) -> None:
        super().__init__()
        self._device_reader = _forever_device_reader(device, baudrate)
        self._publish_queue = publish_queue
        self._exit = False

    def shutdown(self) -> None:
        self._exit = True

    def run(self) -> None:
        while not self._exit:
            data = self._device_reader(PACKET_SIZE)
            if data:
                if len(data) == PACKET_SIZE:
                    packet = _parse_packet(data)
                    logger.debug(packet)
                    if packet:
                        self._publish_queue.put(packet)
                    else:
                        logger.warning("Dropping unparseable packet")
                else:
                    logger.warning(f"Ignoring incorrect size packet {len(data)} bytes")
