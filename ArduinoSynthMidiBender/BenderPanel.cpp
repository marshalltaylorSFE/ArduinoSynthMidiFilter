//**BenderPanel*******************************//
#include "BenderPanel.h"
//#include "PanelComponents.h"
#include "Panel.h"
#include "Arduino.h"
#include "flagMessaging.h"


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
	//Do small machines
	if( downButton.serviceRisingEdge() )
	{
		if( settings.selectedSetting > 8 )
		{
			settings.selectedSetting--;
		}
		else
		{
			settings.selectedSetting = 16;
		}
		newStatusFlag.setFlag();
	}
	else
	{
		newStatusFlag.clearFlag();
	}
	if( upButton.serviceRisingEdge() )
	{
		if( settings.selectedSetting < 16 )
		{
			settings.selectedSetting++;
		}
		else
		{
			settings.selectedSetting = 8;
		}
		newStatusFlag.setFlag();
	}
	else
	{
		newStatusFlag.clearFlag();
	}
	if( option1Button.serviceRisingEdge() )
	{
		//Check if setting available
		
	}
	if( option2Button.serviceRisingEdge() )
	{
		
	}
	if( option3Button.serviceRisingEdge() )
	{
		
	}

	if( leftSelector.serviceChanged() )
	{
		leftKnobPosition = leftSelector.getState();
		displayMode = 2;
	}
	if(( rightKnob.getState() > rightKnobPosition + 5 )||( rightKnob.getState() < rightKnobPosition - 5 ))
	{
		rightKnobPosition = rightKnob.getState();
		displayMode = 3;
	}
	if( selector.serviceChanged() )
	{
		selectorPosition = selector.getState();

	}
	
	//Do main machine
	tickStateMachine();
	
	//Do pure LED operations first
	//System level LEDs
	txLed.setState(serviceTxLed());
	rxLed.setState(serviceRxLed());
	
	//Panel level LEDs
	option1Led.setState(LEDON);
	option2Led.setState(LEDON);
	option3Led.setState(LEDON);
	
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
			
			//sprintf(tempString, "%4d", (unsigned int)settings.selectedSetting);
			if( settings.selectedSetting > 15 )
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
				if( settings.selectedSetting > 9 )
				{
					tempString[2] = 55 + settings.selectedSetting;
				}
				else
				{
					tempString[2] = 48 + settings.selectedSetting;
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