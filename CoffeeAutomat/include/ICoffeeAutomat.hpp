/*
 * I.hpp
 *
 *  Created on: 17.11.2019
 *      Author: denis
 */

#ifndef ICOFFEEAUTOMAT_HPP_
#define ICOFFEEAUTOMAT_HPP_

#include <string>

namespace moco
{
namespace command
{
	static const std::string CoffeeAutomatSwitchOn    = "CoffeeAutomat.SwitchOn";
	static const std::string CoffeeAutomatSwitchOff   = "CoffeeAutomat.SwitchOff";
	static const std::string CoffeeAutomatReset       = "CoffeeAutomat.Reset";
	static const std::string CoffeeAutomatGetState    = "CoffeeAutomat.GetState";
} // namespace command

namespace notification
{
	static const std::string CoffeeAutomatSelfTestFinished = "CoffeeAutomat.SelfTestFinished";
} // namespace notification
} // namespace moco

#endif /* ICOFFEEAUTOMAT_HPP_ */
