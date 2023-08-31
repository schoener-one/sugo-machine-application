'''
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
'''

__author__      = "denis@schoener-one.de"
__copyright__   = "Copyright (C) 2020 by Denis Schoener"

from dataclasses import dataclass
from typing import List

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
class Notification:
    """Keeps information about notification"""
    name: str
    receivers: list

@dataclass
class Message:
    """Keeps information about messages"""
    name: str
    event: str
    forward: str

    def __str__(self) -> str:
        return self.name

@dataclass
class Inbound:
    """Keeps information about inbound messages"""
    requests: List[Message]
    notifications: List[Message]

@dataclass
class Outbound:
    """Keeps information about outbound messages"""
    notifications: list

@dataclass
class ServiceComponent:
    """Keeps the content of a service component"""
    inbound: Inbound
    outbound: Outbound
    events: list
    statemachine: StateMachine
    