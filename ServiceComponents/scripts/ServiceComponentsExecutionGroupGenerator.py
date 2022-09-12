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
        self._generate_header_file(self._header_file_path(), component_names)
        self._generate_source_file(self._source_file_path(), component_names)
    
    def _generate_header_file(self, path, components):
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

#include "ServiceComponentExecutionBundle.hpp"
#include "ServiceLocator.hpp"

#include <array>
#include <memory>

namespace sugo
{{
/**
 * @brief Class representing an execution group of service components.
 * The execution group manages the start and stop of service component execution.
 */
class ServiceComponentsExecutionGroup final
{{
public:
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
    
    /**
     * @brief Waits blocked until all components have been stopped.
     *
     */
    void waitUntilFinished();

    /**
     * @brief Stops the execution of all group components.
     * The call blocks until the last component has stopped.
     *
     */
    void stop();
    
private:
    /// @brief Number all of components
    static constexpr unsigned NumberOfComponents = {len(components)}u;

    std::array<std::unique_ptr<IServiceComponentExecutionBundle>, NumberOfComponents> m_bundles;
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
    for (auto const& bundle : m_bundles)
    {{
        if (!bundle->getServiceComponent().start())
        {{
            LOG(error) << "Failed to start component " << bundle->getId();
            return false;
        }}
    }}
    return true;
}}

void ServiceComponentsExecutionGroup::waitUntilFinished()
{{
    for (auto const& bundle : m_bundles)
    {{
        bundle->getContext().waitUntilFinished();
    }}
}}

void ServiceComponentsExecutionGroup::stop()
{{
    for (auto const& bundle : m_bundles)
    {{
        bundle->getContext().stop();
    }}
    waitUntilFinished();
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
        i = 0
        for name in components:
            out_str += f'''m_bundles[{i}] = std::make_unique<ServiceComponentExecutionBundle<{name}, const ServiceLocator>>(serviceLocator);
    '''
            i += 1
        return out_str

    @staticmethod
    def _generate_bundle_list(components):
        out_str = str()
        for name in components:
            out_str += f'''&{name[0].lower() + name[1:]},
        '''
        return out_str