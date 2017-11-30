#include "polulu.h"

/*
*	Possible pins: [P9_22 , P9_31] , [P9_21 , P9_29] , [P9_42] , [P9_14 , P8_36]
*			[P9_16 , P8_34] , [P8_19 , P8_45] , [P8_13 , P8_46] , [P9_28]
*
*
*/
/*
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
*/
struct motorControlUnit initMotorControlUnit(char * pwmFileLocation , char * gpioSupportFileLocation )
{
    struct motorControlUnit newMotorControlUnit;
    
	newMotorControlUnit.polarityFP = -1;
    newMotorControlUnit.dutyFP = -1;
    newMotorControlUnit.periodFP = -1;
    newMotorControlUnit.gpioSupportFP = -1;

    newMotorControlUnit.duty = 0;
    newMotorControlUnit.period = 0;
	newMotorControlUnit.polarity = 0;
    newMotorControlUnit.motorState = 0;
    newMotorControlUnit.clkState = 0;
    newMotorControlUnit.counterClkState = 0;

    newMotorControlUnit.pwmFileLocation[0]='\0';
    newMotorControlUnit.gpioSupportFileLocation[0]='\0';

    snprintf(newMotorControlUnit.pwmFileLocation, sizeof newMotorControlUnit.pwmFileLocation , "%s", pwmFileLocation);

    snprintf(newMotorControlUnit.gpioSupportFileLocation, sizeof newMotorControlUnit.gpioSupportFileLocation , "%s", gpioSupportFileLocation);

    #ifdef DEBUG
	printf("DEBUG:initMotorControlUnit: NEW MOTOR CONTROL UNIT:\npwm loc:%s\ngpio loc:%s\n",
    newMotorControlUnit.pwmFileLocation , 
    newMotorControlUnit.gpioSupportFileLocation);
	#endif

    newMotorControlUnit.periodFP = openPeriod(newMotorControlUnit.pwmFileLocation);
    newMotorControlUnit.dutyFP = openDuty(newMotorControlUnit.pwmFileLocation);
    newMotorControlUnit.polarityFP = openPolarity(newMotorControlUnit.pwmFileLocation);
	newMotorControlUnit.gpioSupportFP = openGpio(newMotorControlUnit.gpioSupportFileLocation);
    
	if((newMotorControlUnit.periodFP<0)||(newMotorControlUnit.dutyFP<0)||(newMotorControlUnit.polarityFP<0)||(newMotorControlUnit.gpioSupportFP<0))
    {
        if(newMotorControlUnit.periodFP<0)
        {
        printf("ERROR: PERIOD file did not open\n");
        }
        else{close(newMotorControlUnit.periodFP);}
        

        if(newMotorControlUnit.dutyFP<0)
        {
        printf("ERROR: DUTY file did not open\n");
        }
        else{close(newMotorControlUnit.dutyFP);}

        if(newMotorControlUnit.polarityFP<0)
        {
        printf("ERROR: POLARITY file did not open\n");
        }
        else{close(newMotorControlUnit.polarityFP);}
		
		if(newMotorControlUnit.gpioSupportFP<0)
        {
        printf("ERROR: GPIO file did not open\n");
        }
        else{close(newMotorControlUnit.gpioSupportFP);}		
	
		printf("ERROR: Exiting\n");
    	exit(-1);
    }
    
	return newMotorControlUnit;
	


}

int addPwmDevice(char * deviceName)
{

	if(isPwmDeviceExists(deviceName))
	{
		printf("Attention:addPwmDevice:Device exists(pre check).\n");
		
		return 1;
	}
	
	char addSystemCom[systemCommandLength];

	snprintf(addSystemCom, sizeof addSystemCom , "%s%s%s", pwmAddDevCommand_part1 , deviceName , pwmAddDevCommand_part2);
	
	system(addSystemCom);

	#ifdef DEBUG
	printf("DEBUG:addPwmDevice:%s\n",addSystemCom);
	#endif

	if(isPwmDeviceExists(deviceName))
	{
		#ifdef DEBUG
		printf("DEBUG:addPwmDevice:PWM device added.\n");
		#endif
		return 1;
	}	
	else
	{	
		#ifdef DEBUG
		printf("DEBUG:addPwmDevice:PWM device didn't add.\n");
		#endif
		return 0; 
	}
}

