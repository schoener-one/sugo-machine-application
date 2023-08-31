'''
Contains the C++ generator for source and header files.

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
///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: AUTOMATIC GENERATED FILE!
 * @data: {datetime.now().strftime('%Y-%m-%d')}
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Common/ServiceLocator.hpp"
#include "ServiceComponent/I{context.name}.hpp"

namespace sugo::service_component
{{
/**
 * @class {context.name}
 */
class {context.name} : public I{context.name}
{{
public:    
    // Constructor / Destructor
    explicit {context.name}(IMessageBroker& messageBroker, const ServiceLocator& serviceLocator) 
        :I{context.name}(messageBroker), m_serviceLocator(serviceLocator)
    {{
    }}
    virtual ~{context.name}()
    {{
    }}
   
protected:
    
{Generator._generate_request_handler_declarations(context.component.requests)}
{Generator._generate_trans_actions_declarations(context.actions, context.name)}

private:
    const ServiceLocator& m_serviceLocator;
}};

}} // namespace sugo
''')

    @staticmethod
    def _generate_request_handler_declarations(requests):
        out_str = '    // Request handlers\n'
        for request in requests:
            request_name = request.replace('.', '')
            out_str += f'    message_broker::MessageResponse onRequest{request_name}(const message_broker::Message& request) override;\n'
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
///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: AUTOMATIC GENERATED FILE!
 * @data: {datetime.now().strftime('%Y-%m-%d')}
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "{context.name}.hpp"

using namespace sugo;
using namespace sugo::service_component;

///////////////////////////////////////////////////////////////////////////////
// Requests:
{Generator._generate_request_handler_definitions(context)}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:
{Generator._generate_action_method_definitions(context)}
''')
      
    @staticmethod
    def _generate_request_handler_definitions(context):
        out_str = ''
        for request in context.component.requests:
            request_name = request.replace('.', '')
            out_str += f'''
message_broker::MessageResponse {context.name}::onRequest{request_name}(const message_broker::Message& request)
{{
    LOG(debug) << "Request {request} received in {context.name}...";
    return createUnsupportedRequestResponse(request);
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
    LOG(debug) << "Transition action {action} in {context.name}...";
}}
'''
        return out_str
