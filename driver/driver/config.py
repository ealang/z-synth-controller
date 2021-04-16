from dataclasses import dataclass
from enum import Enum
from typing import Dict, Any, Tuple, Optional


class MappingType(Enum):
    LINEAR = "linear"
    WEIGHTED = "weighted"


@dataclass
class SensorCalibration:
    adc_range: Tuple[int, int]  # sensor range inclusive
    adc_ignore_below: int  # drop any values below this value, otherwise clamp to `adc_range`

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "SensorCalibration":
        return SensorCalibration(
            adc_range=tuple(data["adc_range"]),
            adc_ignore_below=data["adc_ignore_below"],
        )


@dataclass
class ParamMapping:
    mapping_type: MappingType
    mapping_params: Dict[str, Any]

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "ParamMapping":
        return ParamMapping(
            mapping_type=MappingType(data["type"]),
            mapping_params=data["params"],
        )


@dataclass
class SensorMapping:
    name: str
    nrpn: int  # midi non-registered parameter number lsb
    param_mapping: str

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "SensorMapping":
        return SensorMapping(**data)


@dataclass
class Config:
    sensor_calibration: Dict[int, SensorCalibration]
    sensor_mappings: Dict[str, SensorMapping]
    mappings: Dict[str, ParamMapping]

    @classmethod
    def from_dict(cls, data: Dict[str, Any]) -> "Config":
        return Config(
            sensor_calibration={
                int(s_id): SensorCalibration.from_dict(calibration)
                for s_id, calibration in data["sensor_calibration"].items()
            },
            sensor_mappings={
                int(s_id): SensorMapping.from_dict(mapping)
                for s_id, mapping in data["sensor_mapping"].items()
            },
            mappings={
                name: ParamMapping.from_dict(mapping)
                for name, mapping in data["mappings"].items()
            }
        )
