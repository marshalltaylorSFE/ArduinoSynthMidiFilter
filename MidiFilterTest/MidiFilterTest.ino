#include <timeKeeper.h>
#include <timerModule.h>


//HOW TO OPERATE
//  Make TimerClass objects for each thing that needs periodic service
//  pass the interval of the period in ticks
//
//  Set MAXINTERVAL to rollover rate
//  Set MAXTIMER to the max foreseen interval of any timer.
//  MAXTIMER + MAXINTERVAL = max countable value.

//Globals
uint16_t MAXTIMER = 60000;
uint16_t MAXINTERVAL = 2000;


#define LEDPIN 13
#include "timerModule.h"
#include "stdint.h"

//Not used by this sketch but dependant on one 
#include "Wire.h"

//If 328p based, do this
#ifdef __AVR__
#include <Arduino.h>
#endif

//If Teensy 3.1 based, do this
#ifdef __MK20DX256__
IntervalTimer myTimer;
#endif

//Globals
//**Copy to make a new timer******************//  
//TimerClass32 msTimerA( 200 ); //200 ms
TimerClass panelUpdateTimer( 10 );

TimerClass ledToggleTimer( 333 );
uint8_t ledToggleState = 0;

TimerClass ledToggleFastTimer( 100 );
uint8_t ledToggleFastState = 0;

TimerClass debounceTimer(5);

TimerClass debugTimer(2000);


//**Panel State Machine***********************//
//#include "BenderPanel.h"
//BenderPanel myBenderPanel;

// MIDI things
#include <MIDI.h>
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, midiA);

//uint8_t rxLedFlag = 0;

//Note on TimerClass-
//Change with msTimerA.setInterval( <the new interval> );


uint16_t msTicks = 0;

//  The lock works like this:
//
//    When the interrupt fires, the lock is removed.  Now
//    the main free-wheeling loop can update the change to
//    the timerModules.  Once complete, the lock is replaced
//    so that it can't update more than once per firing
//    of the interrupt

uint8_t msTicksLocked = 1; //start locked out

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
	midiA.sendNoteOn(34, 56, 7);
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
	midiA.sendNoteOff(34, 0, 7);
}

void setup()
{
	//Initialize serial:
	//Serial.begin(9600);
	delay(2000);
	//Serial.println("Program Started");

	//Connect MIDI handlers
	midiA.setHandleNoteOn(handleNoteOn);  // Put only the name of the function
	midiA.setHandleNoteOff(handleNoteOff);
	
	// Initiate MIDI communications, listen to all channels
	midiA.begin(MIDI_CHANNEL_OMNI);
	//midiA.turnThruOn();
	midiA.turnThruOff();


	//If 328p based, do this
#ifdef __AVR__
	// initialize Timer1
	cli();          // disable global interrupts
	TCCR1A = 0;     // set entire TCCR1A register to 0
	TCCR1B = 0;     // same for TCCR1B

	// set compare match register to desired timer count:
	OCR1A = 16000;

	// turn on CTC mode:
	TCCR1B |= (1 << WGM12);

	// Set CS10 and CS12 bits for 1 prescaler:
	TCCR1B |= (1 << CS10);


	// enable timer compare interrupt:
	TIMSK1 |= (1 << OCIE1A);

	// enable global interrupts:
	sei();
#endif

	//If Teensy 3.1 based, do this
#ifdef __MK20DX256__
	// initialize IntervalTimer
	myTimer.begin(serviceMS, 1000);  // serviceMS to run every 0.001 seconds
#endif


}

void loop()
{
	//Update the timers, but only once per interrupt
	if( msTicksLocked == 0 )
	{
		//**Copy to make a new timer******************//  
		//msTimerA.update(msTicks);
		
		ledToggleTimer.update(msTicks);
		ledToggleFastTimer.update(msTicks);
		panelUpdateTimer.update(msTicks);
		debounceTimer.update(msTicks);
		debugTimer.update(msTicks);
		
		//Done?  Lock it back up
		msTicksLocked = 1;
	}  //The ISR will unlock.

	//**Copy to make a new timer******************//  
	//if(msTimerA.flagStatus() == PENDING)
	//{
	//	//User code
	//}

	//**Debounce timer****************************//  
	if(debounceTimer.flagStatus() == PENDING)
	{
	}
		
	//**Process the panel and state machine***********//  
	if(panelUpdateTimer.flagStatus() == PENDING)
	{
	}
	//**Fast LED toggling of the panel class***********//  
	if(ledToggleFastTimer.flagStatus() == PENDING)
	{
	}

	//**LED toggling of the panel class***********//  
	if(ledToggleTimer.flagStatus() == PENDING)
	{
	}
	//**Debug timer*******************************//  
	if(debugTimer.flagStatus() == PENDING)
	{
	}
	midiA.read();
	
	
}

//If 328p based, do this
#ifdef __AVR__
ISR(TIMER1_COMPA_vect)
#else
#endif
//If Teensy 3.1 based, do this
#ifdef __MK20DX256__
void serviceMS(void)
#else
#endif
{
	uint32_t returnVar = 0;
	if( msTicks >= ( MAXTIMER + MAXINTERVAL ) )
	{
		returnVar = msTicks - MAXTIMER;

	}
	else
	{
		returnVar = msTicks + 1;
	}
	msTicks = returnVar;
	msTicksLocked = 0;  //unlock
}

