#include "BenderSettings.h"

DeviceSettings::DeviceSettings( void )
{
	stat8.modifiableMask = 0x1F;
	stat9.modifiableMask = 0x1F;
	statA.modifiableMask = 0x07;
	statB.modifiableMask = 0x07;
	statC.modifiableMask = 0x07;
	statD.modifiableMask = 0x07;
	statE.modifiableMask = 0x07;
	statF.modifiableMask = 0x27;
    globalstat.modifiableMask = 0x37;
	
	stat8.modifiedMask = 0;
	stat9.modifiedMask = 0;
	statA.modifiedMask = 0;
	statB.modifiedMask = 0;
	statC.modifiedMask = 0;
	statD.modifiedMask = 0;
	statE.modifiedMask = 0;
	statF.modifiedMask = 0;	
	globalstat.modifiedMask = 0;
	
}