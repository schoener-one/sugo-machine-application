///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2022-10-25
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Logger.hpp"
#include "StatedServiceComponent.hpp"

namespace sugo
{
/**
 * @brief Helper class to control other service components.
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
        const message::CommandResponse response =
            static_cast<DerivedClassT*>(this)->send(ServiceComponentT::CommandGetState);

        if (response.result() != message::CommandResponse_Result_SUCCESS)
        {
            LOG(error) << "Failed to get component state";
            return false;
        }

        const Json& jsonState = Json::parse(response.response()).at(message::protocol::IdState);

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
        LOG(debug) << "Turn off service component: " << ServiceComponentT::ReceiverId;
        const auto response =
            static_cast<DerivedClassT*>(this)->send(ServiceComponentT::CommandSwitchOff);

        if (response.result() != message::CommandResponse_Result_SUCCESS)
        {
            LOG(error) << "Failed to turn off component " << ServiceComponentT::ReceiverId;
            return false;
        }

        return true;
    }
};
}  // namespace sugo