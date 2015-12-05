#include "Panel.h"

//This is where PanelComponents are joined to form the custom panel

#define option1ButtonPin 2
#define option2ButtonPin 3
#define option3ButtonPin 4
#define upButtonPin 5
#define downButtonPin 8

#define rxLedPin 6
#define txLedPin 7
#define option1LedPin 11
#define option2LedPin 10
#define option3LedPin 9

#define selectorPin A2
#define leftKnobPin A0
#define rightKnobPin A1

Panel::Panel( void )
{
	flasherState = 0;
	fastFlasherState = 0;
}

void Panel::init( void )
{
	option1Button.init(option1ButtonPin);
	option2Button.init(option2ButtonPin);
	option3Button.init(option3ButtonPin);
	upButton.init(upButtonPin);
	downButton.init(downButtonPin);
	rxLed.init(rxLedPin, 0, &flasherState, &fastFlasherState);
	txLed.init(txLedPin, 0, &flasherState, &fastFlasherState);
	option1Led.init(option1LedPin, 0, &flasherState, &fastFlasherState);
	option2Led.init(option2LedPin, 0, &flasherState, &fastFlasherState);
	option3Led.init(option3LedPin, 0, &flasherState, &fastFlasherState);

	selector.init(selectorPin, 255, 0, 10 ); //With max, min ranges and resolution
	leftSelector.init(leftKnobPin, 255, 0, 17 ); //With max, min ranges and resolution
	rightSelector.init(rightKnobPin, 255, 0, 17 ); //With max, min ranges and resolution
	leftKnob.init(leftKnobPin);
	rightKnob.init(rightKnobPin);

	display.init( 0x71, &flasherState, &fastFlasherState );
	
 	flasherState = 0;
	fastFlasherState = 0;
	
}

void Panel::update( void )
{
	option1Button.update();
	option2Button.update();
	option3Button.update();
	upButton.update();
	downButton.update();

	rxLed.update();
	txLed.update();
	option1Led.update();
	option2Led.update();
	option3Led.update();

	selector.update();
	leftSelector.update();
	rightSelector.update();
	leftKnob.update();
	rightKnob.update();
	display.update();
}

void Panel::toggleFlasherState( void )
{
	flasherState ^= 0x01;
}

void Panel::toggleFastFlasherState( void )
{
	fastFlasherState ^= 0x01;
}