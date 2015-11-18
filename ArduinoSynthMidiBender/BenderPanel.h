//Header
#ifndef BenderPanel_H_INCLUDED
#define BenderPanel_H_INCLUDED


#include "stdint.h"
#include "timeKeeper.h"
#include "PanelComponents.h"
#include "Panel.h"
#include "flagMessaging.h"
#include "BenderSettings.h"

enum PStates
{
	PInit,
	PIdle,
	PNewStatus,
	PNewSelector,

};

class BenderPanel : public Panel
{
public:
	BenderPanel( void );
	void setTxLed( void );
	ledState_t serviceTxLed( void );
	void setRxLed( void );
	ledState_t serviceRxLed( void );
	
	void reset( void );
	
	//State machine stuff  
	void processMachine( void );
	void tickStateMachine( void );

	void timersMIncrement( uint8_t );
	
	//Flags coming in from the system

	uint8_t tapLedFlag;
	uint8_t syncLedFlag;
	uint8_t rxLedFlag;
	uint8_t txLedFlag;
	
	//Internal - and going out to the system - Flags
	//MessagingFlag clearSongFlag;
	
	//  ..and data.
	uint8_t displayMode;
	uint8_t selectorPosition;
	uint8_t leftKnobPosition;
	uint8_t rightKnobPosition;
	
	uint8_t messagePassThroughTable[16];
	
private:
	//Internal Flags
	//  ..and data
	MessagingFlag newStatusFlag;
	MessagingFlag newLeftKnob;
	MessagingFlag newRightKnob;
	
	//State machine stuff  
	PStates state;
	DeviceSettings settings;
	
};

#endif