int isGpioDeviceExits(char * deviceName)
{

	char locationString[stdStrLenght];
	locationString[0]='\0';
	getLocationOfGpioDevice( deviceName , locationString , stdStrLenght);	
   	
    	
	
	if(locationString[0] == '\0')
	{
		printf("Gpio device: %s , no file! \n",deviceName);
		return 0;
	}

	#ifdef DEBUG
	printf("DEBUG:isGpioDeviceExits: device exists , location: %s\nDEBUG:isGpioDeviceExits:PWM device exists.\n",locationString);
	#endif

	return 1;
}

int isPwmDeviceExists(char * deviceName)
{
	
    char locationString[stdStrLenght];
	locationString[0]='\0';
	getLocationOfPWMDevice( deviceName , locationString , stdStrLenght);	
   	
    	
	
	if(locationString[0] == '\0')
	{
		printf("PWM pin: %s , no pwm file! \n",deviceName);
		return 0;
	}

	#ifdef DEBUG
	printf("DEBUG:isPwmDeviceExists: device exists , location: %s\nDEBUG:isPwmDeviceExists:PWM device exists.\n",locationString);
	#endif

    	return 1;
}

int getLocationOfGpioDevice(char * deviceName , char * gpioGrpDeviceLocationRet , int gpioGrpDeviceLocationRetLim )
{
	char gpioGrpFindSystemCommand[systemCommandLength];
	gpioGrpFindSystemCommand[0]='\0';

	//snprintf(gpioGrpFindSystemCommand, sizeof gpioGrpFindSystemCommand , "%s%s", gpioDeviceLocation ,  deviceName );
	snprintf(gpioGrpFindSystemCommand, sizeof gpioGrpFindSystemCommand , "%s%s%s", gpioSearchDevCommand_part1 ,  deviceName , gpioSearchDevCommand_part2);

	FILE* file = popen(gpioGrpFindSystemCommand , "r");
	if(file==NULL)
	{
		printf("ERROR:Gpio grp check system command failure.");
		return 0;
	}	

	#ifdef DEBUG
	printf("DEBUG:getLocationOfGpioDevice:%s\nDEBUG:getLocationOfGpioDevice: ->popen of pwm find worked\n",gpioGrpFindSystemCommand);
	#endif

    
   	fscanf(file, "%s", gpioGrpDeviceLocationRet);
	//fread(gpioGrpDeviceLocationRet, gpioGrpDeviceLocationRetLim, 1, file);
	
	pclose(file);
	
	#ifdef DEBUG
	printf("DEBUG:getLocationOfGpioDevice:%s\n",gpioGrpFindSystemCommand);
	printf("DEBUG:getLocationOfGpioDevice: returned string %s	\n",gpioGrpDeviceLocationRet);
	#endif

	return 1;

}

int getLocationOfPWMDevice(char * deviceName , char * pwmDeviceLocationRet , int pwmDeviceLocationRetLim )
{
	char pwmFindSystemCommand[systemCommandLength];
	pwmFindSystemCommand[0]='\0';

	//snprintf(pwmFindSystemCommand, sizeof pwmFindSystemCommand , "%s%s%s", "find /sys/devices -name '*pwm_test_" ,  deviceName , "*'");
	snprintf(pwmFindSystemCommand, sizeof pwmFindSystemCommand , "%s%s%s", pwmSearchDevCommand_part1 ,  deviceName , pwmSearchDevCommand_part2);

	FILE* file = popen(pwmFindSystemCommand , "r");
	if(file==NULL)
	{
		printf("ERROR:PWM check system command failure.");
		return 0;
	}

	#ifdef DEBUG
	printf("DEBUG:getLocationOfPWMDevice:%s\nDEBUG:getLocationOfPWMDevice: ->popen of pwm find worked\n",pwmFindSystemCommand);
	#endif

    
   	fscanf(file, "%s", pwmDeviceLocationRet);
	//fread(pwmDeviceLocationRet, pwmDeviceLocationRetLim, 1, file);
	
	pclose(file);
	
	#ifdef DEBUG
	printf("DEBUG:getLocationOfPWMDevice:%s\nDEBUG:getLocationOfPWMDevice: ->popen of pwm find worked\n",pwmFindSystemCommand);
	printf("DEBUG:getLocationOfPWMDevice: returned string %s\n",pwmDeviceLocationRet);
	#endif

	return 1;
}

int openGpio(char * gpioFileLocation)
{

	char gpioFileLocationSafe[stdStrLenght];
    snprintf(gpioFileLocationSafe, sizeof gpioFileLocationSafe , "%s", gpioFileLocation ); 
	int gpioFP = open( gpioFileLocationSafe , O_RDWR );
	#ifdef DEBUG
	printf("DEBUG:openGpio: gpioFP: %d\n",gpioFP);
	#endif
    return gpioFP; 
}

