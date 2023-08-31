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

#pragma once

#include "Common/Logger.hpp"
#include "ServiceComponent/StatedServiceComponent.hpp"

namespace sugo::machine_service_component
{
/**
 * @brief Class provides a service component control service.
 *
 * @tparam DerivedClassT Derived class (CRTP)
 */
template <class DerivedClassT>
class ControlService
{
public:
    /**
     * @brief Requests the state of another component.
     *
     * @tparam ServiceComponentT      Type of the service component to request.
     * @tparam ServiceComponentStateT Type of the service component state.
     * @param[out] state              Receives the requested state if the request was successful.
     * @return true                   If the request could be sent successfully.
     * @return false                  If the request could not be sent successfully.
     */
    template <class ServiceComponentT, class ServiceComponentStateT>
    bool getStateOf(ServiceComponentStateT& state)
    {
        // static_assert(
        //     not std::is_base_of_v<ServiceComponentT, StatedServiceComponent<StateT, EventT>>);
        const message_broker::ResponseMessage response =
            static_cast<DerivedClassT*>(this)->send(ServiceComponentT::RequestGetState);

        if (response.getResult() != message_broker::ResponseMessage::Result::Success)
        {
            LOG(error) << "Failed to get component state";
            return false;
        }

        const common::Json& jsonState =
            common::Json::parse(response.getPayload()).at(service_component::id::State);

        if (jsonState.empty())
        {
            LOG(fatal) << "Invalid state parameter received";
            return false;
        }

        state = static_cast<ServiceComponentStateT>(jsonState.get<unsigned>());
        return true;
    }

    /**
     * @brief Switch off another component.
     *
     * @tparam ServiceComponentT      Type of the service component to request.
     * @return true                   If the request could be sent successfully.
     * @return false                  If the request could not be sent successfully.
     */
    template <class ServiceComponentT>
    bool switchOffServiceComponent()
    {
        LOG(debug) << "Turn off service component: " << ServiceComponentT::Identifier;
        const auto success =
            static_cast<DerivedClassT*>(this)->send(ServiceComponentT::RequestSwitchOff);

        if (!success)
        {
            LOG(error) << "Failed to turn off component " << ServiceComponentT::Identifier;
            return false;
        }

        return true;
    }
};
}  // namespace sugo::machine_service_component