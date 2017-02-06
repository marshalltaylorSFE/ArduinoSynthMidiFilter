#ifndef BENDERSETTINGS_H
#define BENDERSETTINGS_H

#include "stdint.h"

enum VelocityValues
{
	VELO_OFF,
	VELO_FIXED,
	VELO_SCALED
};

enum InputChannelValues
{
	IN_PASS,
	IN_ONLY
};

enum OutputChannelValues
{
	OUT_PASS,
	OUT_MERGE
};

enum SplitModeValues
{
	SPLIT_OFF,
	SPLIT_NORMAL,
	SPLIT_CHROUS_TOP,
	SPLIT_CHROUS_BOTTOM
};
class DeviceSettings
{
public:
	DeviceSettings( void );
	void setCurrentStatus( uint8_t );
	
	uint8_t inputAcceptChannel;
	uint8_t outputMergeChannel;
	uint8_t statusBlockBits;
	uint8_t statusFilterEnabled;
	
	
	uint8_t maxVelocity;
	uint8_t minVelocity;
	uint8_t fixedVelocity;
	
	uint8_t currentStatus;
	uint8_t editing;
	
	VelocityValues velocitySetting;
	uint8_t channelOpEdited;
	InputChannelValues inputChannelSetting;
	OutputChannelValues outputChannelSetting;
	uint8_t inputChannel;
	uint8_t outputChannel; //Used for channel
	uint8_t upperOutputChannel; //Used for split
	uint8_t splitPoint;
	uint8_t splitEdited;
	SplitModeValues splitMode;
	int8_t upperOctave;

};

#endif