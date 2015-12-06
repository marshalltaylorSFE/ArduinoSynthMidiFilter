//**BenderPanel*******************************//
#include "BenderPanel.h"
//#include "PanelComponents.h"
#include "Panel.h"
#include "Arduino.h"
#include "flagMessaging.h"
#include "BenderSettings.h"

#define KNOBDELTADETECT 5  //5 units of 255 for knob change detect

BenderPanel::BenderPanel( void )
{
	//Controls

	txLedFlag = 0;
	rxLedFlag = 0;	
	
	state = PInit;
	
	
}

void BenderPanel::setTxLed( void )
{
	txLedFlag = 1;
}

ledState_t BenderPanel::serviceTxLed( void )
{
	ledState_t returnVar = LEDOFF;
	if( txLedFlag == 1 )
	{
		returnVar = LEDON;
		txLedFlag = 0;
		
	}
	return returnVar;
	
}

void BenderPanel::setRxLed( void )
{
	rxLedFlag = 1;
}

ledState_t BenderPanel::serviceRxLed( void )
{
	ledState_t returnVar = LEDOFF;
	if( rxLedFlag == 1 )
	{
		returnVar = LEDON;
		rxLedFlag = 0;
		
	}
	return returnVar;
	
}

void BenderPanel::reset( void )
{
	//Set explicit states
	//Set all LED off
	txLed.setState(LEDOFF);
	rxLed.setState(LEDOFF);
	option1Led.setState(LEDOFF);
	option2Led.setState(LEDOFF);
	option3Led.setState(LEDOFF);
	display.setState(SSOFF);
	
	update();
	
}

//---------------------------------------------------------------------------//
//
//  To process the machine,
//    take the inputs from the system
//    process human interaction hard-codes
//    process the state machine
//    clean up and post output data
//
//---------------------------------------------------------------------------//
void BenderPanel::processMachine( void )
{
	
	
	//Do main machine
	tickStateMachine();
	
	//Do pure LED operations first
	//System level LEDs
	txLed.setState(serviceTxLed());
	rxLed.setState(serviceRxLed());
	
	//-- Select the correct 7 segment sources here --//


	//Make displays
	display.setState( SSON );
	switch( displayMode )
	{
		case 0:  //Display Status
			if( settings.currentStatus > 15 )
			{
				tempString[0] = ' ';
				tempString[1] = 'A';
				tempString[2] = 'L';
				tempString[3] = 'L';
			}
			else
			{
				tempString[0] = ' ';
				tempString[1] = ' ';
				if( settings.currentStatus > 9 )
				{
					tempString[2] = 55 + settings.currentStatus;
				}
				else
				{
					tempString[2] = 48 + settings.currentStatus;
				}
				tempString[3] = 'h';
			}
			display.setData(tempString);
		break;
		case 1:  //Display selector
		    sprintf(tempString, "%4d", (unsigned int)selectorPosition);
			tempString[0] = '-';
			tempString[1] = tempString[2];
			tempString[2] = tempString[3];
			tempString[3] = '-';
			display.setData(tempString);
		break;
		case 2:  //show channels
			if( settings.inputChannel > 0 )
			{
				sprintf(tempString, "%4d", (unsigned int)settings.inputChannel);
				tempOutputString[1] = tempString[3];
				tempOutputString[0] = tempString[2];
			}
			else
			{
				tempOutputString[1] = '-';
				tempOutputString[0] = '-';
			}
			if( settings.outputChannel > 0 )
			{
				sprintf(tempString, "%4d", (unsigned int)settings.outputChannel);
				tempOutputString[3] = tempString[3];
				tempOutputString[2] = tempString[2];
			}
			else
			{
				tempOutputString[3] = '-';
				tempOutputString[2] = '-';
			}
			display.setData(tempOutputString);
		break;
		case 3:  //Display Fixed Velo
		    sprintf(tempString, "%4d", (unsigned int)settings.fixedVelocity);
			display.setData(tempString);
		break;
		case 4:  //Display Min Velo
		    sprintf(tempString, "%4d", (unsigned int)settings.minVelocity);
			tempString[0] = tempString[1];
			tempString[1] = tempString[2];
			tempString[2] = tempString[3];
			tempString[3] = '-';			
			display.setData(tempString);
		break;
		case 5:  //Display Max Velo
		    sprintf(tempString, "%4d", (unsigned int)settings.maxVelocity);
			tempString[0] = '-';	
			display.setData(tempString);
		break;
		case 6:  //data
			tempString[0] = 'D';
			tempString[1] = 'A';
			tempString[2] = 'T';
			tempString[3] = 'A';			
			display.setData(tempString);
		break;
		default:
		case 7:  //data
			tempString[0] = '-';
			tempString[1] = ' ';
			tempString[2] = ' ';
			tempString[3] = '-';			
			display.setData(tempString);
		break;		break;
	}
	
	update();
}

