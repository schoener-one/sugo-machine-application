'''
Contains the parser for parsing component configuration
'''

import logging
import yaml
import re
from ServiceComponentData import *
__author__ = "denis@schoener-one.de"
__copyright__ = "Copyright 2022, Schoener-One"


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
        for component in self._config['components']:
            component_name = list(component.keys())[0]
            if component_name.startswith('_'):
                logging.info(f"ignoring component: {component_name}")
                continue
            config = component[component_name]
            logging.info(f"parsing component {component_name}")
            if 'using' in config:
                self._parse_component_using(component_name, config)
            else:
                self._parse_component(component_name, config)
        self._validate_messages()
        self._create_receiver_lists()
        return self._components

    def _parse_component_using(self, component_name, config):
        used_component = config['using']
        if used_component not in self._components:
            raise ParseException(
                f"unknown component {used_component} used in component {component_name}")
        self._components[component_name] = self._components[used_component]

    def _parse_component(self, component_name, config):
        inbound = self._parse_inbound(
            config['inbound']) if 'inbound' in config else Inbound([], [])
        outbound = self._parse_outbound(
            config['outbound']) if 'outbound' in config else Outbound([])
        self._components[component_name] = ServiceComponent(inbound, outbound,
                                                            config['events'], None)
        self._parse_statemachine(
            component_name, self._components[component_name], config['statemachine'])

    @staticmethod
    def _parse_message(value) -> Message:
        if type(value) is dict:
            name = next(iter(value))
            value = value[name]
            event = value['event'] if 'event' in value else None
            forward = value['forward'] if 'forward' in value else None
            if event and forward:
                raise ParseException(
                    f"only event {event} creation or forwarding is allowed for a inbound message")
            return Message(name, event, forward)
        else:  # str
            return Message(str(value), None, False)

    def _parse_inbound(self, inbound) -> Inbound:
        commands = list(
            map(lambda message: Parser._parse_message(message), inbound['commands'])) if 'commands' in inbound else list()
        notifications = list(map(lambda message: Parser._parse_message(
            message), inbound['notifications'])) if 'notifications' in inbound else list()
        return Inbound(commands, notifications)

    def _parse_outbound(self, outbound) -> Outbound:
        return Outbound(
            [Notification(name, list()) for name in outbound['notifications']] if outbound and 'notifications' in outbound else list())

    def _parse_statemachine(self, component_name, component, config):
        start_state = config['start']
        if not start_state in config['states']:
            raise ParseException(f"unknown start state: {start_state}")
        component.statemachine = StateMachine(
            config['states'], start_state, list())
        for transition in config['transitions']:
            self._parse_transition(component_name, component, transition)

    def _parse_transition(self, component_name, component, config):
        regex = re.compile(config['state'])
        matching_states = list(
            filter(lambda x: regex.match(x), component.statemachine.states))
        if len(matching_states) == 0:
            raise ParseException(
                f"state {config['state']} not in statemachine of component {component_name}")
        if not config['next'] in component.statemachine.states:
            raise ParseException(
                f"next state {config['next']} not in statemachine of component {component_name}")
        if not config['event'] in component.events:
            raise ParseException(
                f"event {config['event']} not in events of component {component_name}")
        for state in matching_states:
            existing_transition = list(filter(lambda x: x.state == state and x.next ==
                                       config['next'] and x.event == config['event'], component.statemachine.transitions))
            if not existing_transition:
                transition = Transition(state, config['next'], config['event'],
                                        config['action'] if 'action' in config else None)
                component.statemachine.transitions.append(transition)
            else:
                raise ParseException(
                    f"Transition ({state} -> {config['next']} / {config['event']}) already exists")

    def _validate_messages(self):
        for component_name, component in self._components.items():
            for notification in component.inbound.notifications:
                chunks = notification.name.split('.')
                if len(chunks) != 2:
                    raise ParseException(
                        f"invalid notification {notification} from {component_name} has unexpected number of separators")
                if not chunks[0] in self._components.keys():
                    raise ParseException(
                        f"invalid command {notification} from {component_name} refers to unknown component {chunks[0]}")
                if not self._component_has_notification(chunks[0], chunks[1]):
                    raise ParseException(
                        f"invalid command {notification} from {component_name} has no notification {chunks[1]} in {chunks[0]}")
            for command in component.inbound.commands:
                if '.' in command.name:
                    raise ParseException(
                        f"invalid command {command.name} from {component_name} has separator")
            for notification in component.outbound.notifications:
                if '.' in notification.name:
                    raise ParseException(
                        f"invalid notification {notification.name} from {component_name} has separator")

    def _component_has_notification(self, component, notification_name) -> bool:
        return any(notification_name == notification.name for notification in self._components[component].outbound.notifications)

    def _create_receiver_lists(self):
        for component_name, component in self._components.items():
            for notification in component.inbound.notifications:
                publisher_name, notification_name = notification.name.split('.')
                publisher = self._components[publisher_name]
                publisher_notif = next(filter(
                    lambda notif: notif.name == notification_name, publisher.outbound.notifications))
                publisher_notif.receivers.append(component_name)
