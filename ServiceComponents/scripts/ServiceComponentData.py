'''
Contains the parser to generator data classes
'''

__author__      = "denis@schoener-one.de"
__copyright__   = "Copyright 2022, Schoener-One"

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
    commands: List[Message]
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
    