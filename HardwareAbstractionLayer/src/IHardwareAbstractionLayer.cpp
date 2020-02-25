/*
 * IHardwareAbstractionLayer.cpp
 *
 *  Created on: 30.12.2019
 *      Author: denis
 */

#include "IHardwareAbstractionLayer.hpp"

using namespace moco;

std::shared_ptr<IHardwareAbstractionLayer> IHardwareAbstractionLayer::s_hal = nullptr;


