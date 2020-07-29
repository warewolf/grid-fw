#ifndef GRID_PROTOCOL_H_INCLUDED
#define GRID_PROTOCOL_H_INCLUDED

#define GRID_PROTOCOL_VERSION_MAJOR		1
#define GRID_PROTOCOL_VERSION_MINOR		0
#define GRID_PROTOCOL_VERSION_PATCH		5


// Module HWCFG definitions

#define  GRID_MODULE_PO16_RevB		0
#define  GRID_MODULE_PO16_RevC		8

#define  GRID_MODULE_BU16_RevB		128
#define  GRID_MODULE_BU16_RevC		136

#define  GRID_MODULE_PBF4_RevA		64
#define  GRID_MODULE_EN16_RevA		192

#define  GRID_PARAMETER_HEARTBEAT_interval	250
#define  GRID_PARAMETER_PING_interval		100

#define  GRID_PARAMETER_TEXT_maxlength		100
#define  GRID_PARAMETER_PACKET_maxlength	300
#define  GRID_PARAMETER_PACKET_marign		200




#define GRID_CONST_NUL				0x00

#define GRID_CONST_SOH				0x01	//start of header
#define GRID_CONST_STX				0x02	//start of text
#define GRID_CONST_ETX				0x03	//end of text
#define GRID_CONST_EOB				0x17	//end of block
#define GRID_CONST_EOT				0x04	//end of transmission

#define GRID_CONST_LF				0x0A	//linefeed, newline

#define GRID_CONST_ACK				0x06	//acknowledge
#define GRID_CONST_NAK				0x15	//nacknowledge	
#define GRID_CONST_CAN				0x18	//cancel

#define GRID_CONST_NORTH			0x11	// Device Control 1
#define GRID_CONST_EAST				0x12	// Device Control 2
#define GRID_CONST_SOUTH			0x13	// Device Control 3
#define GRID_CONST_WEST				0x14	// Device Control 4

#define GRID_CONST_DCT				0x0E	// Shift In = Direct
#define GRID_CONST_BRC				0x0F	// Shift Out = Broadcast

#define GRID_CONST_BELL				0x07 	





#define GRID_PARAMETER_MIDI_NOTEOFF							0x80
#define GRID_PARAMETER_MIDI_NOTEON							0x90
#define GRID_PARAMETER_MIDI_CONTROLCHANGE					0xB0


#define GRID_PARAMETER_KEYBOARD_KEYDOWN						128
#define GRID_PARAMETER_KEYBOARD_KEYUP						129
#define GRID_PARAMETER_KEYBOARD_MODIFIER					130
#define GRID_PARAMETER_KEYBOARD_NOTMODIFIER					131
 	



// HEADER BROADCAST

#define GRID_BRC_format				"%c%c%02x%02x%02x%02x%02x%02x%c",GRID_CONST_SOH,GRID_CONST_BRC,len,id,dx,dy,age,rot,GRID_CONST_EOB
#define GRID_BRC_frame				"%c%c............%c",GRID_CONST_SOH,GRID_CONST_BRC,GRID_CONST_EOB


#define GRID_BRC_LEN_offset			2
#define GRID_BRC_LEN_length			2

#define GRID_BRC_ID_offset			4
#define GRID_BRC_ID_length			2

#define GRID_BRC_DX_offset			6
#define GRID_BRC_DX_length			2

#define GRID_BRC_DY_offset			8
#define GRID_BRC_DY_length			2

#define GRID_BRC_AGE_offset			10
#define GRID_BRC_AGE_length			2

#define GRID_BRC_ROT_offset			12
#define GRID_BRC_ROT_length			2


//NEW ERA


#define GRID_INSTR_length					1
#define GRID_INSTR_offset					4

#define GRID_INSTR_REQ_code					0xE
#define GRID_INSTR_REP_code					0xF


#define GRID_CLASS_length					3
#define GRID_CLASS_offset					1

// MIDI
#define GRID_CLASS_MIDIRELATIVE_code		0x000
#define GRID_CLASS_MIDIRELATIVE_format		"%c%03x%01x%02x%02x%02x%02x%c",GRID_CONST_STX,GRID_CLASS_MIDIRELATIVE_code,instruction,cablechannel,channelcommand,param1,param2,GRID_CONST_ETX
#define GRID_CLASS_MIDIRELATIVE_frame		"%c%03x_........%c",GRID_CONST_STX,GRID_CLASS_MIDIRELATIVE_code,GRID_CONST_ETX


#define GRID_CLASS_MIDIRELATIVE_CABLECOMMAND_offset			5
#define GRID_CLASS_MIDIRELATIVE_CABLECOMMAND_length			2

#define GRID_CLASS_MIDIRELATIVE_COMMANDCHANNEL_offset		7
#define GRID_CLASS_MIDIRELATIVE_COMMANDCHANNEL_length		2

#define GRID_CLASS_MIDIRELATIVE_PARAM1_offset				9
#define GRID_CLASS_MIDIRELATIVE_PARAM1_length				2

#define GRID_CLASS_MIDIRELATIVE_PARAM2_offset				11
#define GRID_CLASS_MIDIRELATIVE_PARAM2_length				2

