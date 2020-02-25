/*
 * I.hpp
 *
 *  Created on: 17.11.2019
 *      Author: denis
 */

#ifndef ICUPROTATIONTRAY_HPP_
#define ICUPROTATIONTRAY_HPP_

#include <string>

namespace moco
{
namespace command
{
const std::string CupRotationTraySwitchOn     = "CupRotationTray.SwitchOn";
const std::string CupRotationTraySwitchOff    = "CupRotationTray.SwitchOff";
const std::string CupRotationTrayReset        = "CupRotationTray.Reset";
const std::string CupRotationTrayGetState     = "CupRotationTray.GetState";
const std::string CupRotationTrayGetPosition  = "CupRotationTray.GetPosition";
const std::string CupRotationTraySetPosition  = "CupRotationTray.SetPosition";
const std::string CupRotationTraySwitchOnTest = "CupRotationTray.SwitchOnTest";
const std::string CupRotationTrayTestRotation = "CupRotationTray.TestRotation";
}  // namespace command
}  // namespace moco

namespace moco
{
namespace notification
{
const std::string CupRotationTraySelfTestFinished = "CupRotationTray.SelfTestFinished";
const std::string CupRotationTrayTestRotationFinished = "CupRotationTray.TestRotationFinished";
}  // namespace notification
}  // namespace moco

#endif /* ICUPROTATIONTRAY_HPP_ */
