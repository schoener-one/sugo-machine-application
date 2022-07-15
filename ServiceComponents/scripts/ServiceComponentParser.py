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
            if 'using' in config:
                self._parse_component_using(component_name, config)
            else:
                self._parse_component(component_name, config)
        self._validate_commands()
        return self._components
            
    def _parse_component_using(self, component_name, config):
        used_component = config['using']
        if used_component not in self._components:
            raise ParseException(f"unknown component {used_component} used in component {component_name}")
        self._components[component_name] = self._components[used_component]
        
    def _parse_component(self, component_name, config):
        self._components[component_name] = ServiceComponent(config['commands'],
                                                  config['notifications'] if 'notifications' in config else list(),
                                                  config['events'], None)
        self._parse_statemachine(component_name, self._components[component_name], config['statemachine'])

    def _parse_statemachine(self, component_name, component, config):
        start_state = config['start']
        if not start_state in config['states']:
            raise ParseException(f"unknown start state: {start_state}")
        component.statemachine = StateMachine(config['states'], start_state, list())
        for transition in config['transitions']:
            self._parse_transition(component_name, component, transition)
    
    def _parse_transition(self, component_name, component, config):
        if not config['state'] in component.statemachine.states:
            raise ParseException(f"state {config['state']} not in statemachine of component {component_name}")
        if not config['next'] in component.statemachine.states:
            raise ParseException(f"next state {config['next']} not in statemachine of component {component_name}")
        if not config['event'] in component.events:
            raise ParseException(f"event {config['event']} not in events of component {component_name}")
        transition = Transition(config['state'], config['next'], config['event'], config['action'] if 'action' in config else None)
        component.statemachine.transitions.append(transition)
       
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