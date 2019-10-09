#ifndef GRID_PROTOCOL_H_INCLUDED
#define GRID_PROTOCOL_H_INCLUDED

#include "sam.h"


#define GRID_MSG_NULL					0x00

#define GRID_MSG_START_OF_HEADING		0x01
#define GRID_MSG_START_OF_TEXT			0x02
#define GRID_MSG_END_OF_TEXT			0x03
#define GRID_MSG_END_OF_BLOCK			0x17
#define GRID_MSG_END_OF_TRANSMISSION	0x04

#define GRID_MSG_ACKNOWLEDGE			0x06
#define GRID_MSG_NACKNOWLEDGE			0x15	
#define GRID_MSG_CANCEL					0x18	// discard and send next

#define GRID_MSG_NORTH					0x11	// Device Control 1
#define GRID_MSG_EAST					0x12	// Device Control 2
#define GRID_MSG_SOUTH					0x13	// Device Control 3
#define GRID_MSG_WEST					0x14	// Device Control 4

#define GRID_MSG_BELL					0x07


#define GRID_MSG_PROTOCOL				160

#define GRID_MSG_PROTOCOL_MIDI			0
#define GRID_MSG_PROTOCOL_KEYBOARD		1
#define GRID_MSG_PROTOCOL_MOUSE			2

#define GRID_MSG_COMMAND_MIDI_CONTROLCHANGE 176
				


#define GRID_MSG_BOOT 'B'
#define GRID_MSG_PING 'P'
#define GRID_MSG_CONNECT 'C'


#endif /* GRID_PROTOCOL_H_INCLUDED */