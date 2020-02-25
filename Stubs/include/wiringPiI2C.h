/*
 * wiringPiI2C.h
 *
 *  Created on: 13.10.2019
 *      Author: denis
 */

#ifndef WIRINGPISTUB_INCLUDE_WIRINGPII2C_H_
#define WIRINGPISTUB_INCLUDE_WIRINGPII2C_H_

inline int wiringPiI2CRead           (int /*fd*/) { return 0; }
inline int wiringPiI2CReadReg8       (int /*fd*/, int /*reg*/) { return 0; }
inline int wiringPiI2CReadReg16      (int /*fd*/, int /*reg*/) { return 0; }

inline int wiringPiI2CWrite          (int /*fd*/, int /*data*/) { return 0; }
inline int wiringPiI2CWriteReg8      (int /*fd*/, int /*reg*/, int /*data*/) { return 0; }
inline int wiringPiI2CWriteReg16     (int /*fd*/, int /*reg*/, int /*data*/) { return 0; }

inline int wiringPiI2CSetupInterface (const char */*device*/, int /*devId*/) { return 0; }
inline int wiringPiI2CSetup          (const int /*devId*/) { return 0; }

#endif /* WIRINGPISTUB_INCLUDE_WIRINGPII2C_H_ */
