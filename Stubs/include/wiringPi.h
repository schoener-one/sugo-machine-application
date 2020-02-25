/*
 * wiringPi.hpp
 *
 *  Created on: 13.10.2019
 *      Author: denis
 */

#ifndef WIRINGPISTUB_INCLUDE_WIRINGPI_H_
#define WIRINGPISTUB_INCLUDE_WIRINGPI_H_

// C doesn't have true/false by default and I can never remember which
//	way round they are, so ...
//	(and yes, I know about stdbool.h but I like capitals for these and I'm old)

#ifndef	TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(!TRUE)
#endif

// GCC warning suppressor

#define	UNU	__attribute__((unused))

// Mask for the bottom 64 pins which belong to the Raspberry Pi
//	The others are available for the other devices

#define	PI_GPIO_MASK	(0xFFFFFFC0)

// Handy defines

// wiringPi modes

#define	WPI_MODE_PINS		 0
#define	WPI_MODE_GPIO		 1
#define	WPI_MODE_GPIO_SYS	 2
#define	WPI_MODE_PHYS		 3
#define	WPI_MODE_PIFACE		 4
#define	WPI_MODE_UNINITIALISED	-1

// Pin modes

#define	INPUT			 0
#define	OUTPUT			 1
#define	PWM_OUTPUT		 2
#define	GPIO_CLOCK		 3
#define	SOFT_PWM_OUTPUT		 4
#define	SOFT_TONE_OUTPUT	 5
#define	PWM_TONE_OUTPUT		 6

#define	LOW			 0
#define	HIGH			 1

// Pull up/down/none

#define	PUD_OFF			 0
#define	PUD_DOWN		 1
#define	PUD_UP			 2

// PWM

#define	PWM_MODE_MS		0
#define	PWM_MODE_BAL		1

// Interrupt levels

#define	INT_EDGE_SETUP		0
#define	INT_EDGE_FALLING	1
#define	INT_EDGE_RISING		2
#define	INT_EDGE_BOTH		3

// Pi model types and version numbers
//	Intended for the GPIO program Use at your own risk.

#define	PI_MODEL_A		 0
#define	PI_MODEL_B		 1
#define	PI_MODEL_AP		 2
#define	PI_MODEL_BP		 3
#define	PI_MODEL_2		 4
#define	PI_ALPHA		 5
#define	PI_MODEL_CM		 6
#define	PI_MODEL_07		 7
#define	PI_MODEL_3		 8
#define	PI_MODEL_ZERO		 9
#define	PI_MODEL_CM3		10
#define	PI_MODEL_ZERO_W		12
#define	PI_MODEL_3P 		13

#define	PI_VERSION_1		0
#define	PI_VERSION_1_1		1
#define	PI_VERSION_1_2		2
#define	PI_VERSION_2		3

#define	PI_MAKER_SONY		0
#define	PI_MAKER_EGOMAN		1
#define	PI_MAKER_EMBEST		2
#define	PI_MAKER_UNKNOWN	3

//	Intended for the GPIO program Use at your own risk.

// Threads

#define	PI_THREAD(X)	void *X (UNU void *dummy)

// Failure modes

#define	WPI_FATAL	(1==1)
#define	WPI_ALMOST	(1==2)

// wiringPiNodeStruct:
//	This describes additional device nodes in the extended wiringPi
//	2.0 scheme of things.
//	It's a simple linked list for now, but will hopefully migrate to
//	a binary tree for efficiency reasons - but then again, the chances
//	of more than 1 or 2 devices being added are fairly slim, so who
//	knows....

struct wiringPiNodeStruct
{
	int pinBase;
	int pinMax;

	int fd;	// Node specific
	unsigned int data0;	//  ditto
	unsigned int data1;	//  ditto
	unsigned int data2;	//  ditto
	unsigned int data3;	//  ditto

