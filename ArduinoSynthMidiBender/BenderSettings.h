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

class DeviceSettings
{
public:
	DeviceSettings( void );
	void setCurrentStatus( uint8_t );
	uint8_t getCurrentStatus( void );
	
	uint8_t inputAcceptChannel;
	uint8_t outputMergeChannel;
	uint8_t status8PassEnable : 1;
	uint8_t status9PassEnable : 1;
	uint8_t statusAPassEnable : 1;
	uint8_t statusBPassEnable : 1;
	uint8_t statusCPassEnable : 1;
	uint8_t statusDPassEnable : 1;
	uint8_t statusEPassEnable : 1;
	uint8_t statusFPassEnable : 1;
	uint8_t :0;
	uint8_t maxVelocity;
	uint8_t minVelocity;
	
	uint8_t currentStatus;
	uint8_t editing;
	
	VelocityValues velocitySetting;
	InputChannelValues inputChannelSetting;
	OutputChannelValues outputChannelSetting;

};

#endif