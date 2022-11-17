///////////////////////////////////////////////////////////////////////////////
/**
 * @file
 * @license: Copyright 2022, Schoener-One
 *
 * @author: denis@schoener-one.de
 * @date:   2022-11-22
 */
///////////////////////////////////////////////////////////////////////////////

#include "HardwareAbstractionLayer.hpp"

using namespace sugo::hal;

HardwareAbstractionLayer::HardwareAbstractionLayer()
    : IHardwareAbstractionLayer(id::HardwareAbstractionLayer)
{
}

HardwareAbstractionLayer::~HardwareAbstractionLayer()  // NOLINT(modernize-use-equals-default)
{
}

void HardwareAbstractionLayer::postInitialization()
{
}

void HardwareAbstractionLayer::postFinalization()
{
}