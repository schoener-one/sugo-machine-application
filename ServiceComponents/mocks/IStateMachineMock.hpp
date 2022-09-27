///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.05.2020
 */
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <gmock/gmock.h>

#include "IStateMachine.hpp"

namespace sugo
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
    MOCK_METHOD(EventQueue<test::Event>&, getEventQueue, ());
};

}  // namespace sugo