int openPolarity(char * pwmFileLocation)
{
    char pwmPolarityFileLocation[stdStrLenght];
    snprintf(pwmPolarityFileLocation, sizeof pwmPolarityFileLocation , "%s%s", pwmFileLocation , locationPolarity); 
    #ifdef DEBUG
	printf("DEBUG:openPolarity:Open (polarity) %s + %s -> %s\n" , pwmFileLocation , locationDuty , pwmPolarityFileLocation);
	#endif
	int polarityFP = open( pwmPolarityFileLocation , O_RDWR );
	#ifdef DEBUG
	printf("DEBUG:openPolarity: polarityFP: %d\n",polarityFP);
	#endif
    return polarityFP; 
	
}

int openPeriod(char * pwmFileLocation)
{
    char pwmPeriodFileLocation[stdStrLenght]; 
    snprintf(pwmPeriodFileLocation, sizeof pwmPeriodFileLocation , "%s%s", pwmFileLocation , locationPeriod);
    #ifdef DEBUG
	printf("DEBUG:openPeriod:Open (period) %s + %s -> %s\n" , pwmFileLocation , locationDuty ,pwmPeriodFileLocation);
	#endif
	int periodFP = open( pwmPeriodFileLocation , O_RDWR );
	#ifdef DEBUG
	printf("DEBUG:openPeriod: periodFP: %d\n",periodFP);
	#endif
	return periodFP;
}

int openDuty(char * pwmFileLocation)
{
    char pwmDutyFileLocation[stdStrLenght]; 
    snprintf(pwmDutyFileLocation, sizeof pwmDutyFileLocation , "%s%s", pwmFileLocation ,  locationDuty);
    #ifdef DEBUG
	printf("DEBUG:openDuty:Open (duty) %s + %s -> %s\n" , pwmFileLocation , locationDuty , pwmDutyFileLocation);
	#endif
	int dutyFP = open( pwmDutyFileLocation , O_RDWR );
	#ifdef DEBUG
	printf("DEBUG:openDuty: dutyFP: %d\n",dutyFP);
	#endif
	return dutyFP;
}


