'''
Contains the parser to generator data classes
'''

__author__      = "denis@schoener-one.de"
__copyright__   = "Copyright 2022, Schoener-One"

from dataclasses import dataclass

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
class ServiceComponent:
    """Keeps the content of a service component"""
    commands: list
    notifications: list
    events: list
    statemachine: StateMachine
    