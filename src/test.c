#include "polulu.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	
	char deviceFileLocation[stdStrLenght];
	char test[15] = "P9_14";
	
	addPwmDevice(test );
	
	getLocationOfPWMDevice( test , deviceFileLocation , stdStrLenght );
	printf("--%s--\n",deviceFileLocation);

	char testPinControlGrpName[100]="polulu_pin_control_group_1";
	char gpioDeviceFileLocation[stdStrLenght];
	
	getLocationOfGpioDevice( testPinControlGrpName , gpioDeviceFileLocation , stdStrLenght );
	
    struct motorControlUnit mtrCntUnit= initMotorControlUnit( deviceFileLocation , gpioDeviceFileLocation );
	
	printf("Enable the motor\n");
	motorEnable(&mtrCntUnit);	
	sleep(1);

	setPeriod(1000000,&mtrCntUnit);
	setDutyRatio(50,&mtrCntUnit);
	
	//printf("period = %lu\n",mtrCntUnit.period);

	close(mtrCntUnit.gpioSupportFP);
   	close(mtrCntUnit.periodFP);
   	close(mtrCntUnit.dutyFP);
	close(mtrCntUnit.polarityFP);
}
