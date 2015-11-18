#ifndef PANEL_H
#define PANEL_H

#include "PanelComponents.h"

class Panel
{
public:
	Panel( void );
	void update( void );
	void init( void );
	void toggleFlasherState( void );
	void toggleFastFlasherState( void );

	PanelButton option1Button;
	PanelButton option2Button;
	PanelButton option3Button;
	PanelButton upButton;
	PanelButton downButton;

	PanelLed rxLed;
	PanelLed txLed;
	PanelLed option1Led;
	PanelLed option2Led;
	PanelLed option3Led;

	PanelSelector selector;
	PanelSelector leftSelector;
	PanelSelector rightSelector;
	PanelKnob8Bit leftKnob;
	PanelKnob8Bit rightKnob;
	
	char tempString[10];  // Will be used with sprintf to create strings
	sSDisplay display;

protected:
private:
	volatile uint8_t flasherState;
	volatile uint8_t fastFlasherState;

};

#endif // PANEL_H