	void (*pinMode)(struct wiringPiNodeStruct *node, int pin, int /*mode*/);
	void (*pullUpDnControl)(struct wiringPiNodeStruct *node, int /*pin*/, int /*mode*/);
	int (*digitalRead)(struct wiringPiNodeStruct *node, int /*pin*/);
	//unsigned int    (*digitalRead8)     (struct wiringPiNodeStruct *node, int pin) ;
	void (*digitalWrite)(struct wiringPiNodeStruct *node, int /*pin*/, int /*value*/);
	//         void   (*digitalWrite8)    (struct wiringPiNodeStruct *node, int pin, int value) ;
	void (*pwmWrite)(struct wiringPiNodeStruct *node, int /*pin*/, int /*value*/);
	int (*analogRead)(struct wiringPiNodeStruct *node, int /*pin*/);
	void (*analogWrite)(struct wiringPiNodeStruct *node, int /*pin*/, int /*value*/);

	struct wiringPiNodeStruct *next;
};

//struct wiringPiNodeStruct *wiringPiNodes;

// Data

// Internal

inline int wiringPiFailure(int /*fatal*/, const char /**message*/, ...) { return 0; }

// Core wiringPi functions

inline struct wiringPiNodeStruct* wiringPiFindNode(int /*pin*/) { return nullptr; }
inline struct wiringPiNodeStruct* wiringPiNewNode(int /*pinBase*/, int /*numPins*/) { return nullptr; }

inline void wiringPiVersion(int /**major*/, int /**minor*/) {}
inline int wiringPiSetup(void) { return 0; }
inline int wiringPiSetupSys(void) { return 0; }
inline int wiringPiSetupGpio(void) { return 0; }
inline int wiringPiSetupPhys(void) { return 0; }

inline void pinModeAlt(int /*pin*/, int /*mode*/) {}
inline void pinMode(int /*pin*/, int /*mode*/) {}
inline void pullUpDnControl(int /*pin*/, int /*pud*/) {}
inline int digitalRead(int /*pin*/) { return 0; }
inline void digitalWrite(int /*pin*/, int /*value*/) {}
inline unsigned int digitalRead8(int /*pin*/) { return 0; }
inline void digitalWrite8(int /*pin*/, int /*value*/) {}
inline void pwmWrite(int /*pin*/, int /*value*/) {}
inline int analogRead(int /*pin*/) { return 0; }
inline void analogWrite(int /*pin*/, int /*value*/) {}

// PiFace specifics
//	(Deprecated)

inline int wiringPiSetupPiFace(void) { return 0; }
inline int wiringPiSetupPiFaceForGpioProg(void) { return 0; }// Don't use this - for gpio program only

// On-Board Raspberry Pi hardware specific stuff

inline int piGpioLayout(void) { return 0; }
inline int piBoardRev(void) { return 0; }	// Deprecated
inline void piBoardId(int /**model*/, int /**rev*/, int /**mem*/, int /**maker*/, int /**overVolted*/) {}
inline int wpiPinToGpio(int /*wpiPin*/) { return 0; }
inline int physPinToGpio(int /*physPin*/) { return 0; }
inline void setPadDrive(int /*group*/, int /*value*/);
inline int getAlt(int /*pin*/) { return 0; }
inline void pwmToneWrite(int /*pin*/, int /*freq*/) {}
inline void pwmSetMode(int /*mode*/) {}
inline void pwmSetRange(unsigned int /*range*/) {}
inline void pwmSetClock(int /*divisor*/) {}
inline void gpioClockSet(int /*pin*/, int /*freq*/) {}
inline unsigned int digitalReadByte(void) { return 0; }
inline unsigned int digitalReadByte2(void) { return 0; }
inline void digitalWriteByte(int /*value*/) {}
inline void digitalWriteByte2(int /*value*/) {}

// Interrupts
//	(Also Pi hardware specific)

inline int waitForInterrupt(int /*pin*/, int /*mS*/) { return 0; }
inline int wiringPiISR(int /*pin*/, int /*mode*/, void* /*(*function)(void)*/) { return 0; }

// Threads

inline int piThreadCreate(void* /*(*fn)(void*)*/) { return 0; }
inline void piLock(int /*key*/) {}
inline void piUnlock(int /*key*/) {}

// Schedulling priority

inline int piHiPri(const int /*pri*/) { return 0; }

// Extras from arduino land

inline void delay(unsigned int /*howLong*/) {}
inline void delayMicroseconds(unsigned int /*howLong*/) {}
inline unsigned int millis(void) { return 0; }
inline unsigned int micros(void) { return 0; }

#endif /* WIRINGPISTUB_INCLUDE_WIRINGPI_H_ */
