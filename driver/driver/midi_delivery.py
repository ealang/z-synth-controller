import logging
from typing import List, Optional, Iterable, Tuple, Dict

import rtmidi
from rtmidi.midiconstants import NRPN_LSB, NRPN_MSB, DATA_ENTRY_MSB, CONTROL_CHANGE
from rtmidi.midiutil import open_midioutput

from .param_mapping import ParamMapping, execute_mapping


logger = logging.getLogger(__name__)

NRPN_MSB_VALUE = 0x10


class MidiDelivery:
    """
    Send messages to midi output port.

    TODO: this code has no idea if messages need to be resent (e.g.
    z-synth restarted and needs all the params again). As a quick solution, it
    is continuously re-sending all params.
    """

    def __init__(self, device_name: str, param_mappings: Dict[int, ParamMapping]) -> None:
        self._param_mappings = param_mappings
        self._device = open_midioutput(
            port=0,
            client_name=device_name,
            interactive=False,
            use_virtual=True,
        )[0]

    def __call__(self, params: List[int]) -> None:
        commands = MidiDelivery._params_to_midi_messages(
            params,
            self._param_mappings,
        )
        for command in commands:
            self._device.send_message(command)

    @classmethod
    def _params_to_midi_messages(cls, params: List[int], param_mappings: Dict[int, ParamMapping]) -> Iterable[Tuple[int, int, int]]:
        """
        Given (param id, param value) tuples, return a sequence of NRPN midi commands.
        """
        if not params:
            return

        yield CONTROL_CHANGE, NRPN_MSB, NRPN_MSB_VALUE

        for param_id, param_value in enumerate(params):
            if param_id not in param_mappings:
                continue

            mapping = param_mappings[param_id]
            yield CONTROL_CHANGE, NRPN_LSB, mapping.nrpn
            yield CONTROL_CHANGE, DATA_ENTRY_MSB, execute_mapping(mapping, param_value)
