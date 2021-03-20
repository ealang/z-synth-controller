from dataclasses import dataclass
from typing import List


@dataclass
class BoardPacket:
    pass


@dataclass
class ActiveParameters(BoardPacket):
    values: List[int]


@dataclass
class BoardLogging(BoardPacket):
    message: str