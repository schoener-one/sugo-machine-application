'''
Contains the C++ generator for source files.


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

__author__ = "denis@schoener-one.de"
__copyright__ = "Copyright (C) 2020 by Denis Schoener"

from datetime import datetime
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

#include "ServiceComponent/I{self.context.name}.hpp"
{ServiceComponentSourceGenerator._generate_includes(self.context.component.inbound.notifications)}
namespace {{
    const sugo::service_component::IServiceComponent::NotificationIdList SubscriptionIds{{
{ServiceComponentSourceGenerator._generate_subscription_id_list(self.context.name, self.context.component.inbound.notifications)}
    }};
}} // namespace

using namespace sugo;
using namespace sugo::service_component;
using Transition = I{self.context.name}::StateMachine::Transition;

std::ostream& sugo::service_component::operator<<(std::ostream& ostr, {self.context.namespace}::State const& value)
{{
    switch (value)
    {{
{newline.join(f'        case {self.context.namespace}::State::{state}:{newline}            ostr << "{state}";{newline}            break;' for state in self.context.component.statemachine.states)}
        default:
            ASSERT_NOT_REACHABLE;
    }}
    return ostr;
}}

std::ostream& sugo::service_component::operator<<(std::ostream& ostr, {self.context.namespace}::Event const& value)
{{
    switch (value)
    {{
{newline.join(f'        case {self.context.namespace}::Event::{event}:{newline}            ostr << "{event}";{newline}            break;' for event in self.context.component.events)}
        default:
            ASSERT_NOT_REACHABLE;
    }}
    return ostr;
}}

I{self.context.name}::I{self.context.name}(message_broker::IMessageBroker& messageBroker, common::IProcessContext& processContext)
    : StateMachine(State::{self.context.component.statemachine.start},
          {{
              // clang-format off
            {self._generate_transitions()}
              // clang-format on
          }}),
      StatedServiceComponent<I{self.context.name}::State, I{self.context.name}::Event>(messageBroker, SubscriptionIds, *this, processContext)
{{
    getMessageBroker().registerRequestMessageHandler(RequestGetState, [this](const message_broker::Message& request) {{ return this->onRequestGetState(request); }});
{ServiceComponentSourceGenerator._generate_request_handler_registrations(self.context.name, self.context.component.inbound.requests)}
{ServiceComponentSourceGenerator._generate_notification_handler_registrations(self.context.name, self.context.component.inbound.notifications)}
}}

message_broker::ResponseMessage I{self.context.name}::onRequestGetState(const message_broker::Message& request)
{{
    return handleStateRequest(request);
}}


///////////////////////////////////////////////////////////////////////////////
// Requests:
{self._generate_request_handler_definitions(self.context.component.inbound.requests)}

///////////////////////////////////////////////////////////////////////////////
// Notifications:
{self._generate_notification_handler_definitions(self.context.component.inbound.notifications)}
''')

    @staticmethod
    def _generate_includes(notifications):
        senders = set()
        for notification in notifications:
            senders.add(notification.name.split('.')[0])
        out_str = ''
        for sender in senders:
            out_str += f'''#include "ServiceComponent/I{sender}.hpp"
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
    def _generate_request_handler_registrations(context_name, requests):
        out_str = ''
        for request in requests:
            request_name = request.name.replace('.', '')
            out_str += f'''    getMessageBroker().registerRequestMessageHandler(Request{request_name}, [this](const message_broker::Message& request) {{ return this->onRequest{request_name}(request); }});
'''
        return out_str

    @staticmethod
    def _generate_notification_handler_registrations(context_name, notifications):
        out_str = ''
        for notification in notifications:
            component_name, notification_name = notification.name.split('.')
            out_str += f'''    getMessageBroker().registerNotificationMessageHandler(I{component_name}::Notification{notification_name}, [this](const message_broker::Message& notification) {{ return this->onNotification{component_name}{notification_name}(notification); }});
'''
        return out_str

    @staticmethod
    def _generate_subscription_id_list(context_name, notifications):
        out_str = ''
        for notification in notifications:
            component_name, notification_name = notification.name.split('.')
            out_str += f'''        sugo::service_component::I{component_name}::Notification{notification_name},
'''
        return out_str

    def _generate_request_handler_definitions(self, messages) -> str:
        out_str = ''
        for message in messages:
            message_name = message.name.replace('.', '')
            if message.event:
                event_name = message.event.replace('.', '') if message.event else ''
                out_str += f'''
message_broker::ResponseMessage I{self.context.name}::onRequest{message_name}(const message_broker::Message& request)
{{
    return handleEventMessage(request, Event::{event_name});
}}
'''
            if message.forward:
                forward_message = f"I{message.forward.replace('.', '::')}"
                out_str += f'''
message_broker::ResponseMessage I{self.context.name}::onRequest{message_name}(const message_broker::Message& request)
{{
    return forward({forward_message}, request);
}}
'''
        return out_str

    def _generate_notification_handler_definitions(self, messages) -> str:
        out_str = ''
        for message in messages:
            message_name = message.name.replace('.', '')
            if message.event:
                event_name = message.event.replace('.', '') if message.event else ''
                out_str += f'''
void I{self.context.name}::onNotification{message_name}(const message_broker::Message& request)
{{
    handleEventMessage(request, Event::{event_name});
}}
'''
            if message.forward:
                raise Exception(f"forwarding not supported for notifications: {str(message.event)}")
        return out_str
