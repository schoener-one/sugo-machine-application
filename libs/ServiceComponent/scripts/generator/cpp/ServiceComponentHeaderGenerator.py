'''
Contains the C++ generator for header files

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

#include <string>
#include <ostream>

#include "Common/IProcessContext.hpp"
#include "MessageBroker/Message.hpp"
#include "ServiceComponent/StatedServiceComponent.hpp"
#include "ServiceComponent/StateMachine.hpp"

namespace sugo::service_component
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

using StateMachine = service_component::StateMachine<State, Event>;
}} // namespace {self.context.namespace}

std::ostream& operator<<(std::ostream& ostr, {self.context.namespace}::State const& value);
std::ostream& operator<<(std::ostream& ostr, {self.context.namespace}::Event const& value);

/**
 * @brief Interface class
 */
class I{self.context.name}
    : public {self.context.namespace}::StateMachine, //TODO change state machine to member object!
      public StatedServiceComponent<{self.context.namespace}::State, {self.context.namespace}::Event>
{{
public:
    using State        = {self.context.namespace}::State;
    using StateMachine = {self.context.namespace}::StateMachine;
    using Event        = {self.context.namespace}::Event;

    static constexpr IServiceComponent::Identifier Identifier{{"{self.context.name}"}};
    
    // Requests
    static constexpr RequestId RequestGetState{{Identifier, "GetState"}};
{ServiceComponentHeaderGenerator._generate_requests(self.context.component.inbound.requests) if len(self.context.component.inbound.requests) else ''}
{ServiceComponentHeaderGenerator._generate_notifications(self.context.name, self.context.component.outbound.notifications) if len(self.context.component.outbound.notifications) else ''}    
    // Constructor / Destructor
    explicit I{self.context.name}(message_broker::IMessageBroker& messageBroker, common::IProcessContext& processContext);
    ~I{self.context.name}() override = default;

    I{self.context.name}(const I{self.context.name}&) = default;
    I{self.context.name}& operator=(const I{self.context.name}&) = default;
    I{self.context.name}(I{self.context.name}&&) noexcept = default;
    I{self.context.name}& operator=(I{self.context.name}&&) noexcept = default;
    
protected:
    // Message handlers
    message_broker::ResponseMessage onRequestGetState(const message_broker::Message& request);

{ServiceComponentHeaderGenerator._generate_request_handler_declarations(self.context.component.inbound.requests)}
{ServiceComponentHeaderGenerator._generate_notification_handler_declarations(self.context.component.inbound.notifications)}
    // Transition actions
{ServiceComponentHeaderGenerator._generate_trans_actions_declarations(self.context.actions, self.context.name)}
}};

}} // namespace sugo
''')

    @staticmethod
    def _generate_requests(requests):
        out_str = ''
        for request in requests:
            request_name = request.name.replace('.', '')
            out_str += f'    static constexpr RequestId Request{request_name}{{Identifier, "{request}"}};\n'
        return out_str

    @staticmethod
    def _generate_notifications(component_name, notifications):
        out_str = '    // Notifications\n' if len(notifications) > 0 else ''
        for notification in notifications:
            receiver_list = '"' + \
                '","'.join(notification.receivers) + \
                '"' if notification.receivers else ""
            out_str += f'    static constexpr NotificationId Notification{notification.name}{{Identifier, "{notification.name}"}};\n'
        return out_str

    @staticmethod
    def _generate_request_handler_declarations(requests):
        out_str = f'    // Request handlers\n' if len(requests) > 0 else ''
        for request in requests:
            message_name = request.name.replace('.', '')
            postfix = " = 0" if not request.event and not request.forward else ""
            out_str += f'    virtual message_broker::ResponseMessage onRequest{message_name}(const message_broker::Message& request){postfix};\n'
        return out_str

    @staticmethod
    def _generate_notification_handler_declarations(notifications):
        out_str = f'    // Notification handlers\n' if len(notifications) > 0 else ''
        for notification in notifications:
            message_name = notification.name.replace('.', '')
            postfix = " = 0" if not notification.event and not notification.forward else ""
            out_str += f'    virtual void onNotification{message_name}(const message_broker::Message& request){postfix};\n'
        return out_str

    @staticmethod
    def _generate_trans_actions_declarations(actions, component_name):
        out_str = ''
        for action in actions:
            out_str += f'    virtual void {action}(const Event& event, const State& state) = 0;\n'
        return out_str