int clockwiseEnable(struct motorControlUnit * mtrCntUnit)
{
	if(mtrCntUnit->gpioSupportFP>=0)
	{
		
		int retVal = ioctl(mtrCntUnit->gpioSupportFP, CLKWE);
		
		if(retVal==1)
		{
			mtrCntUnit->clkState = 1;
			#ifdef DEBUG		
			printf("DEBUG:clockwiseEnable: %s \n", mtrCntUnit->gpioSupportFileLocation);
			#endif	
			return 1;	
		}		
		else
		{
			#ifdef DEBUG	
			printf("DEBUG:clockwiseEnable: mtrCntUnit->gpioSupportFP= %d \n", mtrCntUnit->gpioSupportFP);
			printf("DEBUG:clockwiseEnable: retVal = %d \n", retVal);
			#endif
			printf("ERROR:clockwiseEnable: Gpio driver did not recognised the CLKWE massege!\n");
			return 0;
		}	
	}	
	else
	{
		printf("ERROR:clockwiseEnable: Gpio file pointer is unsuitable\n");
		return 0;
	}
		
}
int clockwiseDisable(struct motorControlUnit * mtrCntUnit)
{

	if(mtrCntUnit->gpioSupportFP>=0)
	{
		
		int retVal = ioctl(mtrCntUnit->gpioSupportFP, CLKWD);
		
		if(retVal==1)
		{
			mtrCntUnit->clkState = 0;
			#ifdef DEBUG		
			printf("DEBUG:clockwiseDisable: %s \n", mtrCntUnit->gpioSupportFileLocation);
			#endif	
			return 1;	
		}		
		else
		{
			#ifdef DEBUG	
			printf("DEBUG:clockwiseDisable: mtrCntUnit->gpioSupportFP= %d \n", mtrCntUnit->gpioSupportFP);
			printf("DEBUG:clockwiseDisable: retVal = %d \n", retVal);
			#endif
			printf("ERROR:clockwiseDisable: Gpio driver did not recognised the CLKWD massege!\n");
			return 0;
		}	
	}	
	else
	{
		printf("ERROR:clockwiseDisable: Gpio file pointer is unsuitable\n");
		return 0;
	}

}
int counterClockwiseEnable(struct motorControlUnit * mtrCntUnit)
{
	if(mtrCntUnit->gpioSupportFP>=0)
	{
		
		int retVal = ioctl(mtrCntUnit->gpioSupportFP, CCLKWE);
		
		if(retVal==1)
		{
			mtrCntUnit->counterClkState = 1;
			#ifdef DEBUG		
			printf("DEBUG:counterClockwiseEnable: %s \n", mtrCntUnit->gpioSupportFileLocation);
			#endif	
			return 1;	
		}		
		else
		{
			#ifdef DEBUG	
			printf("DEBUG:counterClockwiseEnable: mtrCntUnit->gpioSupportFP= %d \n", mtrCntUnit->gpioSupportFP);
			printf("DEBUG:counterClockwiseEnable: retVal = %d \n", retVal);
			#endif
			printf("ERROR:counterClockwiseEnable: Gpio driver did not recognised the CCLKWE massege!\n");
			return 0;
		}	
	}	
	else
	{
		printf("ERROR:counterClockwiseEnable : Gpio file pointer is unsuitable\n");
		return 0;
	}
	
}
int counterClockwiseDisable(struct motorControlUnit * mtrCntUnit)
{

	if(mtrCntUnit->gpioSupportFP>=0)
	{
		
		int retVal = ioctl(mtrCntUnit->gpioSupportFP, CCLKWD);
		
		if(retVal==1)
		{
			mtrCntUnit->counterClkState = 0;
			#ifdef DEBUG		
			printf("DEBUG:counterClockwiseDisable: %s \n", mtrCntUnit->gpioSupportFileLocation);
			#endif	
			return 1;	
		}		
		else
		{
			#ifdef DEBUG	
			printf("DEBUG:counterClockwiseDisable: mtrCntUnit->gpioSupportFP= %d \n", mtrCntUnit->gpioSupportFP);
			printf("DEBUG:counterClockwiseDisable: retVal = %d \n", retVal);
			#endif
			printf("ERROR:counterClockwiseDisable: Gpio driver did not recognised the CCLKWE massege!\n");
			return 0;
		}	
	}	
	else
	{
		printf("ERROR:counterClockwiseDisable : Gpio file pointer is unsuitable\n");
		return 0;
	}
}
int motorEnable(struct motorControlUnit * mtrCntUnit)
{

	if(mtrCntUnit->gpioSupportFP>=0)
	{
		int retVal = ioctl(mtrCntUnit->gpioSupportFP, MOTORE);
				
		
		
		if(retVal==1)
		{
			mtrCntUnit->motorState = 1;
			#ifdef DEBUG		
			printf("DEBUG:motorEnable: %s \n", mtrCntUnit->gpioSupportFileLocation);
			#endif	
			return 1;	
		}		
		else
		{
			#ifdef DEBUG	
			printf("DEBUG:motorEnable: mtrCntUnit->gpioSupportFP= %d \n", mtrCntUnit->gpioSupportFP);
			printf("DEBUG:motorEnable: retVal = %d \n", retVal);
			#endif
			printf("ERROR:motorEnable: Gpio driver did not recognised the MOTORE massege!\n");
			return 0;
		}	
	}	
	else
	{
		printf("ERROR:motorEnable: Gpio file pointer is unsuitable\n");
		return 0;
	}

}
int motorDisable(struct motorControlUnit * mtrCntUnit)
{

	if(mtrCntUnit->gpioSupportFP>=0)
	{
		int retVal = ioctl(mtrCntUnit->gpioSupportFP, MOTORD);
		
		if(retVal==1)
		{
			mtrCntUnit->motorState = 0;
			#ifdef DEBUG		
			printf("DEBUG:motorDisable: %s \n", mtrCntUnit->gpioSupportFileLocation);
			#endif	
			return 1;	
		}		
		else
		{
			#ifdef DEBUG	
			printf("DEBUG:motorDisable: mtrCntUnit->gpioSupportFP= %d \n", mtrCntUnit->gpioSupportFP);
			printf("DEBUG:motorDisable: retVal = %d \n", retVal);
			#endif
			printf("ERROR:motorDisable: Gpio driver did not recognised the MOTORD massege!\n");
			return 0;
		}	
	}	
	else
	{
		printf("ERROR:motorDisable: Gpio file pointer is unsuitable\n");
		return 0;
	}
}

