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

#include <gmock/gmock.h>

#include "ServiceComponent/IStateMachine.hpp"

namespace sugo::service_component
{
namespace test
{
enum State
{
    State1,
    State2,
    State3
};
enum Event
{
    Event1,
    Event2,
    Event3
};
}  // namespace test

class IStateMachineMock : public IStateMachine<test::State, test::Event>
{
public:
    using typename IStateMachine<test::State, test::Event>::Action;

    MOCK_METHOD(test::State, getCurrentState, (), (const));
    MOCK_METHOD(bool, push, (const test::Event&));
    MOCK_METHOD(bool, processNextEvent, ());
    MOCK_METHOD(IQueue<test::Event>&, getEventQueue, ());
};

}  // namespace sugo::service_component
