'''
Contains the C++ generator for source and header files
'''

__author__      = "denis@schoener-one.de"
__copyright__   = "Copyright 2022, Schoener-One"

from dataclasses import dataclass
from pathlib import Path
import logging
import re

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
            self._generate_header_files(context)
            self._generate_source_files(context)
    
    def _generate_header_files(self, context):
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
namespace {context.namespace}
{{
/// States of the component interface I{context.name}
enum State
{{
{newline.join(f'    {state},' for state in context.component.statemachine.states)}
}};

/// Event identifiers of the component interface I{context.name}
enum EventId
{{
{newline.join(f'    {event},' for event in context.component.events)}
}};

using Event        = Ident<EventId>;
using StateMachine = StateMachine<State, Event>;
}} // namespace {context.namespace}

std::ostream& operator<<(std::ostream& ostr, {context.namespace}::State const& value);
std::ostream& operator<<(std::ostream& ostr, {context.namespace}::EventId const& value);

/**
 * @brief Interface class
 */
class I{context.name}
    : public {context.namespace}::StateMachine,
      public StatedServiceComponent<{context.namespace}::State, {context.namespace}::Event>
{{
public:
    using State        = {context.namespace}::State;
    using StateMachine = {context.namespace}::StateMachine;
    using Event        = {context.namespace}::Event;
    using EventId      = {context.namespace}::EventId;

    inline static const std::string ReceiverId{{"{context.name}"}};
    
    // Commands
    inline static const CommandId CommandGetState{{ReceiverId, "GetState"}};
{Generator._generate_commands(context.component.commands) if len(context.component.commands) else ''}
    // Notifications
{Generator._generate_notifications(context.component.notifications) if len(context.component.notifications) else ''}    
    // Constructor / Destructor
    explicit I{context.name}(ICommandMessageBroker& messageBroker);
    virtual ~I{context.name}(){{}}

    I{context.name}(const I{context.name}&) = default;
    I{context.name}& operator=(const I{context.name}&) = default;
    I{context.name}(I{context.name}&&) = default;
    I{context.name}& operator=(I{context.name}&&) = default;
    
protected:

    // Command handlers
    message::CommandResponse onCommandGetState(const message::Command& command);
{Generator._generate_command_handler_declarations(context.component.commands)}
    // Transition actions
{Generator._generate_trans_actions_declarations(context.actions, context.name)}
}};

}} // namespace sugo
''')

    @staticmethod
    def _generate_commands(commands):
        out_str = ''
        for command in commands:
            command_name = command.replace('.', '')
            out_str += f'    inline static const CommandId Command{command_name}{{ReceiverId, "{command}"}};\n'
        return out_str

    @staticmethod
    def _generate_notifications(notifications):
        out_str = ''
        for notification in notifications:
            notif_name = notification.replace('.', '')
            out_str += f'    inline static const CommandId Notification{notif_name}{{ReceiverId, "{notif_name}"}};\n'
        return out_str

    @staticmethod
    def _generate_command_handler_declarations(commands):
        out_str = ''
        for command in commands:
            command_name = command.replace('.', '')
            out_str += f'    virtual message::CommandResponse onCommand{command_name}(const message::Command& command) = 0;\n'
        return out_str

    @staticmethod
    def _generate_trans_actions_declarations(actions, component_name):
        out_str = ''
        for action in actions:
            out_str += f'    virtual void {action}(const Event& event, const State& state) = 0;\n'
        return out_str

    def _generate_source_files(self, context):
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
std::ostream& sugo::operator<<(std::ostream& ostr, {context.namespace}::State const& value)
{{
    switch (value)
    {{
{newline.join(f'        case {context.namespace}::State::{state}:{newline}        ostr << "{state}";{newline}        break;' for state in context.component.statemachine.states)}
        default:
            ASSERT_NOT_REACHABLE;
    }}
    return ostr;
}}

std::ostream& sugo::operator<<(std::ostream& ostr, {context.namespace}::EventId const& value)
{{
    switch (value)
    {{
{newline.join(f'        case {context.namespace}::EventId::{event}:{newline}        ostr << "{event}";{newline}        break;' for event in context.component.events)}
        default:
            ASSERT_NOT_REACHABLE;
    }}
    return ostr;
}}

namespace
{{
const ICommandMessageBroker::ReceiverIdList NotificationReceivers
{{
{Generator._generate_receiver_list(context)}
}};    
}} // namespace

I{context.name}::I{context.name}(ICommandMessageBroker& messageBroker)
    : StateMachine(State::{context.component.statemachine.start},
          {{
              // clang-format off
            {Generator._generate_transitions(context)}
              // clang-format on
          }}),
      StatedServiceComponent<I{context.name}::State, I{context.name}::Event>(messageBroker, NotificationReceivers, *this)
{{
    registerCommandHandler(CommandGetState, std::bind(&I{context.name}::onCommandGetState, this, std::placeholders::_1));
{Generator._generate_command_register_calls(context)}
}}

message::CommandResponse I{context.name}::onCommandGetState(const message::Command& command)
{{
    return handleCommandGetState(command);
}}
''')

    @staticmethod
    def _generate_receiver_list(context):
        out_str = str()
        for receiver in context.receivers:
            out_str += f'    "{receiver}",\n'
        return out_str
        
    @staticmethod
    def _generate_transitions(context):
        out_str = ''
        for trans in context.component.statemachine.transitions:
            make_action = f', StateMachine::MakeAction(&I{context.name}::{trans.action}, this)' if trans.action else ""
            out_str += f'''Transition(State::{trans.state}, State::{trans.next}, Event(EventId::{trans.event}){make_action}),
            '''
        return out_str
    
    @staticmethod
    def _generate_command_register_calls(context):
        out_str = ''
        for command in context.component.commands:
            command_name = command.replace('.', '')
            out_str += f'''    registerCommandHandler(Command{command_name}, std::bind(&I{context.name}::onCommand{command_name}, this, std::placeholders::_1));
'''
        return out_str
