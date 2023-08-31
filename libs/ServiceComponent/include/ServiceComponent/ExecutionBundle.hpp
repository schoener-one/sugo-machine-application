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

#include <memory>

#include "Common/IRunnable.hpp"
#include "Common/Logger.hpp"
#include "Common/ProcessContext.hpp"
#include "MessageBroker/MessageBroker.hpp"
#include "ServiceComponent/IExecutionBundle.hpp"
#include "ServiceComponent/IServiceComponent.hpp"

namespace sugo::service_component
{
/**
 * @brief Class to bundle all service components for execution.
 *
 * @tparam ComponentT    Component class to be bundled.
 * @tparam ComponentArgs Additional component arguments to be pass.
 */
template <class ComponentT, class... ComponentArgs>
class ExecutionBundle : public IExecutionBundle
{
public:
    /// Component identifier
    static constexpr decltype(ComponentT::Identifier) Identifier = ComponentT::Identifier;

    // TODO remove dynamic memory allocation here. Possible solution could be a in-placement
    // FIXME the paramenter 'processContext' is inconsistent to ServiceComponent parameters
    // construction!
    /**
     * @brief Construct a new execution bundle object
     *
     * @param componentArgs Parameters for the bundled component. It is assumed that the
     * first parameter is the message broker, which will be passed automatically on first position.
     */
    explicit ExecutionBundle(ComponentArgs&&... componentArgs)
        : m_ioContext(std::make_shared<common::IOContext>(Identifier)),
          m_broker(std::make_shared<message_broker::MessageBroker>(Identifier, *m_ioContext)),
          m_processContext(std::make_shared<common::ProcessContext>(
              Identifier)),  // TODO configure thread policy and priority!
          m_component(std::make_shared<ComponentT>(*m_broker, *m_processContext,
                                                   std::forward<ComponentArgs&&>(componentArgs)...))
    {
    }
    ~ExecutionBundle() override = default;

    /// @brief Copy constructor.
    ExecutionBundle(const ExecutionBundle&) = delete;

    /**
     * @brief Move constructor.
     *
     * @param bundle Bundle object to move to this.
     */
    ExecutionBundle(ExecutionBundle&& bundle)
        : m_ioContext(std::move(bundle.m_ioContext)),
          m_broker(std::move(bundle.m_broker)),
          m_processContext(std::move(bundle.m_processContext)),
          m_component(std::move(bundle.m_component))
    {
    }

    /// @brief Copy assignment operator.
    ExecutionBundle& operator=(const ExecutionBundle&) = delete;

    /**
     * @brief Move assignment operator.
     *
     * @param bundle Bundle object to move to this.
     * @return This object.
     */
    ExecutionBundle& operator=(ExecutionBundle&& bundle)
    {
        *this = std::move(bundle);
        return *this;
    }

    bool start() override
    {
        return m_component->start();
    }

    void stop() override
    {
        m_component->stop();
    }

    bool isRunning() const override
    {
        return m_component->isRunning();
    }

    std::string getId() const override
    {
        return std::string(Identifier);
    }

    void waitUntilFinished() override
    {
        m_ioContext->waitUntilFinished();
    }

    /**
     * @brief Returns the concrete component object.
     *
     * @return Concrete component object.
     */
    ComponentT& getComponent()
    {
        return *m_component;
    }

    IServiceComponent& getServiceComponent() override
    {
        return *m_component;
    }

private:
    std::shared_ptr<common::IOContext>             m_ioContext;
    std::shared_ptr<message_broker::MessageBroker> m_broker;
    std::shared_ptr<common::ProcessContext>        m_processContext;
    std::shared_ptr<ComponentT>                    m_component;
};

}  // namespace sugo::service_component