#include "BenderSettings.h"

DeviceSettings::DeviceSettings( void )
{
	inputAcceptChannel = 1;
	outputMergeChannel = 1;
	currentStatus = 0x00;  //Hardcode default
	editing = 0;
	
	velocitySetting = VELO_OFF;
	maxVelocity = 127;
	minVelocity = 0;
	fixedVelocity = 100;
	
	channelOpEdited = 0;
	inputChannelSetting = IN_PASS;
	outputChannelSetting = OUT_PASS;
	inputChannel = 0;
	outputChannel = 0;
	
	upperOutputChannel = 0; //Used for split
	splitPoint = 40;
	splitMode = SPLIT_OFF;
	upperOctave = 0;
	splitEdited = 0;
	
	statusBlockBits = 0xFC;
	statusFilterEnabled = 1;
}
