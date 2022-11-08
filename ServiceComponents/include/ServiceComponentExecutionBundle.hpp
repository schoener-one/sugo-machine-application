///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one
 * @date:   2019-12-01
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CommandMessageBroker.hpp"
#include "IOContext.hpp"
#include "IRunnable.hpp"
#include "IServiceComponent.hpp"
#include "IServiceComponentExecutionBundle.hpp"

namespace sugo
{
/**
 * @brief Class to bundle all needed objects for executing the component.
 *
 * @tparam ComponentT    Component class to be bundled.
 * @tparam ComponentArgs Additional component arguments to be pass.
 */
template <class ComponentT, class... ComponentArgs>
class ServiceComponentExecutionBundle : public IServiceComponentExecutionBundle
{
public:
    explicit ServiceComponentExecutionBundle(ComponentArgs&... componentArgs)
        : m_id(ComponentT::ReceiverId),
          m_ioContext(m_id),
          m_broker(m_id, m_ioContext),
          m_component(m_broker, componentArgs...)
    {
    }
    ~ServiceComponentExecutionBundle() = default;

    bool start() override
    {
        return m_component.start();
    }

    void stop() override
    {
        m_component.stop();
    }

    bool isRunning() const override
    {
        return m_component.isRunning();
    }

    const std::string& getId() const override
    {
        return m_id;
    }

    void waitUntilFinished() override
    {
        m_ioContext.waitUntilFinished();
    }

    IServiceComponent& getServiceComponent() override
    {
        return m_component;
    }

private:
    const decltype(ComponentT::ReceiverId) m_id;
    message::IOContext                     m_ioContext;
    message::CommandMessageBroker          m_broker;
    ComponentT                             m_component;
};

}  // namespace sugo