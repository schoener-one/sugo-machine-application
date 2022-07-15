'''
Contains the C++ generator for source and header files
'''

__author__      = "denis@schoener-one.de"
__copyright__   = "Copyright 2022, Schoener-One"

from dataclasses import dataclass
from pathlib import Path
import logging

from ServiceComponentData import ServiceComponent

@dataclass
class GeneratorContext:
    name: str
    component: ServiceComponent
    actions: set
    
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
            context = GeneratorContext(name, component, set([trans.action for trans in component.statemachine.transitions if trans.action]))
            logging.info(f"generating component I{context.name} files")
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
 * @author: AUTOMATIC GENERATED FILE!
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <ostream>

#include "ICommandMessageBroker.hpp"
#include "StatedServiceComponent.hpp"
#include "StateMachine.hpp"
#include "Ident.hpp"

namespace sugo
{{
/// States of the component interface I{context.name}
enum I{context.name}State
{{
{newline.join(f'    {state},' for state in context.component.statemachine.states)}
}};

/// Event identifiers of the component interface I{context.name}
enum I{context.name}EventId
{{
{newline.join(f'    {event},' for event in context.component.events)}
}};

using I{context.name}Event        = Ident<I{context.name}EventId>;
using I{context.name}StateMachine = StateMachine<I{context.name}State, I{context.name}Event>;

std::ostream& operator<<(std::ostream& ostr, I{context.name}State const& value);
std::ostream& operator<<(std::ostream& ostr, I{context.name}EventId const& value);

/**
 * @brief Interface class
 */
class I{context.name}
    : public I{context.name}StateMachine,
      public StatedServiceComponent<I{context.name}State, I{context.name}Event>
{{
public:
    using State = I{context.name}State;
    using StateMachine = I{context.name}StateMachine;
    using Event = I{context.name}Event;
    using EventId = I{context.name}EventId;

    inline static const std::string ReceiverId{{"{context.name}"}};
    
{Generator._generate_commands(context.component.commands) if len(context.component.commands) else ''}
{Generator._generate_notifications(context.component.notifications) if len(context.component.notifications) else ''}
    
    // Constructor / Destructor
    explicit I{context.name}(ICommandMessageBroker& messageBroker);
    virtual ~I{context.name}(){{}}

    I{context.name}(const I{context.name}&) = default;
    I{context.name}& operator=(const I{context.name}&) = default;
    I{context.name}(I{context.name}&&) = default;
    I{context.name}& operator=(I{context.name}&&) = default;
    
protected:
    
{Generator._generate_command_handler_declarations(context.component.commands)}
{Generator._generate_trans_actions_declarations(context.actions, context.name)}
}};

}}  // namespace sugo
                   ''')
    @staticmethod
    def _generate_commands(commands):
        out_str = '    // Commands\n'
        for command in commands:
            command_name = command.replace('.', '')
            out_str += f'    inline static const CommandId Command{command_name}{{ReceiverId, "{command_name}"}};\n'
        return out_str

    @staticmethod
    def _generate_notifications(notifications):
        out_str = '    // Notifications\n'
        for notification in notifications:
            notif_name = notification.replace('.', '')
            out_str += f'    inline static const CommandId Command{notif_name}{{ReceiverId, "{notif_name}"}};\n'
        return out_str

    @staticmethod
    def _generate_command_handler_declarations(commands):
        out_str = '    // Command handlers\n'
        for command in commands:
            command_name = command.replace('.', '')
            out_str += f'    virtual message::CommandResponse onCommand{command_name}(const message::Command& command);\n'
        return out_str

    @staticmethod
    def _generate_trans_actions_declarations(actions, component_name):
        out_str = '    // Transition actions\n'
        for action in actions:
            out_str += f'    virtual void {action}(const I{component_name}::Event& event, const I{component_name}::State& state);\n'
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
 * @author: AUTOMATIC GENERATED FILE!
 */
///////////////////////////////////////////////////////////////////////////////

#include "I{context.name}.hpp"

using namespace sugo;
using Transition = I{context.name}::StateMachine::Transition;

// cppcheck-suppress unusedFunction
std::ostream& sugo::operator<<(std::ostream& ostr, I{context.name}State const& value)
{{
    switch (value)
    {{
{newline.join(f'        case I{context.name}::State::{state}:{newline}        ostr << "{state}";{newline}        break;' for state in context.component.statemachine.states)}
        default:
            ASSERT_NOT_REACHABLE;
    }}
    return ostr;
}}

std::ostream& sugo::operator<<(std::ostream& ostr, I{context.name}EventId const& value)
{{
    switch (value)
    {{
{newline.join(f'        case I{context.name}::EventId::{event}:{newline}        ostr << "{event}";{newline}        break;' for event in context.component.events)}
        default:
            ASSERT_NOT_REACHABLE;
    }}
    return ostr;
}}

I{context.name}::I{context.name}(ICommandMessageBroker& messageBroker)
    : StateMachine({context.component.statemachine.start},
          {{
              // clang-format off
            {Generator._create_transitions(context)}
              // clang-format on
          }}),
      StatedServiceComponent<I{context.name}::State, I{context.name}::Event>(messageBroker, *this)
{{
    {Generator._create_command_register_calls(context)}
}}

///////////////////////////////////////////////////////////////////////////////
// Commands:
{Generator._create_command_methods(context)}

///////////////////////////////////////////////////////////////////////////////
// Transition actions:
{Generator._create_action_methods(context)}
                        ''')

    @staticmethod
    def _create_transitions(context):
        out_str = ''
        for trans in context.component.statemachine.transitions:
            make_action = f', StateMachine::MakeAction(&I{context.name}::{trans.action}, this)' if trans.action else ""
            out_str += f'''Transition({trans.state}, {trans.next}, Event({trans.event}){make_action}),
            '''
        return out_str
    
    @staticmethod
    def _create_command_register_calls(context):
        out_str = ''
        for command in context.component.commands:
            out_str += f'''registerCommandHandler(Command{command}, std::bind(&I{context.name}::onCommand{command}, this, std::placeholders::_1));
    '''
        return out_str
    
    @staticmethod
    def _create_command_methods(context):
        out_str = ''
        for command in context.component.commands:
            out_str += f'''
message::CommandResponse I{context.name}::onCommand{command}(const message::Command&)
{{
    LOG(debug) << "command {command} received in I{context.name}...";
    return message::CommandResponse();
}}
            '''
        return out_str
    
    @staticmethod
    def _create_action_methods(context):
        out_str = ''
        for action in context.actions:
            out_str += f'''
void I{context.name}::{action}(const I{context.name}::Event&, const I{context.name}::State&)
{{
    LOG(debug) << "transition action {action} in I{context.name}...";
}}
            '''
        return out_str
