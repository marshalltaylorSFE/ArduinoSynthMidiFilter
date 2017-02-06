#include <Wire.h>
//#include <flagMessaging.h>
#include <PanelComponents.h>
//#include <s7sWrapper.h>
#include <timeKeeper.h>
#include <timerModule.h>
#include "BenderSettings.h"


//HOW TO OPERATE
//  Make TimerClass objects for each thing that needs periodic service
//  pass the interval of the period in ticks
//
//  Set MAXINTERVAL to rollover rate
//  Set MAXTIMER to the max foreseen interval of any timer.
//  MAXTIMER + MAXINTERVAL = max countable value.

//Globals
uint16_t maxTimer = 60000;
uint16_t maxInterval = 2000;


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
#include "BenderPanel.h"
BenderPanel myBenderPanel;

// MIDI things
#include <MIDI.h>
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, midiA);

uint8_t txLedFlag = 0;
uint8_t rxLedFlag = 0;

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
	uint8_t playSecondNote = 0;
	rxLedFlag = 1;
	if(( myBenderPanel.settings.inputChannel == 0 )||( myBenderPanel.settings.inputChannel == channel ))
	{
		if( myBenderPanel.settings.channelOpEdited )
		{
			channel = myBenderPanel.settings.outputChannel;
		}
		if( myBenderPanel.settings.splitEdited )
		{
			int tempPitch = 0;
			switch(myBenderPanel.settings.splitMode)
			{
				case SPLIT_OFF:
				default:
				break;
				case SPLIT_NORMAL:
					if( pitch > myBenderPanel.settings.splitPoint )
					{
						channel = myBenderPanel.settings.upperOutputChannel;
						tempPitch = pitch + myBenderPanel.settings.upperOctave * 12;
						if( tempPitch > 127 ) tempPitch = 127;
						if( tempPitch < 0 ) tempPitch = 0;
						pitch = tempPitch;
					}
				break;
				case SPLIT_CHROUS_TOP:
					if( pitch > myBenderPanel.settings.splitPoint )
					{
						tempPitch = pitch + myBenderPanel.settings.upperOctave * 12;
						if( tempPitch > 127 ) tempPitch = 127;
						if( tempPitch < 0 ) tempPitch = 0;
						pitch = tempPitch;
						playSecondNote = 1;
					}
				break;
				case SPLIT_CHROUS_BOTTOM:
					if( pitch < myBenderPanel.settings.splitPoint )
					{
						tempPitch = pitch + myBenderPanel.settings.upperOctave * 12;
						if( tempPitch > 127 ) tempPitch = 127;
						if( tempPitch < 0 ) tempPitch = 0;
						pitch = tempPitch;
						playSecondNote = 1;
					}
				break;
			}
		}
		
		//Modify velocity
		switch( myBenderPanel.settings.velocitySetting )
		{
			case VELO_FIXED:
				//Over-ride input
				velocity = myBenderPanel.settings.fixedVelocity;
			break;
			case VELO_SCALED:
				//Scale, assume input has full 127 range
				if( myBenderPanel.settings.maxVelocity > myBenderPanel.settings.minVelocity )
				{
					int8_t tempRange = myBenderPanel.settings.maxVelocity - myBenderPanel.settings.minVelocity;
					float mathVar = tempRange * velocity / 127;
					mathVar += myBenderPanel.settings.minVelocity;
					velocity = mathVar;
				}
				else
				{
					velocity = myBenderPanel.settings.maxVelocity;
				}
			break;
			default:
			case VELO_OFF:
			break;
		}
		midiA.sendNoteOn(pitch, velocity, channel);
		if( playSecondNote )
		{
			channel = myBenderPanel.settings.upperOutputChannel;
			midiA.sendNoteOn(pitch, velocity, channel);
		}
		txLedFlag = 1;
	}
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
	uint8_t playSecondNote = 0;
	rxLedFlag = 1;
	if(( myBenderPanel.settings.inputChannel == 0 )||( myBenderPanel.settings.inputChannel == channel ))
	{
		if( myBenderPanel.settings.channelOpEdited )
		{
			channel = myBenderPanel.settings.outputChannel;
		}
		if( myBenderPanel.settings.splitEdited )
		{
			int tempPitch = 0;
			switch(myBenderPanel.settings.splitMode)
			{
				case SPLIT_OFF:
				default:
				break;
				case SPLIT_NORMAL:
					if( pitch > myBenderPanel.settings.splitPoint )
					{
						channel = myBenderPanel.settings.upperOutputChannel;
						tempPitch = pitch + myBenderPanel.settings.upperOctave * 12;
						if( tempPitch > 127 ) tempPitch = 127;
						if( tempPitch < 0 ) tempPitch = 0;
						pitch = tempPitch;
					}
				break;
				case SPLIT_CHROUS_TOP:
					if( pitch > myBenderPanel.settings.splitPoint )
					{
						tempPitch = pitch + myBenderPanel.settings.upperOctave * 12;
						if( tempPitch > 127 ) tempPitch = 127;
						if( tempPitch < 0 ) tempPitch = 0;
						pitch = tempPitch;
						playSecondNote = 1;
					}
				break;
				case SPLIT_CHROUS_BOTTOM:
					if( pitch < myBenderPanel.settings.splitPoint )
					{
						tempPitch = pitch + myBenderPanel.settings.upperOctave * 12;
						if( tempPitch > 127 ) tempPitch = 127;
						if( tempPitch < 0 ) tempPitch = 0;
						pitch = tempPitch;
						playSecondNote = 1;
					}
				break;
			}
		}
		
		//Modify velocity
		if( velocity != 0 )
		{
			switch( myBenderPanel.settings.velocitySetting )
			{
				case VELO_FIXED:
					//Over-ride input
					velocity = myBenderPanel.settings.fixedVelocity;
				break;
				case VELO_SCALED:
					//Scale, assume input has full 127 range
					if( myBenderPanel.settings.maxVelocity > myBenderPanel.settings.minVelocity )
					{
						int8_t tempRange = myBenderPanel.settings.maxVelocity - myBenderPanel.settings.minVelocity;
						float mathVar = tempRange * velocity / 127;
						mathVar += myBenderPanel.settings.minVelocity;
						velocity = mathVar;
					}
					else
					{
						velocity = myBenderPanel.settings.maxVelocity;
					}
				break;
				default:
				case VELO_OFF:
				break;
			}
		}
		else
		{
			//do nothing
		}
		midiA.sendNoteOff(pitch, velocity, channel);
		if( playSecondNote )
		{
			channel = myBenderPanel.settings.upperOutputChannel;
			midiA.sendNoteOff(pitch, velocity, channel);
		}
		txLedFlag = 1;
	}
	
}

