///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   04.04.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMON_TEST_ICONFIGURATIONMOCK_HPP_
#define COMMON_TEST_ICONFIGURATIONMOCK_HPP_

#include <gmock/gmock.h>

#include "IConfiguration.hpp"

namespace moco
{
class IConfigurationMock : public IConfiguration
{
public:
    MOCK_CONST_METHOD0(getOptionDescriptions, OptionDescriptions());
    MOCK_METHOD1(set, bool(const VariablesMap&));
    MOCK_METHOD1(add, void(const Option&));
    MOCK_CONST_METHOD1(getOption, const Option&(const std::string&));
};
}  // namespace moco

#endif // COMMON_TEST_ICONFIGURATIONMOCK_HPP_
