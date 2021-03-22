import logging
from typing import List, Optional, Iterable, Tuple, Dict

import rtmidi
from rtmidi.midiconstants import NRPN_LSB, NRPN_MSB, DATA_ENTRY_MSB, CONTROL_CHANGE
from rtmidi.midiutil import open_midioutput

from .param_mapping import ParamMapping, execute_mapping


logger = logging.getLogger(__name__)

MIDI_CLIENT_NAME = "zs-driver"
NRPN_MSB_VALUE = 0x10


class MidiDelivery:
    """ Send messages to target midi port. """

    def __init__(self, delivery_device_name: str, param_mappings: Dict[int, ParamMapping]) -> None:
        self._last_params = None
        self._param_mappings = param_mappings
        self._device = rtmidi.MidiOut(name=MIDI_CLIENT_NAME)
        self._delivery_device_name = delivery_device_name

    def __call__(self, params: List[int]) -> None:
        MidiDelivery._update_connection_state(self._device, self._delivery_device_name)
        if not self._device.is_port_open():
            self._last_params = None
            return

        midi_params = MidiDelivery._sensor_to_midi_values(params, self._param_mappings)

        commands = MidiDelivery._params_to_midi_messages(
            MidiDelivery._diff_params(self._last_params, midi_params),
            self._param_mappings,
        )
        for command in commands:
            self._device.send_message(command)
        self._last_params = midi_params

    @classmethod
    def _update_connection_state(cls, device: rtmidi.MidiOut, delivery_device_name: str) -> None:
        """
        TODO: This is a suboptimal way to track the availability of z-synth. It would be nice
        to get a callback if z-synth goes down, so we know to resend all the parameters.

        Problems: this fails to detect quick bounces, adds a few ms of latency, and doesn't control the
        connection polling interval.
        """
        for port_num, other_device_name in enumerate(device.get_ports()):
            if delivery_device_name in other_device_name and other_device_name != MIDI_CLIENT_NAME:
                if not device.is_port_open():
                    device.open_port(port_num)
                return
        device.close_port()

    @classmethod
    def _sensor_to_midi_values(cls, params: List[int], param_mappings: Dict[int, ParamMapping]) -> List[int]:
        """ Use configured mapping to determine midi value for each parameter. """
        return [
            (
                execute_mapping(param_mappings[param_id], param_value)
                if param_id in param_mappings else 0
            )
            for param_id, param_value in enumerate(params)
        ]

    @classmethod
    def _params_to_midi_messages(cls, params: List[Tuple[int, int]], param_mappings: Dict[int, ParamMapping]) -> Iterable[Tuple[int, int, int]]:
        """
        Given (param id, param midi value) tuples, return a sequence of NRPN midi commands.
        """
        if not params:
            return

        yield CONTROL_CHANGE, NRPN_MSB, NRPN_MSB_VALUE

        for param_id, param_midi_value in params:
            if param_id not in param_mappings:
                logger.debug("No config present for param %d", param_id)
                continue

            logger.debug("Resending param %d", param_id)

            yield CONTROL_CHANGE, NRPN_LSB, param_mappings[param_id].nrpn
            yield CONTROL_CHANGE, DATA_ENTRY_MSB, param_midi_value

    @classmethod
    def _diff_params(cls, before: Optional[List[int]], after: List[int]) -> List[Tuple[int, int]]:
        return [
            (param_id, param_value)
            for param_id, param_value in enumerate(after)
            if not before or before[param_id] != after[param_id]
        ]
