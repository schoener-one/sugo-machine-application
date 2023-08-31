'''
Contains the plantuml document generator component configuration.


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
            
