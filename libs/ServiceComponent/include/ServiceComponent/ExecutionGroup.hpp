
///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file
 *
 * @author: AUTOMATIC GENERATED FILE!
 * @data: 2023-12-07
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

#include <tuple>

namespace sugo::service_component
{
/**
 * @brief Class representing an execution group of service component bundles.
 * A execution group handles a defined number of service component bundles to start
 * and stop the component instances in a common way.
 * To create a group you can implement it as follows for example:
 * <code>
 *   ExecutionGroup executionGroup{
 *          ExecutionBundle<ServiceComponentA, common::IConfiguration>{myConfiguration},
 *          ExecutionBundle<ServiceComponentB, common::IConfiguration>{myConfiguration}};
 *   executionGroup.start(); // Will not return until services are stopped!
 * </code>
 * @tparam ExecutionBundleT List of execution bundles which are managed by this class.
 */
template <class... ExecutionBundleT>
class ExecutionGroup final
{
public:
    /// @brief Type declaration of the execution bundles.
    using ExecutionBundleType = std::tuple<ExecutionBundleT...>;

    /**
     * @brief Constructs a new service component execution group object.
     * All bundles must be from type ExecutionBundle and the grouped
     * service components must expect the same constructor parameters!
     *
     * @param args List of execution bundle types.
     */
    ExecutionGroup(ExecutionBundleT&&... args)
        : m_bundles(std::forward<ExecutionBundleT&&>(args)...)
    {
    }

    /// @brief Number of bundles contained in ExecutionBundles.
    static constexpr std::size_t NumberOfBundles = sizeof...(ExecutionBundleT);

    /**
     * @brief Executes all components in a sequence.
     * This call does not return until stop() has been called or a an fatal
     * error has been occurred!
     *
     * @return true if all components could be started successfully and have been stopped again.
     * @return false if at least one component failed to start.
     */
    bool start()
    {
        bool success = true;
        std::apply([&success](auto&... bundle) { ((success = success && bundle.start()), ...); },
                   m_bundles);
        return success;
    }

    /**
     * @brief Waits blocked until all components have been stopped.
     *
     */
    void waitUntilFinished()
    {
        std::apply([](auto&... bundle) { ((bundle.waitUntilFinished()), ...); }, m_bundles);
    }

    /**
     * @brief Stops the execution of all group components.
     * The call blocks until the last component has stopped.
     *
     */
    void stop()
    {
        std::apply([](auto&... bundle) { ((bundle.stop()), ...); }, m_bundles);
    }

    /**
     * @brief Returns the appropriate bundle object.
     *
     * @tparam BundleT Bundle object type.
     * @return Bundle object.
     */
    template <class BundleT>
    auto& get()
    {
        return std::get<BundleT>(m_bundles);
    }

    const ExecutionBundleType& getBundles() const
    {
        return m_bundles;
    }

private:
    ExecutionBundleType m_bundles;  ///< Bundles of the group.
};
}  // namespace sugo::service_component
