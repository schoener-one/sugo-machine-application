'''
Contains the plantuml document generator component configuration
'''

__author__      = "denis@schoener-one.de"
__copyright__   = "Copyright 2022, Schoener-One"

from dataclasses import dataclass
from pathlib import Path
import logging

from ServiceComponentData import ServiceComponent
    
class Generator:
    """Class to generate the C++ files"""
    def __init__(self, out_dir) -> None:
        self._out_dir = Path(out_dir)
        self._out_dir.mkdir(parents=True, exist_ok=True)

    def _doc_file_path(self, component_name):
        inc_path = Path(self._out_dir / 'doc')
        return Path(inc_path / f"{component_name}.plantuml")
          
    def generate(self, components):
        for component_name, component in components.items():
            logging.info(f"generating component {component_name} state machine")
            self._generate_state_machine(component_name, component)
    
    def _generate_state_machine(self, component_name, component):
        path = self._doc_file_path(component_name)
        path.parent.mkdir(parents=True, exist_ok=True)
        newline = '\n'
        with path.open("w", encoding="utf-8") as file:
            file.write(f'''
@startuml

state {component_name} {{
  [*] --> {component.statemachine.start}

{Generator._generate_transitions(component.statemachine)}
}}
@enduml
''')
    
    @staticmethod
    def _generate_transitions(statemachine):
        out_str = ''
        for trans in statemachine.transitions:
            out_str += f'''  {trans.state} --> {trans.next}: {trans.event}
'''
        return out_str
            
