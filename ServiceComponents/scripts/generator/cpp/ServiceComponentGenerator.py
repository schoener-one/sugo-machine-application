'''
Contains the C++ generator for source and header files
'''

__author__      = "denis@schoener-one.de"
__copyright__   = "Copyright 2022, Schoener-One"

from pathlib import Path
import logging
import re
from .GeneratorContext import GeneratorContext
from .ServiceComponentHeaderGenerator import ServiceComponentHeaderGenerator
from .ServiceComponentSourceGenerator import ServiceComponentSourceGenerator

class Generator:
    """Class to generate the C++ files"""
    def __init__(self, out_dir) -> None:
        self._out_dir = Path(out_dir)
        self._out_dir.mkdir(parents=True, exist_ok=True)

    def _source_file_path(self, component_name):
        inc_path = Path(self._out_dir / 'src')
        return Path(inc_path / f"I{component_name}.cpp")
    
    def _header_file_path(self, component_name):
        inc_path = Path(self._out_dir / 'include')
        return Path(inc_path / f"I{component_name}.hpp")
    
    def print_output_files(self, components):
        for component_name in components.keys():
            path = self._source_file_path(component_name)
            print(str(path))
            path = self._header_file_path(component_name)
            print(str(path))
    
    def generate(self, components):
        for name, component in components.items():
            namespace = re.sub( '(?<!^)(?=[A-Z])', '_', name).lower()
            context = GeneratorContext(name, namespace, component, set([trans.action for trans in component.statemachine.transitions if trans.action]),
                                       set([receiver for receiver in components.keys() if receiver != name]))
            logging.info(f"generating component I{context.name} files")
            ServiceComponentHeaderGenerator(context).generate(self._header_file_path(context.name))
            ServiceComponentSourceGenerator(context).generate(self._source_file_path(context.name))
