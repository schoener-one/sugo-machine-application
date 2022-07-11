'''
Main entry point for service component generator
'''

__author__      = "denis@schoener-one.de"
__copyright__   = "Copyright 2022, Schoener-One"

import argparse
import sys
import logging

from ServiceComponentGenerator import Generator
from ServiceComponentParser import Parser, ParseException 
from PlantumlStateGenerator import PlantumlStateGenerator

def parse_args():
    global default_address
    parser = argparse.ArgumentParser(
        description='Register a new device to the Google device registry')
    parser.add_argument('-i', '--input-file', required=True,
                        help='YAML file to be parsed')
    parser.add_argument('-o', '--output-dir', required=False, default='generated',
                        help='Generated output file')
    parser.add_argument('-l', '--list-only', nargs='?', default=False, const=True,
                        help='Lists the generated files only, but does not generate')
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
    args = parse_args()
    try:
        with open(args.input_file, "r") as input_file:
            parsed_config = Parser(input_file).parse()
            if args.list_only:
                Generator(args.output_dir).print_output_files(parsed_config)
            else:
                Generator(args.output_dir).generate(parsed_config)
            PlantumlStateGenerator(args.output_dir).generate(parsed_config)
    except ParseException as ex:
        logging.error(f"parse error:  {str(ex)}")
        sys.exit(2)
    except Exception as ex:
        logging.error(f"unknown error: {str(ex)}")
        sys.exit(1)