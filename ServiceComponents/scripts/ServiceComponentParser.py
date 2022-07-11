'''
Contains the C++ parser for parsing component configuration
'''

__author__      = "denis@schoener-one.de"
__copyright__   = "Copyright 2022, Schoener-One"
    
    
from ServiceComponentData import *
import yaml
import logging

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
        for component in self._config['components']:
            component_name = list(component.keys())[0]
            config = component[component_name]
            logging.info(f"parsing component {component_name}")
            self._parse_component(component_name, config)
        self._validate_commands()
        return self._components
            
    def _parse_component(self, name, config):
        self._current_component = name
        self._components[name] = ServiceComponent(config['commands'],
                                                  config['notifications'] if 'notifications' in config else list(),
                                                  config['events'], None)
        self._parse_statemachine(config['statemachine'])

    def _parse_statemachine(self, config):
        start_state = config['start']
        if not start_state in config['states']:
            raise ParseException(f"unknown start state: {start_state}")
        self._components[self._current_component].statemachine = StateMachine(config['states'], start_state, list())
        for transition in config['transitions']:
            self._parse_transition(transition)
    
    def _parse_transition(self, config):
        if not config['state'] in self._components[self._current_component].statemachine.states:
            raise ParseException(f"state {config['state']} not in statemachine of component {self._current_component}")
        if not config['next'] in self._components[self._current_component].statemachine.states:
            raise ParseException(f"next state {config['next']} not in statemachine of component {self._current_component}")
        if not config['event'] in self._components[self._current_component].events:
            raise ParseException(f"event {config['event']} not in events of component {self._current_component}")
        transition = Transition(config['state'], config['next'], config['event'], config['action'] if 'action' in config else None)
        self._components[self._current_component].statemachine.transitions.append(transition)
       
    def _validate_commands(self):
        for component_name, component in self._components.items():
            for command in component.commands:
                splitted_name = command.split('.')
                if len(splitted_name) == 2:
                    if not splitted_name[0] in self._components.keys():
                        raise ParseException(f"invalid command {command} from {component_name} refers to unknown component {splitted_name[0]}")                        
                    if not self._component_has_notification(splitted_name[0], splitted_name[1]):
                        raise ParseException(f"invalid command {command} from {component_name} has no notification {splitted_name[1]} in {splitted_name[0]}")
                elif len(splitted_name) > 2:
                    raise ParseException(f"invalid command {command} from {component_name} has too many separators")
            for notification in component.notifications:
                if '.' in notification:
                    raise ParseException(f"invalid notification {notification} from {component_name} has separator")
                
    def _component_has_notification(self, component, notification) -> bool:
        return notification in self._components[component].notifications