void BenderPanel::tickStateMachine()
{
	//***** PROCESS THE LOGIC *****//
    //Now do the states.
    PStates nextState = state;
    switch( state )
    {
    case PInit:
		nextState = PIdle;
		break;
	case PIdle:
		//if( selector.serviceChanged() )
		//Do this until a valid selection has been made
		{
			int8_t tempSelector = selector.getState();
			if( ( tempSelector <= 4 ) && ( tempSelector >= 0) )
			{
				//Valid selector
				selectorPosition = tempSelector;
				nextState = PNewSelector;
				displayMode = 1; //show knob
			}
			else
			{
				displayMode = 7; //show blanker
			}

		}
        break;
	case PNewSelector:
		//DO STUFF HERE ON NEW SELECTOR CHANGES (SET UP DISPLAY)
		//Clear a bunch of stuff
		displayMode = 1; //show knob
		settings.editing = 0;
		option1Led.setState( LEDOFF );
		option2Led.setState( LEDOFF );
		option3Led.setState( LEDOFF );
		//Set things per new selection
		switch( selectorPosition )
		{
		case 0:
			//Pass/block
			if( settings.statusFilterEnabled )
			{
				option2Led.setState( LEDON );
				displayMode = 6; //show channels
			}
			break;
			break;
		case 1:
			//Channel Routing
			if( settings.channelOpEdited )
			{
				option2Led.setState( LEDON );
				displayMode = 2; //show channels
			}
			break;
		case 2:
			//Fix Velocity
			if( settings.velocitySetting == VELO_FIXED )
			{
				option2Led.setState( LEDON );
				displayMode = 3; //show fixed
			}
			break;
		case 3:
			//Fix Velocity
			if( settings.velocitySetting == VELO_SCALED )
			{
				option2Led.setState( LEDON );
				displayMode = 6; //show "data"
			}
			break;
		case 4:
			break;
		default:
			break;
		}
		
		nextState = PSelectorIdle;
		break;
	case PSelectorIdle:
		nextState = PSelectorIdle; //Default operation
		switch( selectorPosition )
		{
		case 0:
			//Pass/block statusFilterEnabled
			//Only button effects
			if(( option2Button.serviceRisingEdge() )&&( settings.editing ))
			{
				if( settings.statusFilterEnabled )
				{
					//
					settings.statusFilterEnabled = 0;
					option2Led.setState( LEDOFF );
					option3Led.setState( LEDOFF );
					displayMode = 1; //show selector
				}
				else
				{
					settings.statusFilterEnabled = 1;
					option2Led.setState( LEDON );
					displayMode = 0; //show status'
				}
			}
			if( settings.editing )
			{
				displayMode = 0; //show status'
				if( downButton.serviceRisingEdge() )
				{
					if( settings.currentStatus > 8 )
					{
						settings.currentStatus = settings.currentStatus - 1;
					}
					else
					{
						settings.currentStatus = 15;
					}
					settings.statusFilterEnabled = 1;
					option2Led.setState( LEDON );
					//set led
					if( settings.statusBlockBits & (0x01 << (settings.currentStatus - 8)) )
					{
						option3Led.setState(LEDON);
					}
					else
					{
						option3Led.setState(LEDOFF);
					}
				}
				if( upButton.serviceRisingEdge() )
				{
					if( settings.currentStatus < 15 )
					{
						settings.currentStatus = settings.currentStatus + 1;
					}
					else
					{
						settings.currentStatus = 8;
					}
					settings.statusFilterEnabled = 1;
					option2Led.setState( LEDON );
					//set led
					if( settings.statusBlockBits & (0x01 << (settings.currentStatus - 8)) )
					{
						option3Led.setState(LEDON);
					}
					else
					{
						option3Led.setState(LEDOFF);
					}
				}
				if( option3Button.serviceRisingEdge() )
				{
					//compare selected status with saved data
					if( settings.statusBlockBits & (0x01 << (settings.currentStatus - 8)) )
					{
						option3Led.setState( LEDOFF );
						settings.statusBlockBits &= ~(0x01 << (settings.currentStatus - 8));
					}
					else
					{
						option3Led.setState( LEDON );
						settings.statusBlockBits |= (0x01 << (settings.currentStatus - 8));
					}
				}
			}
			break;
		case 1:
			//Channel Routing
			//Knobs work only if button state
			if(( option2Button.serviceRisingEdge() )&&( settings.editing ))
			{
				if( settings.channelOpEdited )
				{
					//
					settings.channelOpEdited = 0;
					option2Led.setState( LEDOFF );
					displayMode = 1; //show selector
				}
				else
				{
					settings.channelOpEdited = 1;
					option2Led.setState( LEDON );
					displayMode = 2; //show channels
				}
			}
			//If editing enabled, use knob inputs
			if( settings.editing )
			{
				if( leftSelector.serviceChanged() )
				{
					settings.inputChannel = leftSelector.getState();
					settings.channelOpEdited = 1;
					option2Led.setState( LEDON );
					displayMode = 2; //show channels
				}
				if( rightSelector.serviceChanged() )
				{
					settings.outputChannel = rightSelector.getState();
					settings.channelOpEdited = 1;
					option2Led.setState( LEDON );
					displayMode = 2; //show channels
				}
			}
			break;
		case 2:
			//Fix Velocity
			//Knobs work only if button state
			if(( option2Button.serviceRisingEdge() )&&( settings.editing ))
			{
				if(( settings.velocitySetting == VELO_FIXED )||( settings.velocitySetting == VELO_SCALED))
				{
					//
					settings.velocitySetting = VELO_OFF;
					option2Led.setState( LEDOFF );
					displayMode = 1; //show selector
					
				}
				else
				{
					settings.velocitySetting = VELO_FIXED;
					option2Led.setState( LEDON );
					displayMode = 3; //show fixed					
				}
			}
			//If editing enabled, use knob inputs
			if( settings.editing )
			{
				if(( rightKnob.getState() > rightKnobPosition + KNOBDELTADETECT )||( rightKnob.getState() < rightKnobPosition - KNOBDELTADETECT ))
				{
					rightKnobPosition = rightKnob.getState();
					settings.fixedVelocity = rightKnobPosition >> 1;
					settings.velocitySetting = VELO_FIXED;
					option2Led.setState( LEDON );
					displayMode = 3; //show fixed
				}
			}
			break;
		case 3:
			//Scale Velocity
			//Knobs work only if button state
			if(( option2Button.serviceRisingEdge() )&&( settings.editing ))
			{
				if(( settings.velocitySetting == VELO_FIXED )||( settings.velocitySetting == VELO_SCALED))
				{
					//
					settings.velocitySetting = VELO_OFF;
					option2Led.setState( LEDOFF );
					displayMode = 1; //show selector
					
				}
				else
				{
					settings.velocitySetting = VELO_SCALED;
					option2Led.setState( LEDON );
					displayMode = 6; //show data					
				}
			}
			//If editing enabled, use knob inputs
			if( settings.editing )
			{
				if(( leftKnob.getState() > leftKnobPosition + KNOBDELTADETECT )||( leftKnob.getState() < leftKnobPosition - KNOBDELTADETECT ))
				{
					leftKnobPosition = leftKnob.getState();
					settings.minVelocity = leftKnobPosition >> 1;
					settings.velocitySetting = VELO_SCALED;
					option2Led.setState( LEDON );
					displayMode = 4; //show min
				}
				if(( rightKnob.getState() > rightKnobPosition + KNOBDELTADETECT )||( rightKnob.getState() < rightKnobPosition - KNOBDELTADETECT ))
				{
					rightKnobPosition = rightKnob.getState();
					settings.maxVelocity = rightKnobPosition >> 1;
					settings.velocitySetting = VELO_SCALED;
					option2Led.setState( LEDON );
					displayMode = 5; //show max
				}
			}
			break;
		case 4:
			break;
		default:
			break;
		}
		if( selector.serviceChanged() )
		{
			int8_t tempSelector = selector.getState();
			if( ( tempSelector <= 4 ) && ( tempSelector >= 0) )
			{
				//Valid selector
				selectorPosition = tempSelector;
				nextState = PNewSelector;
			}
		}
			
		if( option1Button.serviceRisingEdge() )
		{
			if( settings.editing == 1 )
			{
				settings.editing = 0;
				option1Led.setState( LEDOFF );
			}
			else
			{
				settings.editing = 1;
				option1Led.setState( LEDFLASHING );
			}
		}
        break;
	case PNewInput:
		nextState = PIdle;
        break;
	default:
        nextState = PInit;
        break;
    }

    state = nextState;

}

void BenderPanel::timersMIncrement( uint8_t inputValue )
{
	upButton.buttonDebounceTimeKeeper.mIncrement(inputValue);
	downButton.buttonDebounceTimeKeeper.mIncrement(inputValue);
	option1Button.buttonDebounceTimeKeeper.mIncrement(inputValue);
	option2Button.buttonDebounceTimeKeeper.mIncrement(inputValue);
	option3Button.buttonDebounceTimeKeeper.mIncrement(inputValue);
	
	display.peekThroughTimeKeeper.mIncrement(inputValue);

}