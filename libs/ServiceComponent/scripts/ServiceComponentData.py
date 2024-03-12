"""
Contains the parser to generator data classes.


@license: Copyright (C) 2020 by Denis Schoener

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>.
"""

__author__ = "denis@schoener-one.de"
__copyright__ = "Copyright (C) 2020 by Denis Schoener"

from dataclasses import dataclass
from typing import List
from enum import Enum


@dataclass
class Transition:
    """Contains information about a transition"""

    state: str
    next: str
    event: str
    action: str


@dataclass
class StateMachine:
    """Contains the state machine information"""

    states: list
    start: str
    transitions: list


@dataclass
class Nameable:
    """Nameable type class"""

    name: str

    def __str__(self) -> str:
        return self.name

    def get_normalized_name(self) -> str:
        return self.name.replace(".", "")

@dataclass
class Notification(Nameable):
    """Contains information about a notification"""


@dataclass
class Subscription(Nameable):
    """Contains information about a subscription"""

    publishers: list

@dataclass
class PropertyChange:
    """Contains information about how to a property change item"""
    property: str
    value: str

@dataclass
class Command(Nameable):
    """Contains information about a command"""
    event: str
    forward: str

@dataclass
class SubscribedNotification(Nameable):
    """Contains information about a subscribed notification"""
    event: str
    action: str
    change: List[PropertyChange]


class PrimitiveTypes(Enum):
    CHAR = "char"
    INT8 = "int8_t"
    INT16 = "int16_t"
    INT32 = "int32_t"
    INT64 = "int64_t"
    UINT8 = "uint8_t"
    UINT16 = "uint16_t"
    UINT32 = "uint32_t"
    UINT64 = "uint64_t"
    BOOL = "bool"
    BYTE = UINT8

    @classmethod
    def from_declaration(cls, type):
        return getattr(cls, str(type).upper())


@dataclass
class TypeDeclaration:
    """Contains information about type declaration"""

    type: PrimitiveTypes
    cardinality: int = 1

    def __str__(self) -> str:
        if self.cardinality > 1:
            return str(self.type.value) + f" [{self.cardinality}]"
        else:
            return str(self.type.value)

    def is_array(self) -> bool:
        return self.cardinality > 1
    
@dataclass
class Property(Nameable):
    """Contains information about a primitive property"""
    GET_PREFIX = 'Get'
    SET_PREFIX = 'Set'
    
    type: TypeDeclaration
    value: str
    min: str
    max: str
    notification: str
    readonly: bool = True

    def get_all_normalized_names(self) -> List[str]:
        normalized_name = self.get_normalized_name()
        all = [ Property.GET_PREFIX + normalized_name ]
        if not self.readonly:
            all.append(Property.SET_PREFIX + normalized_name)
        return all
    
    def get_cpp_value(self) -> str:
        #TODO convert value correctly to appropriate CPP type!
        return str(self.value).lower()

@dataclass
class Interface:
    """Contains information about interface messages"""

    commands: List[Command]
    notifications: List[Notification]
    properties: List[Property]

@dataclass
class Utilization:
    """Contains information about utilization messages"""

    notifications: List[SubscribedNotification]


@dataclass
class ServiceComponent:
    """Contains the content of a service component"""

    interface: Interface
    utilization: List[Subscription]
    events: list
    statemachine: StateMachine
