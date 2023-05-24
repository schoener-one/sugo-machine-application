'''
Contains the C++ generator for header files
'''

__author__ = "denis@schoener-one.de"
__copyright__ = "Copyright 2022, Schoener-One"

from .GeneratorContext import GeneratorContext
from pathlib import Path

class ServiceComponentHeaderGenerator:
    """Class to generate the C++ header files"""

    def __init__(self, context: GeneratorContext) -> None:
        self.context = context

    def generate(self, path):
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

namespace sugo
{{
namespace {self.context.namespace}
{{
/// States of the component interface I{self.context.name}
enum State
{{
{newline.join(f'    {state},' for state in self.context.component.statemachine.states)}
}};

/// Event identifiers of the component interface I{self.context.name}
enum Event
{{
{newline.join(f'    {event},' for event in self.context.component.events)}
}};

using StateMachine = StateMachine<State, Event>;
}} // namespace {self.context.namespace}

std::ostream& operator<<(std::ostream& ostr, {self.context.namespace}::State const& value);
std::ostream& operator<<(std::ostream& ostr, {self.context.namespace}::Event const& value);

/**
 * @brief Interface class
 */
class I{self.context.name}
    : public {self.context.namespace}::StateMachine,
      public StatedServiceComponent<{self.context.namespace}::State, {self.context.namespace}::Event>
{{
public:
    using State        = {self.context.namespace}::State;
    using StateMachine = {self.context.namespace}::StateMachine;
    using Event        = {self.context.namespace}::Event;

    inline static const message::Address ReceiverId{{"{self.context.name}"}};
    
    // Commands
    inline static const message::CommandId CommandGetState{{"GetState", ReceiverId}};
{ServiceComponentHeaderGenerator._generate_commands(self.context.component.inbound.commands) if len(self.context.component.inbound.commands) else ''}
    // Notifications
{ServiceComponentHeaderGenerator._generate_notifications(self.context.name, self.context.component.outbound.notifications) if len(self.context.component.outbound.notifications) else ''}    
    // Constructor / Destructor
    explicit I{self.context.name}(message::ICommandMessageBroker& messageBroker);
    ~I{self.context.name}() = default;

    I{self.context.name}(const I{self.context.name}&) = default;
    I{self.context.name}& operator=(const I{self.context.name}&) = default;
    I{self.context.name}(I{self.context.name}&&) noexcept = default;
    I{self.context.name}& operator=(I{self.context.name}&&) noexcept = default;
    
protected:
    // Message handlers
    message::CommandResponse onCommandGetState(const message::Command& command);
{ServiceComponentHeaderGenerator._generate_message_handler_declarations('Command', self.context.component.inbound.commands)}
{ServiceComponentHeaderGenerator._generate_message_handler_declarations('Notification', self.context.component.inbound.notifications)}
    // Transition actions
{ServiceComponentHeaderGenerator._generate_trans_actions_declarations(self.context.actions, self.context.name)}
}};

}} // namespace sugo
''')

    @staticmethod
    def _generate_commands(commands):
        out_str = ''
        for command in commands:
            command_name = command.name.replace('.', '')
            out_str += f'    inline static const message::CommandId Command{command_name}{{"{command}", ReceiverId}};\n'
        return out_str

    @staticmethod
    def _generate_notifications(component_name, notifications):
        out_str = ''
        for notification in notifications:
            receiver_list = '"' + \
                '","'.join(notification.receivers) + \
                '"' if notification.receivers else ""
            out_str += f'    inline static const message::NotificationId Notification{notification.name}{{"{component_name}.{notification.name}", ReceiverId}};\n'
        return out_str

    @staticmethod
    def _generate_message_handler_declarations(prefix, messages):
        out_str = ''
        for message in messages:
            message_name = message.name.replace('.', '')
            postfix = " = 0" if not message.event and not message.forward else ""
            out_str += f'    virtual message::CommandResponse on{prefix}{message_name}(const message::Command& command){postfix};\n'
        return out_str

    @staticmethod
    def _generate_trans_actions_declarations(actions, component_name):
        out_str = ''
        for action in actions:
            out_str += f'    virtual void {action}(const Event& event, const State& state) = 0;\n'
        return out_str
