/*
 * softPWM.hpp
 *
 *  Created on: 13.10.2019
 *      Author: denis
 */

#ifndef WIRINGPISTUB_INCLUDE_SOFTPWM_H_
#define WIRINGPISTUB_INCLUDE_SOFTPWM_H_

inline int  softPwmCreate (int /*pin*/, int /*value*/, int /*range*/) { return 0; }
inline void softPwmWrite  (int /*pin*/, int /*value*/) {}
inline void softPwmStop   (int /*pin*/) {}

#endif /* WIRINGPISTUB_INCLUDE_SOFTPWM_H_ */
