'''
Contains the C++ generator for source and header files
'''

__author__      = "denis@schoener-one.de"
__copyright__   = "Copyright 2022, Schoener-One"

from pathlib import Path
import logging

class Generator:
    """Class to generate the C++ files"""
    def __init__(self, out_dir) -> None:
        self._out_dir = Path(out_dir)
        self._out_dir.mkdir(parents=True, exist_ok=True)
   
    def _source_file_path(self):
        return self._out_dir / 'src' / 'ServiceComponentsExecutionGroup.cpp'
    
    def _header_file_path(self):
        return self._out_dir / 'include' / 'ServiceComponentsExecutionGroup.hpp'
    
    def print_output_files(self):
        print(str(self._header_file_path()))
        print(str(self._source_file_path()))
        
    def generate(self, components):
        logging.info(f"generating component execution group files")
        component_names = list([name for name, component in components.items()])
        self._generate_header_file(self._header_file_path())
        self._generate_source_file(self._source_file_path(), component_names)
    
    def _generate_header_file(self, path):
        path.parent.mkdir(parents=True, exist_ok=True)
        with path.open("w", encoding="utf-8") as file:
            file.write(f'''
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: AUTOMATIC GENERATED FILE!
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IConfiguration.hpp"
#include "ServiceLocator.hpp"

namespace sugo
{{
/**
 * @brief Class representing an execution group of service components.
 * The execution group manages the start and stop of service component execution.
 */
class ServiceComponentsExecutionGroup final
{{
public:
    ~ServiceComponentsExecutionGroup() = default;

    /**
     * @brief Executes all components in a sufficient sequence.
     * This call does not return until stop() has been called or a an fatal
     * error has been occurred!
     *
     * @param  serviceLocator Service locator object.
     * @return true if all components could be executed successfully and have been stopped again.
     * @return false if at least one component failed to start.
     */
    bool start(const ServiceLocator& serviceLocator);
}};

}}  // namespace sugo
''')

    def _generate_source_file(self, path, components):
        path.parent.mkdir(parents=True, exist_ok=True)
        with path.open("w", encoding="utf-8") as file:
            file.write(f'''
///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: AUTOMATIC GENERATED FILE!
 */
///////////////////////////////////////////////////////////////////////////////

#include "ServiceComponentsExecutionGroup.hpp"
#include "CommandMessageBroker.hpp"
#include "ServiceComponent.hpp"
#include "ServiceComponentExecutionBundle.hpp"

{Generator._generate_component_includes(components)}
using namespace sugo;

bool ServiceComponentsExecutionGroup::start(const ServiceLocator& serviceLocator)
{{
    {Generator._generate_component_exec_bundles(components)}
    std::array<IServiceComponentExecutionBundle*, {len(components)}> bundles
    {{{{
        {Generator._generate_bundle_list(components)}
    }}}};

    for (auto const& bundle : bundles)
    {{
        if (!bundle->getServiceComponent().start())
        {{
            LOG(error) << "Failed to start component " << bundle->getId();
            return false;
        }}
    }}

    // Now wait until all have finished
    for (auto const& bundle : bundles)
    {{
        bundle->getContext().waitUntilFinished();
    }}
    return true;
}}
''')

    @staticmethod
    def _generate_component_includes(components):
        out_str = str()
        for name in components:
            out_str += f'''#include "{name}.hpp"
'''
        return out_str
    
    @staticmethod
    def _generate_component_exec_bundles(components):
        out_str = str()
        for name in components:
            out_str += f'''ServiceComponentExecutionBundle<{name}, const ServiceLocator> {name[0].lower() + name[1:]}(serviceLocator);
    '''
        return out_str

    @staticmethod
    def _generate_bundle_list(components):
        out_str = str()
        for name in components:
            out_str += f'''&{name[0].lower() + name[1:]},
        '''
        return out_str