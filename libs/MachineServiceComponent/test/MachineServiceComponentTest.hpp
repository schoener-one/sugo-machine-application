///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   22.09.2020
 *
 * @license: Copyright (C) 2020 by Denis Schoener
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "Common/IConfigurationMock.hpp"
#include "MachineServiceComponent/FilamentCoilMotor.hpp"

namespace sugo::test
{
class MachineServiceComponentTest;

class FilamentCoilMotorTestable : public sugo::machine_service_component::FilamentCoilMotor
{
public:
    using FilamentCoilMotor::FilamentCoilMotor;

    friend class MachineServiceComponentTest;
    FRIEND_TEST(MachineServiceComponentTest, StartFilamentCoilMotor);
};
}  // namespace sugo::test