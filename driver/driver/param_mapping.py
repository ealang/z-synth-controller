import json
from dataclasses import dataclass
from enum import Enum
from typing import Dict, Any, Tuple, Optional
from .config import MappingType, ParamMapping


def _linear_mapping(input_value: float, params: Dict[str, Any]) -> int:
    """
    Map input to a range of values.

    :param input_value: Value in range [0, 1)
    """
    num_positions = params.get("num_positions", 128)
    result = int(input_value * (num_positions - 1) + 0.5)
    if params.get("invert"):
        return num_positions - 1 - result
    return result


def _weighted_mapping(input_value: float, params: Dict[str, Any]) -> int:
    """
    Map input to a weighted range of values.

    :param input_value: Value in range [0, 1)
    """
    values = params["values"]
    num_positions = sum(
        (end - start + 1) * weight
        for [[start, end], weight] in values
    )
    target_pos = int(input_value * num_positions)
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


def _voltage_to_resistance(v: int) -> float:
    if v == 0:
        return 0
    return (255 / v) - 1


def execute_mapping(mapping: ParamMapping, value: int) -> Optional[int]:
    if value < mapping.adc_ignore_below:
        return None

    adc_min, adc_max = mapping.adc_range
    value = max(min(value, adc_max), adc_min);

    min_r = _voltage_to_resistance(adc_min)
    max_r = _voltage_to_resistance(adc_max + 1)
    value_r = _voltage_to_resistance(value)
    value_norm = (value_r - min_r) / (max_r - min_r)
    return _MAPPING_TYPE_FUNCS[mapping.mapping_type](value_norm, mapping.mapping_params)
