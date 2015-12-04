#include "BenderSettings.h"

DeviceSettings::DeviceSettings( void )
{
	inputAcceptChannel = 1;
	outputMergeChannel = 1;
	status8PassEnable = 1;
	status9PassEnable = 1;
	statusAPassEnable = 1;
	statusBPassEnable = 1;
	statusCPassEnable = 1;
	statusDPassEnable = 1;
	statusEPassEnable = 1;
	statusFPassEnable = 1;
	maxVelocity = 127;
	minVelocity = 0;
	currentStatus = 0x10;  //Hardcode default
	editing = 0;
	
	velocitySetting = VELO_OFF;
	inputChannelSetting = IN_PASS;
	outputChannelSetting = OUT_PASS;
	
}

uint8_t DeviceSettings::getCurrentStatus( void )
{
	return currentStatus;
}