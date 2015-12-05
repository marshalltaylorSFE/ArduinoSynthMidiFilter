#include "BenderSettings.h"

DeviceSettings::DeviceSettings( void )
{
	inputAcceptChannel = 1;
	outputMergeChannel = 1;
	statusPassEnable = 0xFF;
	maxVelocity = 127;
	minVelocity = 0;
	currentStatus = 0x00;  //Hardcode default
	editing = 0;
	
	velocitySetting = VELO_OFF;
	inputChannelSetting = IN_PASS;
	outputChannelSetting = OUT_PASS;
	
	statusPassEnable = 0xFC;
	
}