void HandleAfterTouchPoly(byte channel, byte note, byte pressure)
{
	//status A
	rxLedFlag = 1;
	if(myBenderPanel.settings.statusBlockBits & 0x4 )
	{
		//blocked
	}
	else
	{
		txLedFlag = 1;
		midiA.sendPolyPressure(note, pressure, channel);
	}
}

void HandleControlChange(byte channel, byte number, byte value)
{
	//status B
	rxLedFlag = 1;
	if(myBenderPanel.settings.statusBlockBits & 0x8 )
	{
		//blocked
	}
	else
	{
		txLedFlag = 1;
		midiA.sendControlChange(number, value, channel);
	}
}

void HandleProgramChange(byte channel, byte number)
{
	//status C
	rxLedFlag = 1;
	if(myBenderPanel.settings.statusBlockBits & 0x10 )
	{
		//blocked
	}
	else
	{
		txLedFlag = 1;
		midiA.sendProgramChange(number, channel);
	}
}

void HandleAfterTouchChannel(byte channel, byte pressure)
{
	//status D
	rxLedFlag = 1;
	if(myBenderPanel.settings.statusBlockBits & 0x20 )
	{
		//blocked
	}
	else
	{
		txLedFlag = 1;
		midiA.sendAfterTouch(pressure, channel);
	}
}

void HandlePitchBend(byte channel, int bend)
{
	//status E
	rxLedFlag = 1;
	if(myBenderPanel.settings.statusBlockBits & 0x40 )
	{
		//blocked
	}
	else
	{
		txLedFlag = 1;
		midiA.sendPitchBend(bend, channel);
	}
}


void setup()
{
	for(int i = 0; i < 16; i++)
	{
		//messagePassThroughTable[i] = 1;
	}
	//Initialize serial:
	//Serial.begin(9600);
	delay(2000);
	//Serial.println("Program Started");
	
	Wire.begin();  // Initialize hardware I2C pins
	
	//Init panel.h stuff
	myBenderPanel.init();

	//Go to fresh state
	myBenderPanel.reset();
	//Connect MIDI handlers
	midiA.setHandleNoteOn(handleNoteOn);  // Put only the name of the function
	midiA.setHandleNoteOff(handleNoteOff);
	midiA.setHandleControlChange(HandleControlChange);
	midiA.setHandleProgramChange(HandleProgramChange);
	midiA.setHandleAfterTouchChannel(HandleAfterTouchChannel);
	midiA.setHandlePitchBend(HandlePitchBend);

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
		myBenderPanel.timersMIncrement(5);
	
	}
		
	//**Process the panel and state machine***********//  
	if(panelUpdateTimer.flagStatus() == PENDING)
	{
		//Provide inputs
		if( rxLedFlag == 1 )
		{
			rxLedFlag = 0;
			myBenderPanel.setRxLed();
		}
		//Provide inputs
		if( txLedFlag == 1 )
		{
			txLedFlag = 0;
			myBenderPanel.setTxLed();
		}
		//Tick the machine
		myBenderPanel.processMachine();
		
		//Deal with outputs
	}
	//**Fast LED toggling of the panel class***********//  
	if(ledToggleFastTimer.flagStatus() == PENDING)
	{
		myBenderPanel.toggleFastFlasherState();
		
	}

	//**LED toggling of the panel class***********//  
	if(ledToggleTimer.flagStatus() == PENDING)
	{
		myBenderPanel.toggleFlasherState();
		
	}
	//**Debug timer*******************************//  
	if(debugTimer.flagStatus() == PENDING)
	{
		//rxLedFlag = 1;
	
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
	if( msTicks >= ( maxTimer + maxInterval ) )
	{
		returnVar = msTicks - maxTimer;

	}
	else
	{
		returnVar = msTicks + 1;
	}
	msTicks = returnVar;
	msTicksLocked = 0;  //unlock
}

