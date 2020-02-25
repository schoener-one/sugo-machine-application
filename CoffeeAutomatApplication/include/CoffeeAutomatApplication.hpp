/*
 * CoffeeAutomatApplication.hpp
 *
 *  Created on: 01.12.2019
 *      Author: denis
 */

#ifndef COFFEEAUTOMATAPPLICATION_HPP_
#define COFFEEAUTOMATAPPLICATION_HPP_

#include "Globals.hpp"
#include <boost/program_options.hpp>

namespace moco
{

/**
 * CoffeeAutomat application class.
 */
class CoffeeAutomatApplication final
{
public:
	CoffeeAutomatApplication();

	void parseOptions(int argc, char **argv);
	int start();

	static void initLogging(boost::log::trivial::severity_level severity);

private:
	boost::program_options::variables_map m_vm;
	bool m_initSuccessfully;
};

} // namespace moco

#endif /* COFFEEAUTOMATAPPLICATION_HPP_ */
