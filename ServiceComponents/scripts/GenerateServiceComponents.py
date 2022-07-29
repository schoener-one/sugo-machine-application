'''
Main entry point for service component generator
'''

__author__      = "denis@schoener-one.de"
__copyright__   = "Copyright 2022, Schoener-One"

import argparse
import sys
import logging

from ServiceComponentGenerator import Generator as ServiceComponentGenerator
from ServiceComponentParser import Parser, ParseException 
from PlantumlStateGenerator import Generator as PlantumlStateGenerator
from ServiceComponentsExecutionGroupGenerator import Generator as ServiceComponentsExecutionGroupGenerator
from ServiceComponentTemplateGenerator import Generator as TemplateGenerator

def _generate_templates(generator, parsed_config):
    generator.generate(parsed_config)

def _generate_exec_group(generator, parsed_config, list_only):
    if list_only:
        generator.print_output_files()
    else:  
        generator.generate(parsed_config)
    
def _generate_service_components(generator, parsed_config, list_only):
    if list_only:
        generator.print_output_files(parsed_config)
    else:
        generator.generate(parsed_config)
    
def _generate_state_machine(generator, parsed_config):
    generator.generate(parsed_config)
        
def _generate(args, parsed_config):
    if args.generate_templates and not args.list_only:
        _generate_templates(TemplateGenerator(args.output_dir), parsed_config)
    if args.generate_exec_group:
        _generate_exec_group(ServiceComponentsExecutionGroupGenerator(args.output_dir), parsed_config, args.list_only)
    if args.generate_service_components:
        _generate_service_components(ServiceComponentGenerator(args.output_dir), parsed_config, args.list_only)
    if args.generate_state_diagrams and not args.list_only:
        _generate_state_machine(PlantumlStateGenerator(args.output_dir), parsed_config)
        
def _parse_args():
    global default_address
    parser = argparse.ArgumentParser(
        description='Register a new device to the Google device registry')
    parser.add_argument('-i', '--input-file', required=True,
                        help='YAML file to be parsed')
    parser.add_argument('-o', '--output-dir', required=False, default='generated',
                        help='Generated output file')
    parser.add_argument('-l', '--list-only', nargs='?', default=False, const=True,
                        help='Lists the generated files only, but does not generate')
    parser.add_argument('-t', '--generate-templates', nargs='?', default=False, const=True,
                        help='Indicates if template classes should be generated')    
    parser.add_argument('-e', '--generate-exec-group', nargs='?', default=False, const=True,
                        help='Indicates if execution group should be generated')    
    parser.add_argument('-s', '--generate-service-components', nargs='?', default=False, const=True,
                        help='Indicates if service components should be generated')    
    parser.add_argument('-d', '--generate-state-diagrams', nargs='?', default=False, const=True,
                        help='Indicates if service components state diagrams should be generated')    
    levels = {
        'error': logging.ERROR,
        'warning': logging.WARN,
        'info': logging.INFO,
        'debug': logging.DEBUG
    }
    parser.add_argument('-L', '--log-level', choices=list(levels.keys()),
                        default="warning", required=False,
                        help='Sets the log level')
    args = parser.parse_args()
    logging.basicConfig(level=levels[args.log_level], format='%(levelname)s: %(message)s')
    return args

if __name__ == '__main__':
    args = _parse_args()
    try:
        with open(args.input_file, "r") as input_file:
            parsed_config = Parser(input_file).parse()
            _generate(args, parsed_config)
    except ParseException as ex:
        logging.error(f"parse error:  {str(ex)}")
        sys.exit(2)
    except Exception as ex:
        logging.error(f"unknown error: {str(ex)}")
        sys.exit(1)