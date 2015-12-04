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
		if( selector.serviceChanged() )
		{
			int8_t tempSelector = selector.getState();
			if( ( tempSelector <= 6 ) && ( tempSelector >= 0) )
			{
				//Valid selector
				selectorPosition = tempSelector;
				nextState = PNewSelector;
				displayMode = 1; //show knob
			}

		}
        break;
	case PNewSelector:
		nextState = PSelectorIdle;
        break;
	case PSelectorIdle:
		if( selector.serviceChanged() )
		{
			int8_t tempSelector = selector.getState();
			if( ( tempSelector <= 6 ) && ( tempSelector >= 0) )
			{
				//Valid selector
				selectorPosition = tempSelector;
				nextState = PNewSelector;
				displayMode = 1; //show knob
				settings.editing = 0;
				option1Led.setState( LEDOFF );
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
		nextState = PSelectorIdle;
        break;
	case PNewInput:
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