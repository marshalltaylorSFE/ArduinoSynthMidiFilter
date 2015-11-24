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
    statGlobal.modifiableMask = 0x37;
	
	stat8.modifiedMask = 0;
	stat9.modifiedMask = 0;
	statA.modifiedMask = 0;
	statB.modifiedMask = 0;
	statC.modifiedMask = 0;
	statD.modifiedMask = 0;
	statE.modifiedMask = 0;
	statF.modifiedMask = 0;	
	statGlobal.modifiedMask = 0;

	currentStatus = 0x10;  //Hardcode default
	
}

void DeviceSettings::setCurrentStatus( uint8_t input )
{
	currentStatus = input;
	switch( currentStatus )  //Build lookup
	{
		case 8:  // status byte = 8h
			currentStatusSettingsPtr = &stat8;
		break;
		case 9:  //
			currentStatusSettingsPtr = &stat9;
		break;
		case 10:  //
			currentStatusSettingsPtr = &statA;
		break;
		case 11:  //
			currentStatusSettingsPtr = &statB;
		break;
		case 12:  //
			currentStatusSettingsPtr = &statC;
		break;
		case 13:  //
			currentStatusSettingsPtr = &statD;
		break;
		case 14:  //
			currentStatusSettingsPtr = &statE;
		break;
		case 15:  //
			currentStatusSettingsPtr = &statF;
		break;
		case 16:  // Global
			currentStatusSettingsPtr = &statGlobal;
		break;
		default:  //
			currentStatusSettingsPtr = &statGlobal;
		break;
	}
}

statusByteSettings * DeviceSettings::getCurrentStatusPtr( void )
{
	return currentStatusSettingsPtr;
}

uint8_t DeviceSettings::getCurrentStatus( void )
{
	return currentStatus;
}