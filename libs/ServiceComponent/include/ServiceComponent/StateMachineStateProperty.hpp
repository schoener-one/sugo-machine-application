///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: Denis Schoener (denis@schoener-one.de)
 * @date:   15.03.2024
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

#pragma once

#include "Common/Types.hpp"
#include "ServiceComponent/Property.hpp"
#include "ServiceComponent/StateMachine.hpp"

namespace sugo::service_component
{
/// @brief Class for state machine state property.
template <typename StateMachineT>
class StateMachineStateProperty : public Property<uint32_t>
{
public:
    StateMachineStateProperty(const StateMachineT& stateMachine)
        : Property<uint32_t>("State", stateMachine.getCurrentState()), m_stateMachine(stateMachine)
    {
    }

    common::Json getValueAsJson() const override
    {
        return common::Json(
            {{id::PropertyValue, static_cast<uint32_t>(m_stateMachine.getCurrentState())}});
    }

private:
    const StateMachineT& m_stateMachine;
};
}  // namespace sugo::service_component
