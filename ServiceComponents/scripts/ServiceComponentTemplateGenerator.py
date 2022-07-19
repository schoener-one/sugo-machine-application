'''
Contains the C++ generator for source and header files
'''

__author__      = "denis@schoener-one.de"
__copyright__   = "Copyright 2022, Schoener-One"

from dataclasses import dataclass
from pathlib import Path
import logging
import re
from datetime import datetime

from ServiceComponentData import ServiceComponent

@dataclass
class GeneratorContext:
    name: str
    namespace: str
    component: ServiceComponent
    actions: set
    receivers: set
    
class Generator:
    """Class to generate the C++ files"""
    def __init__(self, out_dir) -> None:
        self._out_dir = Path(out_dir)
        self._out_dir.mkdir(parents=True, exist_ok=True)

    def _source_file_path(self, component_name):
        inc_path = Path(self._out_dir / 'templates' / 'src')
        return Path(inc_path / f"{component_name}.cpp")
    
    def _header_file_path(self, component_name):
        inc_path = Path(self._out_dir / 'templates' / 'include')
        return Path(inc_path / f"{component_name}.hpp")
    
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
            logging.info(f"generating component {context.name} files")
            self._generate_headers(context)
            self._generate_source(context)
    
    def _generate_headers(self, context):
        path = self._header_file_path(context.name)
        path.parent.mkdir(parents=True, exist_ok=True)
        newline = '\n'
        with path.open("w", encoding="utf-8") as file:
            file.write(f'''
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   {datetime.now().strftime('%Y-%m-%d')}
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "I{context.name}.hpp"

namespace sugo
{{
/**
 * @class {context.name}
 */
class {context.name} : public I{context.name}
{{
public:    
    // Constructor / Destructor
    explicit {context.name}(ICommandMessageBroker& messageBroker) :I{context.name}(messageBroker)
    {{
    }}
    virtual ~{context.name}()
    {{
    }}
   
protected:
    
{Generator._generate_command_handler_declarations(context.component.commands)}
{Generator._generate_trans_actions_declarations(context.actions, context.name)}
}};

}} // namespace sugo
''')

    @staticmethod
    def _generate_command_handler_declarations(commands):
        out_str = '    // Command handlers\n'
        for command in commands:
            command_name = command.replace('.', '')
            out_str += f'    message::CommandResponse onCommand{command_name}(const message::Command& command) override;\n'
        return out_str

    @staticmethod
    def _generate_trans_actions_declarations(actions, component_name):
        out_str = '    // Transition actions\n'
        for action in actions:
            out_str += f'    void {action}(const Event& event, const State& state) override;\n'
        return out_str

    def _generate_source(self, context):
        path = self._source_file_path(context.name)
        path.parent.mkdir(parents=True, exist_ok=True)
        newline = '\n'
        with path.open("w", encoding="utf-8") as file:
            file.write(f'''
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   {datetime.now().strftime('%Y-%m-%d')}
 */
///////////////////////////////////////////////////////////////////////////////

#include "{context.name}.hpp"

using namespace sugo;

///////////////////////////////////////////////////////////////////////////////
// Commands:
{Generator._generate_command_handler_definitions(context)}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:
{Generator._generate_action_method_definitions(context)}
''')
      
    @staticmethod
    def _generate_command_handler_definitions(context):
        out_str = ''
        for command in context.component.commands:
            command_name = command.replace('.', '')
            out_str += f'''
message::CommandResponse {context.name}::onCommand{command_name}(const message::Command&)
{{
    LOG(debug) << "command {command} received in {context.name}...";
    return message::CommandResponse();
}}
'''
        return out_str
    
    @staticmethod
    def _generate_action_method_definitions(context):
        out_str = ''
        for action in context.actions:
            out_str += f'''
void {context.name}::{action}(const I{context.name}::Event&, const I{context.name}::State&)
{{
    LOG(debug) << "transition action {action} in {context.name}...";
}}
'''
        return out_str
