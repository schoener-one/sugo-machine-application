"""
Contains the parser for parsing component configuration.

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

import logging
import yaml
import re
from ServiceComponentData import *

__author__ = "denis@schoener-one.de"
__copyright__ = "Copyright (C) 2020 by Denis Schoener"


class ParseException(Exception):
    """Class for parse exception"""

    def __init__(self, *args: object) -> None:
        super().__init__(*args)


class Parser:
    """Class to parse the YAML configuration of service components"""

    def __init__(self, config_file) -> None:
        self._config = yaml.safe_load(config_file)
        self._components = dict()

    def parse(self):
        # TODO validate via schema!
        for component in self._config["components"]:
            component_name = list(component.keys())[0]
            if component_name.startswith("_"):
                logging.info(f"ignoring component: {component_name}")
                continue
            config = component[component_name]
            logging.info(f"parsing component {component_name}")
            if "using" in config:
                self._parse_component_using(component_name, config)
            else:
                self._parse_component(component_name, config)
        self._validate_components_references()
        # self._create_receiver_lists()
        return self._components

    def _parse_component_using(self, component_name, config):
        used_component = config["using"]
        if used_component not in self._components:
            raise ParseException(
                f"unknown component {used_component} used in component {component_name}"
            )
        self._components[component_name] = self._components[used_component]

    def _parse_component(self, component_name, config):
        interface = (
            self._parse_interface(config["interface"])
            if "interface" in config and config["interface"]
            else Interface([], [], [])
        )
        utilization = (
            self._parse_utilization(config["utilization"])
            if "utilization" in config and config["utilization"]
            else Utilization([])
        )
        self._components[component_name] = ServiceComponent(
            interface, utilization, config["events"], None
        )
        self._parse_statemachine(
            component_name, self._components[component_name], config["statemachine"]
        )

    @staticmethod
    def _parse_proptery_type_declaration(declaration):
        result = re.findall(r"([a-z0-9]+)\s*(\[\d+\])?", declaration.strip())
        if len(result) != 1:
            raise ParseException(f"invalid property type declaration: '{declaration}'")
        return TypeDeclaration(
            PrimitiveTypes.from_declaration(result[0][0]),
            int(result[0][1][1:-1]) if result[0][1] else 1,
        )

    @staticmethod
    def _parse_property_change(entry) -> PropertyChange:
        assert type(entry) is dict
        property = entry["property"] if "property" in entry else None
        property_value = str(entry["value"]).lower() if "value" in entry else None
        return PropertyChange(property, property_value)

    @staticmethod
    def _parse_command(value) -> Command:
        if type(value) is dict:
            name = next(iter(value))
            entry = value[name]
            event = entry["event"] if "event" in entry else None
            forward = entry["forward"] if "forward" in entry else None
            if event and forward:
                raise ParseException(
                    f"only event {event} creation or forwarding is allowed for a interface message"
                )
            return Command(name, event, forward)
        else:  # str
            return Command(str(value), None, False)
        
    @staticmethod
    def _parse_notification(value) -> Notification:
        if type(value) is dict:
            name = next(iter(value))
            return Notification(name)
        else:
            return Notification(str(value))
        
    @staticmethod
    def _parse_property(value) -> Property:
        assert type(value) is dict
        name = next(iter(value))
        entry = value[name]
        return Property(
            name,
            Parser._parse_proptery_type_declaration(entry["type"]),
            entry["value"] if "value" in entry else None,
            entry["min"] if "min" in entry else None,
            entry["max"] if "max" in entry else None,
            entry["notification"] if "notification" in entry else None,
            entry["readonly"] if "readonly" in entry else False
        )

    @staticmethod
    def _parse_subscribed_notification(value) -> SubscribedNotification:
        if type(value) is dict:
            name = next(iter(value))
            entry = value[name]
            event = entry["event"] if "event" in entry else None
            action = entry["action"] if "action" in entry else None
            change = Parser._parse_property_change(entry["change"]) if "change" in entry else None
            return SubscribedNotification(name, event, action, change)
        else:
            return SubscribedNotification(str(value), None, None, None)

    def _parse_interface(self, interface) -> Interface:
        commands = (
            list(
                map(
                    lambda message: Parser._parse_command(message),
                    interface["commands"],
                )
            )
            if "commands" in interface
            else list()
        )
        notifications = (
            list(
                map(
                    lambda message: Parser._parse_notification(message),
                    interface["notifications"],
                )
            )
            if "notifications" in interface
            else list()
        )
        properties = (
            list(
                map(
                    lambda property: Parser._parse_property(property),
                    interface["properties"],
                )
            )
            if "properties" in interface
            else list()
        )
        return Interface(commands, notifications, properties)

    def _parse_utilization(self, interface) -> Utilization:
        notifications = (
            list(
                map(
                    lambda message: Parser._parse_subscribed_notification(message),
                    interface["notifications"],
                )
            )
            if "notifications" in interface
            else list()
        )
        return Utilization(notifications)
    

    def _parse_statemachine(self, component_name, component, config):
        start_state = config["start"]
        if not start_state in config["states"]:
            raise ParseException(f"unknown start state: {start_state}")
        component.statemachine = StateMachine(config["states"], start_state, list())
        for transition in config["transitions"]:
            self._parse_transition(component_name, component, transition)

    def _parse_transition(self, component_name, component, config):
        regex = re.compile(config["state"])
        matching_states = list(
            filter(lambda x: regex.match(x), component.statemachine.states)
        )
        if len(matching_states) == 0:
            raise ParseException(
                f"state {config['state']} not in statemachine of component {component_name}"
            )
        if not config["next"] in component.statemachine.states:
            raise ParseException(
                f"next state {config['next']} not in statemachine of component {component_name}"
            )
        if not config["event"] in component.events:
            raise ParseException(
                f"event {config['event']} not in events of component {component_name}"
            )
        for state in matching_states:
            existing_transition = list(
                filter(
                    lambda x: x.state == state
                    and x.next == config["next"]
                    and x.event == config["event"],
                    component.statemachine.transitions,
                )
            )
            if not existing_transition:
                transition = Transition(
                    state,
                    config["next"],
                    config["event"],
                    config["action"] if "action" in config else None,
                )
                component.statemachine.transitions.append(transition)
            else:
                raise ParseException(
                    f"Transition ({state} -> {config['next']} / {config['event']}) already exists"
                )

    def _validate_components_references(self):
        for component_name, component in self._components.items():
            for notification in component.utilization.notifications:
                chunks = notification.name.split(".")
                if len(chunks) != 2:
                    raise ParseException(
                        f"invalid utilized notification {notification} from {component_name} has unexpected number of separators"
                    )
                if not chunks[0] in self._components.keys():
                    raise ParseException(
                        f"invalid utilized notification {notification} from {component_name} refers to unknown component {chunks[0]}"
                    )
                if not self._component_has_notification(chunks[0], chunks[1]):
                    raise ParseException(
                        f"invalid utilized notification {notification} from {component_name} refers to unknown notification {chunks[1]} of {chunks[0]}"
                    )
                if notification.change:
                    if not self._component_has_property(component_name, notification.change.property):
                        raise ParseException(
                            f"invalid notification property change {notification.change.property} from {component_name} refers to unknown property"
                        )                        
            for command in component.interface.commands:
                if "." in command.name:
                    raise ParseException(
                        f"invalid command {command} from {component_name} has separator"
                    )
            for notification in component.interface.notifications:
                if "." in notification.name:
                    raise ParseException(
                        f"invalid interface notification {notification} from {component_name} has separator"
                    )
            for property in component.interface.properties:
                if property.notification:
                    if not self._component_has_notification(
                        component_name, property.notification
                    ):
                        raise ParseException(
                            f"invalid property {property} from {component_name} refers to unknown notification {property.notification}"
                        )

    def _component_has_notification(self, component, notification_name) -> bool:
        return any(
            notification_name == notification.name
            for notification in self._components[component].interface.notifications
        )

    def _component_has_property(self, component, property_name) -> bool:
        return any(
            property_name == property.name
            for property in self._components[component].interface.properties            
        )