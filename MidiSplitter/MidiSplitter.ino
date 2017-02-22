#include <MIDI.h>

#include <SoftwareSerial.h>
SoftwareSerial softSerial(10,11);

// MIDI things
#include <MIDI.h>
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, midiA);

#define ENB_SERIAL

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
#ifdef ENB_SERIAL
	softSerial.print("0x");
	softSerial.print(channel, HEX);
	softSerial.print(" 0x");
	softSerial.print(pitch, HEX);
	softSerial.print(" 0x");
	softSerial.print(velocity, HEX);
	softSerial.print("\n");
#endif
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
#ifdef ENB_SERIAL
	softSerial.print("0x");
	softSerial.print(channel, HEX);
	softSerial.print(" 0x");
	softSerial.print(pitch, HEX);
	softSerial.print(" 0x");
	softSerial.print(velocity, HEX);
	softSerial.print("\n");
#endif
}

void setup()
{
	pinMode(11, OUTPUT);
//	Serial.begin(57600);
	softSerial.begin(57600);
	midiA.turnThruOn();
	midiA.setHandleNoteOn(handleNoteOn);  // Put only the name of the function
	midiA.setHandleNoteOff(handleNoteOff);
	
	// Initiate MIDI communications, listen to all channels
	midiA.begin(MIDI_CHANNEL_OMNI);
#ifdef ENB_SERIAL
	softSerial.println("Starting program");
#endif
}

void loop()
{
    midiA.read();
}