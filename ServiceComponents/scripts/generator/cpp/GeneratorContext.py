'''
Contains the C++ generator
'''

__author__      = "denis@schoener-one.de"
__copyright__   = "Copyright 2022, Schoener-One"

from dataclasses import dataclass
from ServiceComponentData import ServiceComponent

@dataclass
class GeneratorContext:
    name: str
    namespace: str
    component: ServiceComponent
    actions: set
    receivers: set
