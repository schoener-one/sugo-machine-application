///////////////////////////////////////////////////////////////////////////////
/** @file
 * @license: CLOSED
 *
 * @author: denis
 * @date:   02.05.2020
 */
///////////////////////////////////////////////////////////////////////////////

#ifndef COMMON_TEST_ISTATEMACHINEMOCK_HPP_
#define COMMON_TEST_ISTATEMACHINEMOCK_HPP_

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

    MOCK_CONST_METHOD0(getCurrentState, test::State());
    MOCK_METHOD2(push, void(const test::Event &, Action));
};

}  // namespace sugo

#endif  // COMMON_TEST_ISTATEMACHINEMOCK_HPP_