//#define GRID_CLASS_MIDIABSOLUTE_code		0x001
//#define GRID_CLASS_MIDIABSOLUTE_format		"%c%03x%01x%02x%02x%02x%02x%c",GRID_CONST_STX,GRID_CLASS_MIDIABSOLUTE_code,instruction,cablechannel,channelcommand,param1,param2,GRID_CONST_ETX

// HEARTBEAT
#define GRID_CLASS_HEARTBEAT_code			0x010
#define GRID_CLASS_HEARTBEAT_format			"%c%03x%01x%02x%02x%02x%02x%c",GRID_CONST_STX,GRID_CLASS_HEARTBEAT_code,instruction,hwcfg,vmajor,vminor,vpatch,GRID_CONST_ETX
#define GRID_CLASS_HEARTBEAT_frame			"%c%03x_........%c",GRID_CONST_STX,GRID_CLASS_HEARTBEAT_code,GRID_CONST_ETX

#define GRID_CLASS_HEARTBEAT_HWCFG_offset		5
#define GRID_CLASS_HEARTBEAT_HWCFG_length		2

#define GRID_CLASS_HEARTBEAT_VMAJOR_offset		7
#define GRID_CLASS_HEARTBEAT_VMAJOR_length		2

#define GRID_CLASS_HEARTBEAT_VMINOR_offset		9
#define GRID_CLASS_HEARTBEAT_VMINOR_length		2

#define GRID_CLASS_HEARTBEAT_VPATCH_offset		11
#define GRID_CLASS_HEARTBEAT_VPATCH_length		2


// DEBUG
//#define GRID_CLASS_DEBUGTEXT_code			0x020
//#define GRID_CLASS_DEBUGTEXT_format			"%c%03x%01x%s%c",GRID_CONST_STX,GRID_CLASS_DEBUGTEXT_code,instruction,message,GRID_CONST_ETX

// BANK
#define GRID_CLASS_BANKACTIVE_code			0x030
#define GRID_CLASS_BANKACTIVE_format		"%c%03x%01x%02x%c",GRID_CONST_STX,GRID_CLASS_BANKACTIVE_code,instruction,banknumber,GRID_CONST_ETX
#define GRID_CLASS_BANKACTIVE_frame			"%c%03x_..%c",GRID_CONST_STX,GRID_CLASS_BANKACTIVE_code,GRID_CONST_ETX

#define GRID_CLASS_BANKACTIVE_BANKNUMBER_offset		5
#define GRID_CLASS_BANKACTIVE_BANKNUMBER_length		2

#define GRID_CLASS_BANKENABLED_code			0x031
#define GRID_CLASS_BANKENABLED_format		"%c%03x%01x%02x%02x%c",GRID_CONST_STX,GRID_CLASS_BANKACTIVE_code,instruction,banknumber,isenabled,GRID_CONST_ETX	
#define GRID_CLASS_BANKENABLED_frame		"%c%03x_....%c",GRID_CONST_STX,GRID_CLASS_BANKACTIVE_code,GRID_CONST_ETX	

#define GRID_CLASS_BANKENABLED_BANKNUMBER_offset		5
#define GRID_CLASS_BANKENABLED_BANKNUMBER_length		2

#define GRID_CLASS_BANKENABLED_ISENABLED_offset			7
#define GRID_CLASS_BANKENABLED_ISENABLED_length			2

#define GRID_CLASS_BANKCOLOR_code			0x032
#define GRID_CLASS_BANKCOLOR_format			"%c%03x%01x%02x%02x%02x%02x%c",GRID_CONST_STX,GRID_CLASS_BANKACTIVE_code,instruction,banknumber,red,green,blue,GRID_CONST_ETX
#define GRID_CLASS_BANKCOLOR_frame			"%c%03x_........%c",GRID_CONST_STX,GRID_CLASS_BANKACTIVE_code,GRID_CONST_ETX

#define GRID_CLASS_BANKCOLOR_BANKNUMBER_offset			5
#define GRID_CLASS_BANKCOLOR_BANKNUMBER_length			2

#define GRID_CLASS_BANKCOLOR_RED_offset					7
#define GRID_CLASS_BANKCOLOR_RED_length					2

#define GRID_CLASS_BANKCOLOR_GREEN_offset				9
#define GRID_CLASS_BANKCOLOR_GREEN_length				2

#define GRID_CLASS_BANKCOLOR_BLUE_offset				11
#define GRID_CLASS_BANKCOLOR_BLUE_length				2



#define GRID_CLASS_LEDPHASE_code			0x040
#define GRID_CLASS_LEDPHASE_format			"%c%03x%01x%02x%02x%02x%c",GRID_CONST_STX,GRID_CLASS_LEDPHASE_code,instruction,layernumber,lednumber,phase,GRID_CONST_ETX
#define GRID_CLASS_LEDPHASE_frame			"%c%03x_......%c",GRID_CONST_STX,GRID_CLASS_LEDPHASE_code,GRID_CONST_ETX

