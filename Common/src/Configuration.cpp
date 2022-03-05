///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   09.05.2020
 */
///////////////////////////////////////////////////////////////////////////////

#include <boost/algorithm/string/predicate.hpp>

#include "Configuration.hpp"

using namespace sugo;

const IConfiguration& Configuration::extract(const std::string& prefix,
                                             IConfiguration&    extractedConfiguration) const
{
    for (const auto& item : m_mapOptions)
    {
        if (boost::starts_with(item.first, prefix))
        {
            extractedConfiguration.add(item.second);
        }
    }
    return extractedConfiguration;
}
