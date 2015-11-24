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
	statusByteSettings * currentStatusObject = settings.getCurrentStatusPtr();
	
	//Do small machines
	if( downButton.serviceRisingEdge() )
	{
		if( settings.getCurrentStatus() > 8 )
		{
			settings.setCurrentStatus( settings.getCurrentStatus() - 1 );
		}
		else
		{
			settings.setCurrentStatus( 16 );
		}
		newStatusFlag.setFlag();
	}
	else
	{
		newStatusFlag.clearFlag();
	}
	if( upButton.serviceRisingEdge() )
	{
		if( settings.getCurrentStatus() < 16 )
		{
			settings.setCurrentStatus( settings.getCurrentStatus() + 1 );
		}
		else
		{
			settings.setCurrentStatus( 8 );
		}
		newStatusFlag.setFlag();
	}
	else
	{
		newStatusFlag.clearFlag();
	}

	uint8_t editChanged = 0;
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
	//Check conditions on new input from ANY option parameter, decide what to do later
	uint8_t parameterChanged = 0;
	if( option2Button.serviceRisingEdge() )
	{
		parameterChanged++;
		option2Led.setState( LEDOFF );
	}
	if( option3Button.serviceRisingEdge() )
	{
		parameterChanged++;
		option3Led.setState( LEDOFF );
	}

	if( leftSelector.serviceChanged() )
	{
		parameterChanged++;
		leftSelectorPosition = leftSelector.getState();
		//displayMode = 2;
	}
	if( rightSelector.serviceChanged() )
	{
		parameterChanged++;
		rightSelectorPosition = rightSelector.getState();
		//displayMode = 2;
	}
	if(( leftKnob.getState() > leftKnobPosition + KNOBDELTADETECT )||( leftKnob.getState() < rightKnobPosition - KNOBDELTADETECT ))
	{
		parameterChanged++;
		leftKnobPosition = leftKnob.getState();
		//displayMode = 3;
	}
	if(( rightKnob.getState() > rightKnobPosition + KNOBDELTADETECT )||( rightKnob.getState() < rightKnobPosition - KNOBDELTADETECT ))
	{
		parameterChanged++;
		rightKnobPosition = rightKnob.getState();
		displayMode = 3;
	}	
	
	if( editChanged )
	{
		//Check for state of 'edit'
		
	}
	
	if( selector.serviceChanged() )
	{
		selectorPosition = selector.getState();
		if( currentStatusObject->modifiableMask & ( 0x01 << selectorPosition ))
		{
			//New position to update
			//Turn off editing LED
			option1Led.setState( LEDOFF );
			//set option led
			if( currentStatusObject->modifiedMask & ( 0x01 << selectorPosition ) )
			{
				option2Led.setState( LEDON );
			}
		}

	}
	
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
		case 0:  //Channel is song number
			display.setData("h");
			//sprintf(tempString, "%4d", (unsigned int)settings.selectedSetting);
			//display.setData(tempString);
		break;
		case 1:  //Display selector
		    sprintf(tempString, "%4d", (unsigned int)selectorPosition);
			tempString[0] = '-';
			tempString[1] = tempString[2];
			tempString[2] = tempString[3];
			tempString[3] = '-';
			display.setData(tempString);
		break;
		case 2:  //Display left knob
		    sprintf(tempString, "%4d", (unsigned int)leftKnobPosition);
			display.setData(tempString);
		break;
		case 3:  //Display right knob
		    sprintf(tempString, "%4d", (unsigned int)rightKnobPosition);
			display.setData(tempString);
		break;
		default:
		break;
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
		if( newStatusFlag.serviceRisingEdge() )
		{
			nextState = PNewStatus;
			
			//sprintf(tempString, "%4d", (unsigned int)settings.getCurrentStatus());
			if( settings.getCurrentStatus() > 15 )
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
				if( settings.getCurrentStatus() > 9 )
				{
					tempString[2] = 55 + settings.getCurrentStatus();
				}
				else
				{
					tempString[2] = 48 + settings.getCurrentStatus();
				}
				tempString[3] = 'h';
			}
			display.peekThrough( tempString, 1500 ); // 'data' type, time in ms to persist
			
			displayMode = 1; //show knob
		}
        break;
	case PNewStatus:
		nextState = PIdle;
        break;
	case PNewSelector:
		nextState = PIdle;
        break;
	default:
        nextState = PInit;
        break;
    }

	////Master with the E-Stop
	if( option1Button.serviceRisingEdge() )
	{
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