"""
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
"""

__author__ = "denis@schoener-one.de"
__copyright__ = "Copyright (C) 2020 by Denis Schoener"

from datetime import datetime
from .GeneratorContext import GeneratorContext
from ServiceComponentData import Command, Property, Notification, SubscribedNotification
from pathlib import Path


class ServiceComponentSourceGenerator:
    """Class to generate the C++ source files"""

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

#include "ServiceComponent/I{self.context.name}.hpp"
{ServiceComponentSourceGenerator._generate_includes(self.context.component.utilization.notifications)}
namespace {{
    const sugo::service_component::IServiceComponent::NotificationIdList SubscriptionIds{{
{ServiceComponentSourceGenerator._generate_subscription_id_list(self.context.name, self.context.component.utilization.notifications)}
    }};
}} // namespace

using namespace sugo;
using namespace sugo::service_component;
using Transition = I{self.context.name}::StateMachine::Transition;

std::ostream& std::operator<<(std::ostream& ostr, sugo::service_component::{self.context.namespace}::State const& value)
{{
    switch (value)
    {{
{newline.join(f'        case sugo::service_component::{self.context.namespace}::State::{state}:{newline}            ostr << "{state}";{newline}            break;' for state in self.context.component.statemachine.states)}
        default:
            ASSERT_NOT_REACHABLE;
    }}
    return ostr;
}}

