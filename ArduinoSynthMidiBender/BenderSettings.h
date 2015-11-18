#ifndef BENDERSETTINGS_H
#define BENDERSETTINGS_H

#include "stdint.h"

struct statusByteSettings
{
	uint8_t modifiableMask;
	uint8_t modifiedMask;
	uint8_t value1;
	uint8_t value2;
};

class DeviceSettings
{
public:
	DeviceSettings( void );
	statusByteSettings stat8;
	statusByteSettings stat9;
	statusByteSettings statA;
	statusByteSettings statB;
	statusByteSettings statC;
	statusByteSettings statD;
	statusByteSettings statE;
	statusByteSettings statF;
	statusByteSettings statGlobal;
	uint8_t selectedSetting;
};

#endif