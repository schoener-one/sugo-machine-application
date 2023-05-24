'''
Contains the C++ generator for source files
'''

__author__ = "denis@schoener-one.de"
__copyright__ = "Copyright 2022, Schoener-One"

from .GeneratorContext import GeneratorContext
from ServiceComponentData import Message
from pathlib import Path


class ServiceComponentSourceGenerator:
    """Class to generate the C++ source files"""

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

#include "I{self.context.name}.hpp"
{ServiceComponentSourceGenerator._generate_includes(self.context.component.inbound.notifications)}
namespace {{
    const sugo::IServiceComponent::NotificationIdList SubscriptionIds{{
{ServiceComponentSourceGenerator._generate_subscription_id_list(self.context.name, self.context.component.inbound.notifications)}
    }};
}} // namespace

using namespace sugo;
using Transition = I{self.context.name}::StateMachine::Transition;

std::ostream& sugo::operator<<(std::ostream& ostr, {self.context.namespace}::State const& value)
{{
    switch (value)
    {{
{newline.join(f'        case {self.context.namespace}::State::{state}:{newline}            ostr << "{state}";{newline}            break;' for state in self.context.component.statemachine.states)}
        default:
            ASSERT_NOT_REACHABLE;
    }}
    return ostr;
}}

std::ostream& sugo::operator<<(std::ostream& ostr, {self.context.namespace}::Event const& value)
{{
    switch (value)
    {{
{newline.join(f'        case {self.context.namespace}::Event::{event}:{newline}            ostr << "{event}";{newline}            break;' for event in self.context.component.events)}
        default:
            ASSERT_NOT_REACHABLE;
    }}
    return ostr;
}}

I{self.context.name}::I{self.context.name}(message::ICommandMessageBroker& messageBroker)
    : StateMachine(State::{self.context.component.statemachine.start},
          {{
              // clang-format off
            {self._generate_transitions()}
              // clang-format on
          }}),
      StatedServiceComponent<I{self.context.name}::State, I{self.context.name}::Event>(messageBroker, SubscriptionIds, *this)
{{
    registerMessageHandler(CommandGetState, [this](const message::Command& command) {{ return this->onCommandGetState(command); }});
{ServiceComponentSourceGenerator._generate_command_handler_registrations(self.context.name, self.context.component.inbound.commands)}
{ServiceComponentSourceGenerator._generate_notification_handler_registrations(self.context.name, self.context.component.inbound.notifications)}
}}

message::CommandResponse I{self.context.name}::onCommandGetState(const message::Command& command)
{{
    return handleCommandGetState(command);
}}


///////////////////////////////////////////////////////////////////////////////
// Commands:
{self._generate_message_handler_definitions('Command', self.context.component.inbound.commands)}

///////////////////////////////////////////////////////////////////////////////
// Notifications:
{self._generate_message_handler_definitions('Notification', self.context.component.inbound.notifications)}
''')

    @staticmethod
    def _generate_includes(notifications):
        senders = set()
        for notification in notifications:
            senders.add(notification.name.split('.')[0])
        out_str = ''
        for sender in senders:
            out_str += f'''#include "I{sender}.hpp"
'''
        return out_str

    def _generate_transitions(self):
        out_str = ''
        for trans in self.context.component.statemachine.transitions:
            make_action = f', StateMachine::MakeAction(&I{self.context.name}::{trans.action}, this)' if trans.action else ""
            out_str += f'''Transition(State::{trans.state}, State::{trans.next}, Event::{trans.event}{make_action}),
            '''
        return out_str

    @staticmethod
    def _generate_command_handler_registrations(context_name, commands):
        out_str = ''
        for command in commands:
            command_name = command.name.replace('.', '')
            out_str += f'''    registerMessageHandler(Command{command_name}, [this](const message::Command& command) {{ return this->onCommand{command_name}(command); }});
'''
        return out_str

    @staticmethod
    def _generate_notification_handler_registrations(context_name, notifications):
        out_str = ''
        for notification in notifications:
            component_name, notification_name = notification.name.split('.')
            out_str += f'''    registerMessageHandler(I{component_name}::Notification{notification_name}, [this](const message::Command& command) {{ return this->onNotification{component_name}{notification_name}(command); }});
'''
        return out_str

    @staticmethod
    def _generate_subscription_id_list(context_name, notifications):
        out_str = ''
        for notification in notifications:
            component_name, notification_name = notification.name.split('.')
            out_str += f'''        sugo::I{component_name}::Notification{notification_name},
'''
        return out_str

    def _generate_message_handler_definitions(self, prefix, messages) -> str:
        out_str = ''
        for message in messages:
            message_name = message.name.replace('.', '')
            if message.event:
                event_name = message.event.replace('.', '') if message.event else ''
                out_str += f'''
message::CommandResponse I{self.context.name}::on{prefix}{message_name}(const message::Command& command)
{{
    return handleEventMessage(command, Event::{event_name});
}}
'''
            if message.forward:
                forward_message = f"I{message.forward.replace('.', '::')}"
                out_str += f'''
message::CommandResponse I{self.context.name}::on{prefix}{message_name}(const message::Command& command)
{{
    return forward({forward_message}, command);
}}
'''
        return out_str
