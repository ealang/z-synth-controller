import json
from dataclasses import dataclass
from enum import Enum
from typing import Dict, Any


class MappingType(Enum):
    LINEAR = "linear"
    QUANTIZED = "quantized"


@dataclass
class ParamMapping:
    name: str
    nrpn: int  # midi non-registered parameter number lsb
    mapping_type: MappingType
    mapping_params: Dict[str, Any]

    @classmethod
    def from_json(cls, data: Dict[str, Any]) -> "ParamMapping":
        return ParamMapping(
            name=data["name"],
            nrpn=data["nrpn"],
            mapping_type=MappingType(data["mapping_type"]),
            mapping_params=data.get("mapping_params", {}),
        )


def load_param_mappings(file_path: str) -> Dict[int, ParamMapping]:
    with open(file_path) as fp:
        data = json.load(fp)
    return {
        int(param_num): ParamMapping.from_json(param_mapping)
        for param_num, param_mapping in data.items()
    }


def _linear_mapping(value: int, params: Dict[str, Any]) -> int:
    """ From 8 bit sensor value to 7 bit midi. """
    return value >> 1


def _quantized_mapping(value: int, params: Dict[str, Any]) -> int:
    """ From 8 bit sensor value to n position quantized. """
    num_positions = params["num_positions"]
    return int((value / 256) * num_positions)


_MAPPING_TYPE_FUNCS = {
    MappingType.LINEAR: _linear_mapping,
    MappingType.QUANTIZED: _quantized_mapping,
}


def execute_mapping(mapping: ParamMapping, value: int) -> int:
    return _MAPPING_TYPE_FUNCS[mapping.mapping_type](value, mapping.mapping_params)
