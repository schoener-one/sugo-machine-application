"""
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
"""

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
        newline = "\n"
        with path.open("w", encoding="utf-8") as file:
            file.write(f"""
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
#include <limits>

#include "Common/IProcessContext.hpp"
#include "MessageBroker/Message.hpp"
#include "ServiceComponent/PrimitiveTypeProperty.hpp"
#include "ServiceComponent/ArrayTypeProperty.hpp"
#include "ServiceComponent/StateMachineStateProperty.hpp"
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
    
{ServiceComponentHeaderGenerator._generate_command_request_ids(self.context.component.interface.commands) if len(self.context.component.interface.commands) else ''}
    // Property request ids
    static constexpr RequestId PropertyRequestGetState{{Identifier, "GetState"}};
{ServiceComponentHeaderGenerator._generate_property_request_ids(self.context.component.interface.properties) if len(self.context.component.interface.properties) else ''}
{ServiceComponentHeaderGenerator._generate_notification_ids(self.context.name, self.context.component.interface.notifications) if len(self.context.component.interface.notifications) else ''}    

    // Constructor / Destructor
    explicit I{self.context.name}(message_broker::IMessageBroker& messageBroker, common::IProcessContext& processContext);
    ~I{self.context.name}() override = default;

    I{self.context.name}(const I{self.context.name}&) = default;
    I{self.context.name}& operator=(const I{self.context.name}&) = default;
    I{self.context.name}(I{self.context.name}&&) noexcept = default;
    I{self.context.name}& operator=(I{self.context.name}&&) noexcept = default;
    
protected:
    // Properties
    StateMachineStateProperty<{self.context.namespace}::StateMachine> m_propertyStateMachineState{{*this}};
{ServiceComponentHeaderGenerator._generate_property_declarations(self.context.component.interface.properties)}
{ServiceComponentHeaderGenerator._generate_command_request_handler_declarations(self.context.component.interface.commands)}
{ServiceComponentHeaderGenerator._generate_notification_handler_declarations(self.context.component.utilization.notifications)}

    // Property request handlers
    virtual message_broker::ResponseMessage onPropertyRequestGetState(const message_broker::Message& request);
{ServiceComponentHeaderGenerator._generate_property_request_handler_declarations(self.context.component.interface.properties)}
{ServiceComponentHeaderGenerator._generate_trans_actions_declarations(self.context.actions, self.context.name)}
}};

}} // namespace sugo::service_component

namespace std
{{
ostream& operator<<(std::ostream& ostr, sugo::service_component::{self.context.namespace}::State const& value);
ostream& operator<<(std::ostream& ostr, sugo::service_component::{self.context.namespace}::Event const& value);
}} // namespace std
""")

    @staticmethod
    def _generate_command_request_ids(commands):
        out_str = "\n    // Command request ids" if len(commands) > 0 else ""
        for command in commands:
            command_name = command.get_normalized_name()
            out_str += f'\n    static constexpr RequestId CommandRequest{command_name}{{Identifier, "{command_name}"}};'
        return out_str

    @staticmethod
    def _generate_property_request_ids(properties):
        out_str = ""
        for property in properties:
            for property_name in property.get_all_normalized_names():
                out_str += f'\n    static constexpr RequestId PropertyRequest{property_name}{{Identifier, "{property_name}"}};'
        return out_str

    @staticmethod
    def _generate_notification_ids(component_name, notifications):
        out_str = "\n    // Notification ids" if len(notifications) > 0 else ""
        for notification in notifications:
            out_str += f'\n    static constexpr NotificationId Notification{notification.name}{{Identifier, "{notification.name}"}};'
        return out_str

    @staticmethod
    def _generate_command_request_handler_declarations(requests):
        out_str = f"\n    // Command request handlers" if len(requests) > 0 else ""
        for request in requests:
            message_name = request.get_normalized_name()
            postfix = " = 0" if not request.event and not request.forward else ""
            out_str += f"\n    virtual message_broker::ResponseMessage onCommandRequest{message_name}(const message_broker::Message& request){postfix};"
        return out_str

    @staticmethod
    def _generate_property_request_handler_declarations(properties):
        out_str = ""
        for property in properties:
            for property_name in property.get_all_normalized_names():
                out_str += f"\n    virtual message_broker::ResponseMessage onPropertyRequest{property_name}(const message_broker::Message& request);"
        return out_str

    @staticmethod
    def _generate_property_declarations(propterties):
        out_str = ""
        for property in propterties:
            value = property.get_cpp_value()
            if property.type.is_array():
                out_str += f'\n    ArrayTypeProperty<{property.type.type.value}, {property.type.cardinality}> m_property{property.name}{{"{property.name}", {{{value}}}}};'
            else:
                #TODO set default values in Property template declaration and keep it free here as default!
                min = property.min if property.min else f"std::numeric_limits<{property.type}>::min()"
                max = property.max if property.max else f"std::numeric_limits<{property.type}>::max()"
                out_str += f'\n    PrimitiveTypeProperty<{property.type}, {min}, {max}> m_property{property.name}{{"{property.name}", {value}}};'
        return out_str

    @staticmethod
    def _generate_notification_handler_declarations(notifications):
        out_str = f"\n    // Notification handlers" if len(notifications) > 0 else ""
        actions = set()
        for notification in notifications:
            message_name = notification.get_normalized_name()
            postfix = (
                " = 0" if not notification.event and not notification.action and not notification.change else ""
            )
            out_str += f"\n    virtual void onNotification{message_name}(const message_broker::Message& notification){postfix};"
            if notification.action:
                actions.add(notification.action)
        # Add action handlers here
        out_str += f"\n\n    // Notification handler actions" if len(actions) > 0 else ""
        for action in actions:
            out_str += f"\n    virtual void {action}() = 0;"
        return out_str

    @staticmethod
    def _generate_trans_actions_declarations(actions, component_name):
        out_str = "\n    // Transition actions" if len(actions) > 0 else ""
        for action in actions:
            out_str += f"\n    virtual void {action}(const Event& event, const State& state) = 0;"
        return out_str
