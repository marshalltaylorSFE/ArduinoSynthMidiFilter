#ifndef BENDERSETTINGS_H
#define BENDERSETTINGS_H

#include "stdint.h"

struct statusByteSettings
{
	uint8_t modifableMask;
	uint8_t modifiedMask;
	uint8_t value1;
	uint8_t value2;
};

class DeviceSettings
{
	DeviceSettings( void );
public:
	statusByteSettings stat8;
	statusByteSettings stat9;
	statusByteSettings statA;
	statusByteSettings statB;
	statusByteSettings statC;
	statusByteSettings statD;
	statusByteSettings statE;
	statusByteSettings statF;
}

#endif