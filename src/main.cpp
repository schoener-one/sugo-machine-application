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

#include "Common/Logger.hpp"
#include "Common/Types.hpp"
#include "MachineApplication/MachineApplication.hpp"

using namespace sugo;
using namespace sugo::machine_application;

static constexpr int Success = 0;
static constexpr int Failure = -1;

// FIXME Add stop handling here!
int main(int argc, char const **argv)
{
    common::Logger::init();
    MachineApplication application("SugoMachine");
    return application.start(argc, argv) ? Success : Failure;
}