#define GRID_CLASS_LEDPHASE_LAYERNUMBER_offset		5
#define GRID_CLASS_LEDPHASE_LAYERNUMBER_length		2

#define GRID_CLASS_LEDPHASE_LEDNUMBER_offset		7
#define GRID_CLASS_LEDPHASE_LEDNUMBER_length		2

#define GRID_CLASS_LEDPHASE_PHASE_offset			9
#define GRID_CLASS_LEDPHASE_PHASE_length			2




#define GRID_TEMPLATE_PARAMETER_LIST_LENGTH     8

enum grid_template_parameter_index_t {
	
	GRID_TEMPLATE_PARAMETER_CONTROLLER_NUMBER,
	GRID_TEMPLATE_PARAMETER_CONTROLLER_NUMBER_REVERSED,
	GRID_TEMPLATE_PARAMETER_CONTROLLER_AV7,
	GRID_TEMPLATE_PARAMETER_CONTROLLER_AV8,
	GRID_TEMPLATE_PARAMETER_CONTROLLER_AV14U,
	GRID_TEMPLATE_PARAMETER_CONTROLLER_AV14L,
	GRID_TEMPLATE_PARAMETER_CONTROLLER_DV7,
	GRID_TEMPLATE_PARAMETER_CONTROLLER_DV8
	
};

#define GRID_PARAMETER_TEMPLATEINDEX_CNUMBER_code		"A0"
#define GRID_PARAMETER_TEMPLATEINDEX_CNUMBERREV_code	"A1"
#define GRID_PARAMETER_TEMPLATEINDEX_AV7_code			"A2"
#define GRID_PARAMETER_TEMPLATEINDEX_AV8_code			"A3"
#define GRID_PARAMETER_TEMPLATEINDEX_AV14U_code			"A4"
#define GRID_PARAMETER_TEMPLATEINDEX_AV14L_code			"A5"
#define GRID_PARAMETER_TEMPLATEINDEX_DV7_code			"A6"
#define GRID_PARAMETER_TEMPLATEINDEX_DV8_code			"A7"


enum grid_ui_element_t {
	
	GRID_UI_ELEMENT_SYSTEM,
	GRID_UI_ELEMENT_POTENTIOMETER,
	GRID_UI_ELEMENT_BUTTON,
	GRID_UI_ELEMENT_ENCODER,
	
};

#define GRID_PARAMETER_ELEMENTTYPE_SYSTEM_code			"00"
#define GRID_PARAMETER_ELEMENTTYPE_POTENTIOMETER_code	"01"
#define GRID_PARAMETER_ELEMENTTYPE_BUTTON_code			"02"
#define GRID_PARAMETER_ELEMENTTYPE_ENCODER_code			"03"


enum grid_ui_event_t {
	
	GRID_UI_EVENT_INIT,
	
	GRID_UI_EVENT_HEARTBEAT,
	
	GRID_UI_EVENT_AVC7,
	GRID_UI_EVENT_RVC,
	GRID_UI_EVENT_DVC,
	GRID_UI_EVENT_DP,
	GRID_UI_EVENT_DR,
	GRID_UI_EVENT_DD,
	
};

// ELEMENT TYPE + ELEMENT NUMBER + EVENT TYPE + PARAMETER

#define GRID_EVENT_HEARTBEAT	"\x02""050f00000100\x03"

#define GRID_EVENT_AVC7_POT		"\x02""050f0102A0A2\x03"

#define GRID_EVENT_DP_BUT		"\x02""050f0205A0A6\x03"
#define GRID_EVENT_DR_BUT		"\x02""050f0206A0A6\x03"

#define GRID_EVENT_AVC7_ENC		"\x02""050f0302A0A2\x03"
#define GRID_EVENT_DP_ENC		"\x02""050f0305A0A6\x03"
#define GRID_EVENT_DR_ENC		"\x02""050f0306A0A6\x03"

#define GRID_DEFAULT_ACTION_AVC7   "\x02""000f00b0A0A2\x03\x02""040f01A0A3\x03"
#define GRID_DEFAULT_ACTION_DP     "\x02""000f0090A0A6\x03\x02""040f01A0A7\x03"
#define GRID_DEFAULT_ACTION_DR     "\x02""000f0080A0A6\x03\x02""040f01A0A7\x03"


#define GRID_CLASS_EVENT_code						0x050
#define GRID_CLASS_EVENT_frame						"%c%03x_........%c",GRID_CONST_STX,GRID_CLASS_EVENT_code,GRID_CONST_ETX

#define GRID_CLASS_EVENT_ELEMENTTYPE_offset				5
#define GRID_CLASS_EVENT_ELEMENTTYPE_length				2

#define GRID_CLASS_EVENT_ELEMENTNUMBER_offset			7
#define GRID_CLASS_EVENT_ELEMENTNUMBER_length			2

#define GRID_CLASS_EVENT_EVENTTYPE_offset				9
#define GRID_CLASS_EVENT_EVENTTYPE_length				2

#define GRID_CLASS_EVENT_EVENTPARAM_offset				11
#define GRID_CLASS_EVENT_EVENTPARAM_length				2





#endif /* GRID_PROTOCOL_H_INCLUDED */