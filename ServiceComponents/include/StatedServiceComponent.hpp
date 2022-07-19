/*
 * CommandExcecutionComponent.hpp
 *
 *  Created on: 17.11.2019
 *      Author: denis
 */

#ifndef STATEDSERVICECOMPONENT_HPP_
#define STATEDSERVICECOMPONENT_HPP_

#include "ServiceComponent.hpp"

#include <sstream>

#include "Globals.hpp"
#include "IStateMachine.hpp"
#include "ServiceComponent.hpp"

namespace sugo
{
/**
 * Class to receive handle state machine events from commands.
 *
 * @tparam StateT State type
 * @tparam EventT Event type
 *
 * @todo Needs to be transformed to a base class of all ServiceComponents in order
 *       to handle messages in a generic manner!
 */
template <class StateT, class EventT>
class StatedServiceComponent : public ServiceComponent
{
protected:
    explicit StatedServiceComponent(ICommandMessageBroker&                       messageBroker,
                                    const ICommandMessageBroker::ReceiverIdList& notifReceivers,
                                    IStateMachine<StateT, EventT>&               stateMachine)
        : ServiceComponent(messageBroker, notifReceivers), m_stateMachine(stateMachine)
    {
    }

    message::CommandResponse handleStateChangeCommand(const message::Command& command,
                                                      const EventT&           event)
    {
        LOG(debug) << "Received command '" << command.name() << "'";
        message::CommandResponse response;
        response.set_id(command.id());
        response.set_result(message::CommandResponse_Result_SUCCESS);
        m_stateMachine.push(event, [&](const EventT& event, const StateT& state) {
            LOG(warning) << "Invalid transition for '" << event << "' in state '" << state << "'";
            response.set_result(message::CommandResponse_Result_INVALID_STATE);
        });
        return response;
    }

    message::CommandResponse handleCommandGetState(const message::Command& command)
    {
        LOG(debug) << "Received command '" << command.name() << "'";
        message::CommandResponse response;
        response.set_id(command.id());
        response.set_result(message::CommandResponse_Result_SUCCESS);
        std::ostringstream oss;
        oss << "{\"state\":\"" << m_stateMachine.getCurrentState() << "\"}";
        response.set_response(oss.str());
        return response;
    }

private:
    sugo::IStateMachine<StateT, EventT>& m_stateMachine;
};

}  // namespace sugo

#endif  // STATEDSERVICECOMPONENT_HPP_
