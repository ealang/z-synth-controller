import json
from dataclasses import dataclass
from enum import Enum
from typing import Dict, Any


class MappingType(Enum):
    LINEAR = "linear"
    WEIGHTED = "weighted"


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
    """ From 8 bit sensor value to n position quantized. """
    num_positions = params.get("num_positions", 128)
    return int((value / 256) * num_positions)


def _weighted_mapping(input_value: int, params: Dict[str, Any]) -> int:
    """ From 8 bit sensor value to weighted space. """
    values = params["values"]
    num_positions = sum(
        (end - start + 1) * weight
        for [[start, end], weight] in values
    )
    target_pos = int((input_value / 256) * num_positions)
    cur_pos = 0
    for [[start, end], weight] in values:
        for output_value in range(start, end + 1):
            if target_pos - cur_pos < weight:
                return output_value
            cur_pos += weight

    raise Exception(f"Failed to map {value}")


_MAPPING_TYPE_FUNCS = {
    MappingType.LINEAR: _linear_mapping,
    MappingType.WEIGHTED: _weighted_mapping,
}


def execute_mapping(mapping: ParamMapping, value: int) -> int:
    return _MAPPING_TYPE_FUNCS[mapping.mapping_type](value, mapping.mapping_params)
