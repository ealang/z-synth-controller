import logging
from dataclasses import dataclass
from typing import List, Optional, Iterable, Callable, Dict, Tuple

import rtmidi
from rtmidi.midiconstants import NRPN_LSB, NRPN_MSB, DATA_ENTRY_MSB, CONTROL_CHANGE


logger = logging.getLogger(__name__)

MIDI_CLIENT_NAME = "zs-driver"
NRPN_MSB_VALUE = 0x10


@dataclass
class MidiData:
    nrpn: int
    value: int


class MidiDelivery:
    """ Send messages to target midi port. """

    def __init__(self, delivery_device_name: str, sensor_to_midi: Callable[[int, int], Optional[MidiData]]) -> None:
        """
        :param delivery_device_name: Midi device to send messages to
        :param sensor_to_midi: (sensor_id, sensor_value) -> midi param/value
        """
        self._last_sensor_to_midi_values = {}
        self._sensor_to_midi = sensor_to_midi
        self._device = rtmidi.MidiOut(name=MIDI_CLIENT_NAME)
        self._delivery_device_name = delivery_device_name

    def __call__(self, sensor_values: List[int]) -> None:
        """ Given list of raw sensor values, publish midi messages to device. """
        self._update_connection_state()

        sensor_to_midi_values = MidiDelivery._get_sensor_to_midi_values(sensor_values, self._last_sensor_to_midi_values, self._sensor_to_midi)
        if sensor_to_midi_values == self._last_sensor_to_midi_values:
            return

        messages = MidiDelivery._params_to_midi_messages(
            MidiDelivery._diff_params(self._last_sensor_to_midi_values, sensor_to_midi_values),
        )

        for message in messages:
            self._device.send_message(message)
        self._last_sensor_to_midi_values = sensor_to_midi_values

    def _update_connection_state(self) -> None:
        """
        TODO: This is a suboptimal way to track the availability of z-synth. It would be nice
        to get a callback if z-synth goes down, so we know to resend all the parameters.

        Problems: this fails to detect quick bounces, adds a few ms of latency, and doesn't control the
        connection polling interval.
        """
        for port_num, other_device_name in enumerate(self._device.get_ports()):
            if self._delivery_device_name in other_device_name and other_device_name != MIDI_CLIENT_NAME:
                if not self._device.is_port_open():
                    self._last_sensor_to_midi_values = {}
                    self._device.open_port(port_num)
                return
        self._device.close_port()

    @classmethod
    def _get_sensor_to_midi_values(cls, sensor_values: List[int], last_sensor_to_midi_values: Dict[int, MidiData], sensor_to_midi: Callable[[int, int], Optional[MidiData]]) -> Dict[int, MidiData]:
        lookup = {}
        for sensor_id, sensor_value in enumerate(sensor_values):
            data = sensor_to_midi(sensor_id, sensor_value)
            if data is None:
                data = last_sensor_to_midi_values.get(sensor_id)

            if data is not None:
                lookup[sensor_id] = data
        return lookup

    @classmethod
    def _diff_params(cls, values_before: Dict[int, MidiData], values_after: Dict[int, MidiData]) -> List[Tuple[int, MidiData]]:
        """ Return (sensor id, midi data) pairs that changed. """

        diff = []
        for sensor_id, value in values_after.items():
            if values_before.get(sensor_id) != value:
                diff.append((sensor_id, value))
        return diff

    @classmethod
    def _params_to_midi_messages(cls, params: List[Tuple[int, MidiData]]) -> Iterable[Tuple[int, int, int]]:
        """ Given (sensor id, midi data) pairs, return a sequence of NRPN midi commands. """
        if not params:
            return

        yield CONTROL_CHANGE, NRPN_MSB, NRPN_MSB_VALUE

        for sensor_id, midi_data in params:
            logger.debug("Resending sensor %d", sensor_id)

            yield CONTROL_CHANGE, NRPN_LSB, midi_data.nrpn
            yield CONTROL_CHANGE, DATA_ENTRY_MSB, midi_data.value