std::ostream& std::operator<<(std::ostream& ostr, sugo::service_component::{self.context.namespace}::Event const& value)
{{
    switch (value)
    {{
{newline.join(f'        case sugo::service_component::{self.context.namespace}::Event::{event}:{newline}            ostr << "{event}";{newline}            break;' for event in self.context.component.events)}
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
    messageBroker.registerRequestMessageHandler([this](const message_broker::Message& request)
    {{
        switch (request.getId())
        {{
            case PropertyRequestGetState:
                LOG(debug) << "Received request " << PropertyRequestGetState;
                return this->onPropertyRequestGetState(request);
{ServiceComponentSourceGenerator._generate_command_request_handler_registrations(self.context.name, self.context.component.interface.commands)}
{ServiceComponentSourceGenerator._generate_property_request_handler_registrations(self.context.name, self.context.component.interface.properties)}
            default:
                LOG(error) << "Received unknown request " << request;
                return message_broker::createErrorResponseMessage(request, message_broker::ResponseMessage::Result::UnsupportedRequest); 
        }}
    }});
    messageBroker.registerNotificationMessageHandler([this](const message_broker::Message& notification)
    {{
        switch (notification.getId())
        {{
{ServiceComponentSourceGenerator._generate_notification_handler_registrations(self.context.name, self.context.component.utilization.notifications)}
            default:
                LOG(error) << "Received unknown notification " << notification;
                break;
        }}
    }});
{ServiceComponentSourceGenerator._generate_register_property_value_change_handlers(self.context.name, self.context.component.interface.properties)}
}}

///////////////////////////////////////////////////////////////////////////////
// Request message handlers:
{self._generate_command_request_handler_definitions(self.context.component.interface.commands)}
{self._generate_property_request_handler_definitions(self.context.component.interface.properties)}
message_broker::ResponseMessage I{self.context.name}::onPropertyRequestGetState(const message_broker::Message& request)
{{
    return handleGetPropertyRequestMessage(request, m_propertyStateMachineState);
}}

///////////////////////////////////////////////////////////////////////////////
// Notification message handlers:
{self._generate_notification_handler_definitions(self.context.component.utilization.notifications)}
""")

    @staticmethod
    def _generate_includes(notifications):
        senders = set()
        for notification in notifications:
            senders.add(notification.name.split(".")[0])
        out_str = ""
        for sender in senders:
            out_str += f"""#include "ServiceComponent/I{sender}.hpp"
"""
        return out_str

    def _generate_transitions(self):
        out_str = ""
        for trans in self.context.component.statemachine.transitions:
            make_action = (
                f", StateMachine::MakeAction(&I{self.context.name}::{trans.action}, this)"
                if trans.action
                else ""
            )
            out_str += f"""Transition(State::{trans.state}, State::{trans.next}, Event::{trans.event}{make_action}),
            """
        return out_str

    @staticmethod
    def _generate_command_request_handler_registrations(context_name, commands):
        out_str = ""
        for request in commands:
            request_name = request.get_normalized_name()
            out_str += f"""
            case CommandRequest{request_name}:
                LOG(debug) << "Received request " << CommandRequest{request_name};
                return this->onCommandRequest{request_name}(request);"""
        return out_str

    @staticmethod
    def _generate_property_request_handler_registrations(context_name, properties):
        out_str = ""
        for property in properties:
            for property_name in property.get_all_normalized_names():
                out_str += f"""
            case PropertyRequest{property_name}:
                LOG(debug) << "Received request " << PropertyRequest{property_name};
                return this->onPropertyRequest{property_name}(request);"""
        return out_str

    @staticmethod
    def _generate_notification_handler_registrations(context_name, notifications):
        out_str = ""
        for notification in notifications:
            component_name, notification_name = notification.name.split(".")
            out_str += f"""
            case I{component_name}::Notification{notification_name}:
                LOG(debug) << "Received notification " << I{component_name}::Notification{notification_name};
                this->onNotification{component_name}{notification_name}(notification);
                break;"""
        return out_str

    @staticmethod
    def _generate_register_property_value_change_handlers(context_name, properties):
        out_str = ""
        for property in properties:
            if property.notification:
                out_str += f"""
    m_property{property.name}.registerValueChangeHandler([this](const IProperty<{property.type.type.value}>& changedProperty) {{
        this->notify(Notification{property.notification}, changedProperty.getValueAsJson());
    }});"""
        return out_str

    @staticmethod
    def _generate_subscription_id_list(context_name, notifications):
        out_str = ""
        for notification in notifications:
            component_name, notification_name = notification.name.split(".")
            out_str += f"""        sugo::service_component::I{component_name}::Notification{notification_name},
"""
        return out_str

    def _generate_command_request_handler_definitions(self, commands) -> str:
        out_str = ""
        for command in commands:
            command_name = command.get_normalized_name()
            if command.event:
                event_name = command.event.replace(".", "") if command.event else ""
                out_str += f"""
message_broker::ResponseMessage I{self.context.name}::onCommandRequest{command_name}(const message_broker::Message& request)
{{
    return handleCommandRequestMessage(request, Event::{event_name});
}}
"""
            if command.forward:
                forward_message = f"I{command.forward.replace('.', '::')}"
                out_str += f"""
message_broker::ResponseMessage I{self.context.name}::onCommandRequest{command_name}(const message_broker::Message& request)
{{
    return forward({forward_message}, request);
}}
"""
        return out_str

    def _generate_property_request_handler_definitions(self, properties) -> str:
        out_str = ""
        for property in properties:
            for property_name in property.get_all_normalized_names():
                handlerFunctionName = "handleGetPropertyRequestMessage" if property_name.startswith(Property.GET_PREFIX) else "handleSetPropertyRequestMessage"
                out_str += f"""
message_broker::ResponseMessage I{self.context.name}::onPropertyRequest{property_name}(const message_broker::Message& request)
{{
    return {handlerFunctionName}(request, m_property{property});
}}
"""
        return out_str
    
    def _generate_notification_handler_definitions(self, notifications) -> str:
        out_str = ""
        for notification in notifications:
            notification_name = notification.get_normalized_name()
            if notification.event or notification.action or notification.change:
                event_name = notification.event.replace(".", "") if notification.event else ""
                out_str += f"""
void I{self.context.name}::onNotification{notification_name}(const message_broker::Message&)
{{"""
                if notification.change:
                    out_str += f"\n    m_property{notification.change.property} = {notification.change.value};"
                if notification.action:
                    out_str += f"\n    {notification.action}();"
                if notification.event:
                    out_str += f"\n    handleNotificationMessage(Event::{event_name});"
                out_str += f"""
}}
"""
        return out_str
