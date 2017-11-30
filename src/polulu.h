#ifndef POLULU_H
#define POLULU_H
#include <stdio.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <stdlib.h>

#define fileLocBufSize 256
#define bufSize 256

#define CLKWE            _IO('q', 1)
#define CLKWD            _IO('q', 2)
#define CCLKWE            _IO('q', 3)
#define CCLKWD            _IO('q', 4)
#define MOTORE            _IO('q', 5)
#define MOTORD            _IO('q', 6)

#define pwmAddDevCommand_part1		"echo bone_pwm_"
#define pwmAddDevCommand_part2  	"> /sys/devices/bone_capemgr.9/slots"


#define pwmSearchDevCommand_part1	"find /sys/devices -name '*pwm_test_"
#define pwmSearchDevCommand_part2	"*'"

#define gpioSearchDevCommand_part1	"find /dev/ -name '*"
#define gpioSearchDevCommand_part2	"*'"

static long int MAX_PERIOD = 1000000000;
static long int MIN_PERIOD = 1000;


#define locationPeriod      "/period"
#define locationPolarity    "/polarity"
#define locationDuty        "/duty"


#define systemCommandLength   	128
#define stdStrLenght    128
#define pwmComBufferSize 128

#define DEBUG

struct motorControlUnit
{
    char  pwmFileLocation[stdStrLenght];
    char  gpioSupportFileLocation[stdStrLenght];
   

    int  polarityFP;
    int  dutyFP;
    int  periodFP;
    int  gpioSupportFP;

    unsigned long int duty;
    unsigned long int period;
	int polarity;
    int motorState;
    int clkState;
    int counterClkState;
};

struct motorControlUnit initMotorControlUnit(char * pwmFileLocation , char * gpioSupportFileLocation );

int addPwmDevice(char * deviceName);

int isGpioDeviceExits(char * deviceName);

int isPwmDeviceExists(char * deviceName);

int getLocationOfGpioDevice(char * deviceName , char * gpioGrpDeviceLocationRet , int gpioGrpDeviceLocationRetLim );

int getLocationOfPWMDevice(char * deviceName , char * pwmDeviceLocationRet , int pwmDeviceLocationRetLim );

int openGpio(char * gpioFileLocation);

int openPolarity(char * pwmFileLocation);

int openPeriod(char * pwmFileLocation);

int openDuty(char * pwmFileLocation);

int clockwiseEnable(struct motorControlUnit * mtrCntUnit);

int clockwiseDisable(struct motorControlUnit * mtrCntUnit);

int counterClockwiseEnable(struct motorControlUnit * mtrCntUnit);

int counterClockwiseDisable(struct motorControlUnit * mtrCntUnit);

int motorEnable(struct motorControlUnit * mtrCntUnit);

int motorDisable(struct motorControlUnit * mtrCntUnit);

int setPolarity( int polarity , struct motorControlUnit * mtrCntUnit);

int setPeriod( unsigned long int period , struct motorControlUnit * mtrCntUnit );

int setDuty( unsigned long int duty , struct motorControlUnit * mtrCntUnit );

int setDutyRatio(int ratio , struct motorControlUnit * mtrCntUnit);


#endif
