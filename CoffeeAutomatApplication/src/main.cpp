/*
 * main.cpp
 *
 *  Created on: 30.11.2019
 *      Author: denis
 */

#include <string>

#include "Globals.hpp"
#include "Logger.hpp"
#include "CoffeeAutomatApplication.hpp"

int main(int argc, char const **argv)
{
	moco::Logger::init();
	moco::CoffeeAutomatApplication application;
	return application.start(argc, argv) ? 0 : -1;
}