int setPolarity( int polarity , struct motorControlUnit * mtrCntUnit)
{
	if((polarity==0)||(polarity==1))
	{
		if(mtrCntUnit->polarityFP>=0)
		{
			char convertString[pwmComBufferSize];
			snprintf(convertString, sizeof convertString, "%d", polarity);
			write(mtrCntUnit->polarityFP,convertString,sizeof convertString);
			mtrCntUnit->polarity = polarity;
			#ifdef DEBUG	
			printf("DEBUG:setPolarity: mtrCntUnit->polarityFP= %d \n", mtrCntUnit->polarityFP);
			printf("DEBUG:setPolarity: requested polarity = %d \n", polarity);
			printf("DEBUG:setPolarity: set polarity = %s \n", convertString);
			#endif	
			return 1;	
		}	
		else
		{
			printf("ERROR:setPolarity: Gpio file pointer is unsuitable\n");
			return 0;
		}
	}
	else
	{
		printf("ATTENTION:setPolarity: unsuitable polarity value passed!\n");
		return 0;
	}
}
int setPeriod( unsigned long int period , struct motorControlUnit * mtrCntUnit )
{
	if((period<=MAX_PERIOD)&&(period>=MIN_PERIOD))
	{
		if(mtrCntUnit->periodFP>=0)
		{
			char convertString[pwmComBufferSize];
			snprintf(convertString, sizeof convertString, "%lu", period);
			write(mtrCntUnit->polarityFP,convertString,sizeof convertString);
			mtrCntUnit->period = period;
			#ifdef DEBUG	
			printf("DEBUG:setPeriod: mtrCntUnit->periodFP= %d \n", mtrCntUnit->polarityFP);
			printf("DEBUG:setPeriod: requested period = %lu \n", period);
			printf("DEBUG:setPeriod: set period = %s \n", convertString);
			printf("DEBUG:setPeriod: set period (structure val)= %lu \n", mtrCntUnit->period);
			#endif
			return 1;		
		}	
		else
		{
			printf("ERROR:setPolarity: Gpio file pointer is unsuitable\n");
			return 0;
		}
	}
	else
	{
		printf("ATTENTION:setPeriod: unsuitable period value passed!\n");
		return 0;
	}
}
int setDuty( unsigned long int duty , struct motorControlUnit * mtrCntUnit )
{
	if((duty<=mtrCntUnit->period)&&(duty>=0))
	{
		if(mtrCntUnit->dutyFP>=0)
		{
			char convertString[pwmComBufferSize];
			snprintf(convertString, sizeof convertString, "%lu", duty);
			write(mtrCntUnit->polarityFP,convertString,sizeof convertString);
			mtrCntUnit->duty = duty;
			#ifdef DEBUG	
			printf("DEBUG:setDuty: mtrCntUnit->dutyFP= %d \n", mtrCntUnit->polarityFP);
			printf("DEBUG:setDuty: requested duty = %lu \n", duty);
			printf("DEBUG:setDuty: set duty = %s \n", convertString);
			#endif
			return 1;		
		}	
		else
		{
			printf("ERROR:setPolarity: Gpio file pointer is unsuitable\n");
			return 0;
		}
	}
	else
	{
		printf("ATTENTION:setPeriod: unsuitable duty value passed!\n");
		return 0;
	}
}

int setDutyRatio(int ratio , struct motorControlUnit * mtrCntUnit)
{

	if((ratio<=100)&&(ratio>=0))
	{

		if(mtrCntUnit->period != 0)	
		{		
			float ratioVal=	(float)ratio/100;
			unsigned long int period = mtrCntUnit->period;
			unsigned long int dutyVal = period * ratioVal;
			int retVal = setDuty( dutyVal , mtrCntUnit );
			#ifdef DEBUG	
			printf("DEBUG:setDutyRatio: mtrCntUnit location %s \n", mtrCntUnit->pwmFileLocation);
			printf("DEBUG:setDutyRatio: period value = %lu \n", period);
			printf("DEBUG:setDutyRatio: period value(structure) = %lu \n", mtrCntUnit->period);
			printf("DEBUG:setDutyRatio: requested ratio = %d \n", ratio);
			printf("DEBUG:setDutyRatio: requested ratio val = %f \n", ratioVal);
			printf("DEBUG:setDutyRatio: calculated duty value = %lu \n", dutyVal);
			if(retVal == 1)
				printf("DEBUG:setDutyRatio: duty ratio set succesfully \n");
			else
				printf("DEBUG:setDutyRatio: duty ratio setting failed \n");
			#endif
			return retVal;
			
			
		}
		else
		{
			printf("ATTENTION:Set period fist!");
			return 0;
		}
	}	
	else
	{
		printf("ATTENTION:setDutyRatio: unsuitable ratio value requested!\n");
		return 0;
	}

 
}


