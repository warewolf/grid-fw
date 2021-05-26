/**
 * \file grid_protocol.h
 * \brief This file defines the communictaion classes and protocol constants of Grid.
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @\
 * or by an at symbol @@.
 * 
 * \author Suku Wc (Intech Studio)
 * \see https://intech.studio/
 * \see https://github.com/intechstudio/grid-fw/
 */


#ifndef GRID_PROTOCOL_H_INCLUDED
#define GRID_PROTOCOL_H_INCLUDED

#define GRID_PROTOCOL_VERSION_MAJOR 1
#define GRID_PROTOCOL_VERSION_MINOR 1
#define GRID_PROTOCOL_VERSION_PATCH 9

// Module HWCFG definitions


#define GRID_MODULE_PO16_RevB 0
#define GRID_MODULE_PO16_RevC 8

#define GRID_MODULE_BU16_RevB 128
#define GRID_MODULE_BU16_RevC 136

#define GRID_MODULE_PBF4_RevA 64

#define GRID_MODULE_EN16_RevA 192
#define GRID_MODULE_EN16_RevD 193

#define GRID_MODULE_EN16_ND_RevA 200
#define GRID_MODULE_EN16_ND_RevD 201


#define GRID_PARAMETER_HEARTBEAT_interval 250
#define GRID_PARAMETER_PING_interval 100

#define GRID_PARAMETER_TEXT_maxlength 150
#define GRID_PARAMETER_PACKET_maxlength 400
#define GRID_PARAMETER_PACKET_marign 200

// SPECIAL CHARACTERS

#define GRID_CONST_NUL 0x00

#define GRID_CONST_SOH 0x01 //start of header
#define GRID_CONST_STX 0x02 //start of text
#define GRID_CONST_ETX 0x03 //end of text
#define GRID_CONST_EOB 0x17 //end of block
#define GRID_CONST_EOT 0x04 //end of transmission

#define GRID_CONST_LF 0x0A //linefeed, newline

#define GRID_CONST_ACK 0x06 //acknowledge
#define GRID_CONST_NAK 0x15 //nacknowledge
#define GRID_CONST_CAN 0x18 //cancel

#define GRID_CONST_NORTH 0x11 // Device Control 1
#define GRID_CONST_EAST 0x12  // Device Control 2
#define GRID_CONST_SOUTH 0x13 // Device Control 3
#define GRID_CONST_WEST 0x14  // Device Control 4

#define GRID_CONST_DCT 0x0E // Shift In = Direct
#define GRID_CONST_BRC 0x0F // Shift Out = Broadcast

#define GRID_CONST_BELL 0x07

#define GRID_PARAMETER_MIDI_NOTEOFF 0x80
#define GRID_PARAMETER_MIDI_NOTEON 0x90
#define GRID_PARAMETER_MIDI_CONTROLCHANGE 0xB0

// HEADER BROADCAST

#define GRID_BRC_frame "%c%c....................%c", GRID_CONST_SOH, GRID_CONST_BRC, GRID_CONST_EOB

#define GRID_BRC_LEN_offset 2
#define GRID_BRC_LEN_length 4

#define GRID_BRC_ID_offset 6
#define GRID_BRC_ID_length 2

#define GRID_BRC_SESSION_offset 8
#define GRID_BRC_SESSION_length 2

#define GRID_BRC_SX_offset 10
#define GRID_BRC_SX_length 2

#define GRID_BRC_SY_offset 12
#define GRID_BRC_SY_length 2

#define GRID_BRC_DX_offset 14
#define GRID_BRC_DX_length 2

#define GRID_BRC_DY_offset 16
#define GRID_BRC_DY_length 2

#define GRID_BRC_ROT_offset 18
#define GRID_BRC_ROT_length 2

#define GRID_BRC_MSGAGE_offset 20
#define GRID_BRC_MSGAGE_length 2

#define GRID_INSTR_length 1
#define GRID_INSTR_offset 4

// Save the following action to the given event & change instruction to execute
#define GRID_INSTR_ACKNOWLEDGE_code 0xA //a

#define GRID_INSTR_NACKNOWLEDGE_code 0xB //b

#define GRID_INSTR_CONFIGURE_code 0xC //c

#define GRID_INSTR_REPORT_code 0xD //c

// Respond with executable please!
#define GRID_INSTR_FETCH_code 0xF //e

// Execute The Action if possible!
#define GRID_INSTR_EXECUTE_code 0xE //f

#define GRID_CLASS_length 3
#define GRID_CLASS_offset 1



// ================== MIDI CLASS =================== //
#define GRID_CLASS_MIDI_code 0x000
#define GRID_CLASS_MIDI_frame "%c%03x_........%c", GRID_CONST_STX, GRID_CLASS_MIDI_code, GRID_CONST_ETX

#define GRID_CLASS_MIDI_CHANNEL_offset 5
#define GRID_CLASS_MIDI_CHANNEL_length 2

#define GRID_CLASS_MIDI_COMMAND_offset 7
#define GRID_CLASS_MIDI_COMMAND_length 2

#define GRID_CLASS_MIDI_PARAM1_offset 9
#define GRID_CLASS_MIDI_PARAM1_length 2

#define GRID_CLASS_MIDI_PARAM2_offset 11
#define GRID_CLASS_MIDI_PARAM2_length 2



// HEARTBEAT (type=0 grid, type=1 gridmaster, type=255 editor)
#define GRID_CLASS_HEARTBEAT_code 0x010
#define GRID_CLASS_HEARTBEAT_frame "%c%03x_..........%c", GRID_CONST_STX, GRID_CLASS_HEARTBEAT_code, GRID_CONST_ETX

#define GRID_CLASS_HEARTBEAT_TYPE_offset 5
#define GRID_CLASS_HEARTBEAT_TYPE_length 2

#define GRID_CLASS_HEARTBEAT_HWCFG_offset 7
#define GRID_CLASS_HEARTBEAT_HWCFG_length 2

#define GRID_CLASS_HEARTBEAT_VMAJOR_offset 9
#define GRID_CLASS_HEARTBEAT_VMAJOR_length 2

#define GRID_CLASS_HEARTBEAT_VMINOR_offset 11
#define GRID_CLASS_HEARTBEAT_VMINOR_length 2

#define GRID_CLASS_HEARTBEAT_VPATCH_offset 13
#define GRID_CLASS_HEARTBEAT_VPATCH_length 2

// SERIAL NUMBER
#define GRID_CLASS_SERIALNUMBER_code 0x011
#define GRID_CLASS_SERIALNUMBER_frame "%c%03x_................................%c", GRID_CONST_STX, GRID_CLASS_SERIALNUMBER_code, GRID_CONST_ETX

#define GRID_CLASS_SERIALNUMBER_WORD0_offset 5
#define GRID_CLASS_SERIALNUMBER_WORD0_length 8

#define GRID_CLASS_SERIALNUMBER_WORD1_offset 13
#define GRID_CLASS_SERIALNUMBER_WORD1_length 8

#define GRID_CLASS_SERIALNUMBER_WORD2_offset 21
#define GRID_CLASS_SERIALNUMBER_WORD2_length 8

#define GRID_CLASS_SERIALNUMBER_WORD3_offset 29
#define GRID_CLASS_SERIALNUMBER_WORD3_length 8

// RESETCAUSE
#define GRID_CLASS_RESETCAUSE_code 0x012
#define GRID_CLASS_RESETCAUSE_frame "%c%03x_..%c", GRID_CONST_STX, GRID_CLASS_RESETCAUSE_code, GRID_CONST_ETX

#define GRID_CLASS_RESETCAUSE_CAUSE_offset 5
#define GRID_CLASS_RESETCAUSE_CAUSE_length 2

// RESET
#define GRID_CLASS_RESET_code 0x013
#define GRID_CLASS_RESET_frame "%c%03xe%c", GRID_CONST_STX, GRID_CLASS_RESET_code, GRID_CONST_ETX

// UPTIME
#define GRID_CLASS_UPTIME_code 0x014
#define GRID_CLASS_UPTIME_frame "%c%03x_........%c", GRID_CONST_STX, GRID_CLASS_UPTIME_code, GRID_CONST_ETX

#define GRID_CLASS_UPTIME_UPTIME_offset 5
#define GRID_CLASS_UPTIME_UPTIME_length 8

// DEBUGTEXT
#define GRID_CLASS_DEBUGTEXT_code 0x020
#define GRID_CLASS_DEBUGTEXT_frame_start "%c%03x_", GRID_CONST_STX, GRID_CLASS_DEBUGTEXT_code
#define GRID_CLASS_DEBUGTEXT_frame_end "%c", GRID_CONST_ETX

// DEBUGTASK
#define GRID_CLASS_DEBUGTASK_code 0x021
#define GRID_CLASS_DEBUGTASK_frame_start "%c%03x_....", GRID_CONST_STX, GRID_CLASS_DEBUGTASK_code
#define GRID_CLASS_DEBUGTASK_frame_end "%c", GRID_CONST_ETX

#define GRID_CLASS_DEBUGTASK_LENGTH_offset 5
#define GRID_CLASS_DEBUGTASK_LENGTH_length 4

#define GRID_CLASS_DEBUGTASK_OUTPUT_offset 9
#define GRID_CLASS_DEBUGTASK_OUTPUT_length 0


// PAGEACTIVE
#define GRID_CLASS_PAGEACTIVE_code 0x030
#define GRID_CLASS_PAGEACTIVE_frame "%c%03x_..%c", GRID_CONST_STX, GRID_CLASS_PAGEACTIVE_code, GRID_CONST_ETX

#define GRID_CLASS_PAGEACTIVE_PAGENUMBER_offset 5
#define GRID_CLASS_PAGEACTIVE_PAGENUMBER_length 2

// PAGECOUNT
#define GRID_CLASS_PAGECOUNT_code 0x031
#define GRID_CLASS_PAGECOUNT_frame "%c%03x_..%c", GRID_CONST_STX, GRID_CLASS_PAGECOUNT_code, GRID_CONST_ETX

#define GRID_CLASS_PAGECOUNT_PAGENUMBER_offset 5
#define GRID_CLASS_PAGECOUNT_PAGENUMBER_length 2

// LED SET PHASE

#define GRID_CLASS_LEDPHASE_code 0x040
#define GRID_CLASS_LEDPHASE_format "%c%03x%01x%02x%02x%02x%c", GRID_CONST_STX, GRID_CLASS_LEDPHASE_code, instruction, layernumber, lednumber, phase, GRID_CONST_ETX
#define GRID_CLASS_LEDPHASE_frame "%c%03x_......%c", GRID_CONST_STX, GRID_CLASS_LEDPHASE_code, GRID_CONST_ETX

#define GRID_CLASS_LEDPHASE_NUM_offset 5
#define GRID_CLASS_LEDPHASE_NUM_length 2

#define GRID_CLASS_LEDPHASE_LAY_offset 7
#define GRID_CLASS_LEDPHASE_LAY_length 2

#define GRID_CLASS_LEDPHASE_PHA_offset 9
#define GRID_CLASS_LEDPHASE_PHA_length 2

// LED SET COLOR

#define GRID_CLASS_LEDCOLOR_code 0x041
#define GRID_CLASS_LEDCOLOR_format "%c%03x%01x%02x%02x%02x%02x%02x%c", GRID_CONST_STX, GRID_CLASS_LEDPHASE_code, instruction, layernumber, lednumber, red, gre, blu, GRID_CONST_ETX
#define GRID_CLASS_LEDCOLOR_frame "%c%03x_..........%c", GRID_CONST_STX, GRID_CLASS_LEDCOLOR_code, GRID_CONST_ETX

#define GRID_CLASS_LEDCOLOR_NUM_offset 5
#define GRID_CLASS_LEDCOLOR_NUM_length 2

#define GRID_CLASS_LEDCOLOR_LAY_offset 7
#define GRID_CLASS_LEDCOLOR_LAY_length 2

#define GRID_CLASS_LEDCOLOR_RED_offset 9
#define GRID_CLASS_LEDCOLOR_RED_length 2

#define GRID_CLASS_LEDCOLOR_GRE_offset 11
#define GRID_CLASS_LEDCOLOR_GRE_length 2

#define GRID_CLASS_LEDCOLOR_BLU_offset 13
#define GRID_CLASS_LEDCOLOR_BLU_length 2

#define GRID_CLASS_LEDPREVIEW_code 0x042
#define GRID_CLASS_LEDPREVIEW_frame "%c%03x_............%c", GRID_CONST_STX, GRID_CLASS_LEDPREVIEW_code, GRID_CONST_ETX
#define GRID_CLASS_LEDPREVIEW_frame_start "%c%03x_....", GRID_CONST_STX, GRID_CLASS_LEDPREVIEW_code
#define GRID_CLASS_LEDPREVIEW_frame_end "%c", GRID_CONST_ETX

#define GRID_CLASS_LEDPREVIEW_LENGTH_offset 5
#define GRID_CLASS_LEDPREVIEW_LENGTH_length 4

#define GRID_CLASS_LEDPREVIEW_NUM_offset 9
#define GRID_CLASS_LEDPREVIEW_NUM_length 2

#define GRID_CLASS_LEDPREVIEW_RED_offset 11
#define GRID_CLASS_LEDPREVIEW_RED_length 2

#define GRID_CLASS_LEDPREVIEW_GRE_offset 13
#define GRID_CLASS_LEDPREVIEW_GRE_length 2

#define GRID_CLASS_LEDPREVIEW_BLU_offset 15
#define GRID_CLASS_LEDPREVIEW_BLU_length 2

#define GRID_TEMPLATE_UI_PARAMETER_LIST_LENGTH 20

#define GRID_LUA_FNC_G_LED_RED_human 	"led_default_red"
#define GRID_LUA_FNC_G_LED_RED_fnptr 	l_led_default_red
#define GRID_LUA_FNC_G_LED_RED_short 	"glr"

#define GRID_LUA_FNC_G_LED_GRE_human 	"led_default_green"
#define GRID_LUA_FNC_G_LED_GRE_fnptr 	l_led_default_green
#define GRID_LUA_FNC_G_LED_GRE_short 	"glg"

#define GRID_LUA_FNC_G_LED_BLU_human 	"led_default_blue"
#define GRID_LUA_FNC_G_LED_BLU_fnptr 	l_led_default_blue
#define GRID_LUA_FNC_G_LED_BLU_short 	"glb"


#define GRID_LUA_FNC_G_LED_PHASE_short 	"glp"
#define GRID_LUA_FNC_G_LED_PHASE_human 	"led_value"
#define GRID_LUA_FNC_G_LED_PHASE_fnptr 	l_grid_led_set_phase

#define GRID_LUA_FNC_G_LED_MIN_short 	"gln"
#define GRID_LUA_FNC_G_LED_MIN_human 	"led_color_min"
#define GRID_LUA_FNC_G_LED_MIN_fnptr 	l_grid_led_set_min

#define GRID_LUA_FNC_G_LED_MID_short 	"gld"
#define GRID_LUA_FNC_G_LED_MID_human 	"led_color_mid"
#define GRID_LUA_FNC_G_LED_MID_fnptr 	l_grid_led_set_mid

#define GRID_LUA_FNC_G_LED_MAX_short 	"glx"
#define GRID_LUA_FNC_G_LED_MAX_human 	"led_color_max"
#define GRID_LUA_FNC_G_LED_MAX_fnptr 	l_grid_led_set_max

#define GRID_LUA_FNC_G_LED_COLOR_short 	"glc"
#define GRID_LUA_FNC_G_LED_COLOR_human 	"led_color"
#define GRID_LUA_FNC_G_LED_COLOR_fnptr 	l_grid_led_set_color

#define GRID_LUA_FNC_G_LED_FREQUENCY_short 	"glf"
#define GRID_LUA_FNC_G_LED_FREQUENCY_human 	"led_animation_rate"
#define GRID_LUA_FNC_G_LED_FREQUENCY_fnptr 	l_grid_led_set_frequency

#define GRID_LUA_FNC_G_LED_SHAPE_short 	"gls"
#define GRID_LUA_FNC_G_LED_SHAPE_human 	"led_animation_type"
#define GRID_LUA_FNC_G_LED_SHAPE_fnptr 	l_grid_led_set_phase

#define GRID_LUA_FNC_G_LED_PSF_short 	"glpfs"
#define GRID_LUA_FNC_G_LED_PSF_human 	"led_animation_phase_rate_type"
#define GRID_LUA_FNC_G_LED_PSF_fnptr 	l_grid_led_set_pfs

#define GRID_LUA_FNC_G_MIDI_SEND_short 	"gms"
#define GRID_LUA_FNC_G_MIDI_SEND_human 	"midi_send"
#define GRID_LUA_FNC_G_MIDI_SEND_fnptr 	l_grid_midi_send


#define GRID_LUA_FNC_G_KEYBOARD_SEND_short 	"gks"
#define GRID_LUA_FNC_G_KEYBOARD_SEND_human 	"keyboard_send"
#define GRID_LUA_FNC_G_KEYBOARD_SEND_fnptr 	l_grid_keyboard_send


#define GRID_LUA_FNC_G_RANDOM_short 	"grnd"
#define GRID_LUA_FNC_G_RANDOM_human 	"random"
#define GRID_LUA_FNC_G_RANDOM_fnptr 	l_grid_random

#define GRID_LUA_FNC_G_HWCFG_short 		"ghwcfg"
#define GRID_LUA_FNC_G_HWCFG_human 		"hardware_configuration"
#define GRID_LUA_FNC_G_HWCFG_fnptr 		l_grid_hwcfg

#define GRID_LUA_FNC_G_VERSION_MAJOR_short 		"gvmaj"
#define GRID_LUA_FNC_G_VERSION_MAJOR_human 		"version_major"
#define GRID_LUA_FNC_G_VERSION_MAJOR_fnptr 		l_grid_version_major

#define GRID_LUA_FNC_G_VERSION_MINOR_short 		"gvmin"
#define GRID_LUA_FNC_G_VERSION_MINOR_human 		"version_minor"
#define GRID_LUA_FNC_G_VERSION_MINOR_fnptr 		l_grid_version_minor

#define GRID_LUA_FNC_G_VERSION_PATCH_short 		"gvpat"
#define GRID_LUA_FNC_G_VERSION_PATCH_human 		"version_patch"
#define GRID_LUA_FNC_G_VERSION_PATCH_fnptr 		l_grid_version_patch

#define GRID_LUA_FNC_G_MODULE_POSX_short 		"gmx"
#define GRID_LUA_FNC_G_MODULE_POSX_human 		"module_position_x"
#define GRID_LUA_FNC_G_MODULE_POSX_fnptr 		l_grid_position_x

#define GRID_LUA_FNC_G_MODULE_POSY_short 		"gmy"
#define GRID_LUA_FNC_G_MODULE_POSY_human 		"module_position_y"
#define GRID_LUA_FNC_G_MODULE_POSY_fnptr 		l_grid_position_y

#define GRID_LUA_FNC_G_MODULE_ROT_short 		"gmr"
#define GRID_LUA_FNC_G_MODULE_ROT_human 		"module_rotation"
#define GRID_LUA_FNC_G_MODULE_ROT_fnptr 		l_grid_rotation


#define GRID_LUA_FNC_G_PAGE_NEXT_short 			"gpn"
#define GRID_LUA_FNC_G_PAGE_NEXT_human 			"page_next"
#define GRID_LUA_FNC_G_PAGE_NEXT_fnptr 			l_grid_page_next

#define GRID_LUA_FNC_G_PAGE_PREV_short 			"gpp"
#define GRID_LUA_FNC_G_PAGE_PREV_human 			"page_prev"
#define GRID_LUA_FNC_G_PAGE_PREV_fnptr 			l_grid_page_prev

#define GRID_LUA_FNC_G_PAGE_LOAD_short 			"gpl"
#define GRID_LUA_FNC_G_PAGE_LOAD_human 			"page_load"
#define GRID_LUA_FNC_G_PAGE_LOAD_fnptr 			l_grid_page_load

#define GRID_LUA_INIT_SCRIPT_1 "print('Hardware: ', ghwcfg(), ' Version: ', gvmaj(), gvmin(), gvpat())"
#define GRID_LUA_INIT_SCRIPT_2 "print('Random: ', grnd(), grnd(), grnd(), grnd())"
#define GRID_LUA_INIT_SCRIPT_3 "print('X, Y, Rot: ', gmx(), gmy(), gmr())"

/*

hwcfg
random_number_generator
uptime
resetcause
memory stats
version
stored config version
stored config date

*/


// ========================= ENCODER =========================== //

#define GRID_LUA_FNC_E_ELEMENT_INDEX_index 0
#define GRID_LUA_FNC_E_ELEMENT_INDEX_helper "0"
#define GRID_LUA_FNC_E_ELEMENT_INDEX_short "ind"
#define GRID_LUA_FNC_E_ELEMENT_INDEX_human "element_index"

#define GRID_LUA_FNC_E_BUTTON_NUMBER_index 1
#define GRID_LUA_FNC_E_BUTTON_NUMBER_helper "1"
#define GRID_LUA_FNC_E_BUTTON_NUMBER_short "bnu"
#define GRID_LUA_FNC_E_BUTTON_NUMBER_human "button_number"

#define GRID_LUA_FNC_E_BUTTON_VALUE_index 2
#define GRID_LUA_FNC_E_BUTTON_VALUE_helper "2"
#define GRID_LUA_FNC_E_BUTTON_VALUE_short "bva"
#define GRID_LUA_FNC_E_BUTTON_VALUE_human "button_value"

#define GRID_LUA_FNC_E_BUTTON_MIN_index 3
#define GRID_LUA_FNC_E_BUTTON_MIN_helper "3"
#define GRID_LUA_FNC_E_BUTTON_MIN_short "bmi"
#define GRID_LUA_FNC_E_BUTTON_MIN_human "button_min"

#define GRID_LUA_FNC_E_BUTTON_MAX_index 4
#define GRID_LUA_FNC_E_BUTTON_MAX_helper "4"
#define GRID_LUA_FNC_E_BUTTON_MAX_short "bma"
#define GRID_LUA_FNC_E_BUTTON_MAX_human "button_max"

#define GRID_LUA_FNC_E_BUTTON_MODE_index 5
#define GRID_LUA_FNC_E_BUTTON_MODE_helper "5"
#define GRID_LUA_FNC_E_BUTTON_MODE_short "bmo"
#define GRID_LUA_FNC_E_BUTTON_MODE_human "button_mode"

#define GRID_LUA_FNC_E_BUTTON_ELAPSED_index 6
#define GRID_LUA_FNC_E_BUTTON_ELAPSED_helper "6"
#define GRID_LUA_FNC_E_BUTTON_ELAPSED_short "bel"
#define GRID_LUA_FNC_E_BUTTON_ELAPSED_human "button_elapsed_time"

#define GRID_LUA_FNC_E_BUTTON_STATE_index 7
#define GRID_LUA_FNC_E_BUTTON_STATE_helper "7"
#define GRID_LUA_FNC_E_BUTTON_STATE_short "bst"
#define GRID_LUA_FNC_E_BUTTON_STATE_human "button_state"

#define GRID_LUA_FNC_E_ENCODER_NUMBER_index 8
#define GRID_LUA_FNC_E_ENCODER_NUMBER_helper "8"
#define GRID_LUA_FNC_E_ENCODER_NUMBER_short "enu"
#define GRID_LUA_FNC_E_ENCODER_NUMBER_human "encoder_number"

#define GRID_LUA_FNC_E_ENCODER_VALUE_index 9
#define GRID_LUA_FNC_E_ENCODER_VALUE_helper "9"
#define GRID_LUA_FNC_E_ENCODER_VALUE_short "eva"
#define GRID_LUA_FNC_E_ENCODER_VALUE_human "encoder_value"

#define GRID_LUA_FNC_E_ENCODER_MIN_index 10
#define GRID_LUA_FNC_E_ENCODER_MIN_helper "10"
#define GRID_LUA_FNC_E_ENCODER_MIN_short "emi"
#define GRID_LUA_FNC_E_ENCODER_MIN_human "encoder_min"

#define GRID_LUA_FNC_E_ENCODER_MAX_index 11
#define GRID_LUA_FNC_E_ENCODER_MAX_helper "11"
#define GRID_LUA_FNC_E_ENCODER_MAX_short "ema"
#define GRID_LUA_FNC_E_ENCODER_MAX_human "encoder_max"

#define GRID_LUA_FNC_E_ENCODER_MODE_index 12
#define GRID_LUA_FNC_E_ENCODER_MODE_helper "12"
#define GRID_LUA_FNC_E_ENCODER_MODE_short "emo"
#define GRID_LUA_FNC_E_ENCODER_MODE_human "encoder_mode"

#define GRID_LUA_FNC_E_ENCODER_ELAPSED_index 13
#define GRID_LUA_FNC_E_ENCODER_ELAPSED_helper "13"
#define GRID_LUA_FNC_E_ENCODER_ELAPSED_short "eel"
#define GRID_LUA_FNC_E_ENCODER_ELAPSED_human "encoder_elapsed_time"

#define GRID_LUA_FNC_E_ENCODER_STATE_index 14
#define GRID_LUA_FNC_E_ENCODER_STATE_helper "14"
#define GRID_LUA_FNC_E_ENCODER_STATE_short "est"
#define GRID_LUA_FNC_E_ENCODER_STATE_human "encoder_state"

#define GRID_LUA_FNC_E_ACTION_ENCODERCHANGE_short "ec"
#define GRID_LUA_FNC_E_ACTION_ENCODERCHANGE_human "encoder_change"

#define GRID_LUA_FNC_E_ACTION_BUTTONCHANGE_short "bc"
#define GRID_LUA_FNC_E_ACTION_BUTTONCHANGE_human "button_change"


// Encoder parameters
#define GRID_LUA_FNC_E_LIST_length 15

// Encoder init function
#define GRID_LUA_E_LIST_init "init_encoder = function (e, i) \
	 \
	e." GRID_LUA_FNC_E_ELEMENT_INDEX_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_ELEMENT_INDEX_helper ", a) end \
	\
	e." GRID_LUA_FNC_E_BUTTON_NUMBER_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_BUTTON_NUMBER_helper ", a) end \
	e." GRID_LUA_FNC_E_BUTTON_VALUE_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_BUTTON_VALUE_helper ", a) end \
	e." GRID_LUA_FNC_E_BUTTON_MIN_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_BUTTON_MIN_helper ", a) end \
	e." GRID_LUA_FNC_E_BUTTON_MAX_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_BUTTON_MAX_helper ", a) end \
	e." GRID_LUA_FNC_E_BUTTON_MODE_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_BUTTON_MODE_helper ", a) end \
	e." GRID_LUA_FNC_E_BUTTON_ELAPSED_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_BUTTON_ELAPSED_helper ", a) end \
	e." GRID_LUA_FNC_E_BUTTON_STATE_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_BUTTON_STATE_helper ", a) end \
	\
	e." GRID_LUA_FNC_E_ENCODER_NUMBER_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_ENCODER_NUMBER_helper ", a) end \
	e." GRID_LUA_FNC_E_ENCODER_VALUE_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_ENCODER_VALUE_helper ", a) end \
	e." GRID_LUA_FNC_E_ENCODER_MIN_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_ENCODER_MIN_helper ", a) end \
	e." GRID_LUA_FNC_E_ENCODER_MAX_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_ENCODER_MAX_helper ", a) end \
	e." GRID_LUA_FNC_E_ENCODER_MODE_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_ENCODER_MODE_helper ", a) end \
	e." GRID_LUA_FNC_E_ENCODER_ELAPSED_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_ENCODER_ELAPSED_helper ", a) end \
	e." GRID_LUA_FNC_E_ENCODER_STATE_short "=function (a) return gtv(i, " GRID_LUA_FNC_E_ENCODER_STATE_helper ", a) end \
	\
	e."GRID_LUA_FNC_E_ACTION_ENCODERCHANGE_short" = function (a) print('undefined action') end\
	e."GRID_LUA_FNC_E_ACTION_BUTTONCHANGE_short" = function (a) print('undefined action') end\
	\
    end"


// ========================= POTMETER =========================== //

#define GRID_LUA_FNC_P_ELEMENT_INDEX_index 0
#define GRID_LUA_FNC_P_ELEMENT_INDEX_helper "0"
#define GRID_LUA_FNC_P_ELEMENT_INDEX_short "ind"
#define GRID_LUA_FNC_P_ELEMENT_INDEX_human "element_index"

#define GRID_LUA_FNC_P_POTMETER_NUMBER_index 1
#define GRID_LUA_FNC_P_POTMETER_NUMBER_helper "1"
#define GRID_LUA_FNC_P_POTMETER_NUMBER_short "pnu"
#define GRID_LUA_FNC_P_POTMETER_NUMBER_human "potmeter_number"

#define GRID_LUA_FNC_P_POTMETER_VALUE_index 2
#define GRID_LUA_FNC_P_POTMETER_VALUE_helper "2"
#define GRID_LUA_FNC_P_POTMETER_VALUE_short "pva"
#define GRID_LUA_FNC_P_POTMETER_VALUE_human "potmeter_value"

#define GRID_LUA_FNC_P_POTMETER_MIN_index 3
#define GRID_LUA_FNC_P_POTMETER_MIN_helper "3"
#define GRID_LUA_FNC_P_POTMETER_MIN_short "pmi"
#define GRID_LUA_FNC_P_POTMETER_MIN_human "potmeter_min"

#define GRID_LUA_FNC_P_POTMETER_MAX_index 4
#define GRID_LUA_FNC_P_POTMETER_MAX_helper "4"
#define GRID_LUA_FNC_P_POTMETER_MAX_short "pma"
#define GRID_LUA_FNC_P_POTMETER_MAX_human "potmeter_max"

#define GRID_LUA_FNC_P_POTMETER_MODE_index 5
#define GRID_LUA_FNC_P_POTMETER_MODE_helper "5"
#define GRID_LUA_FNC_P_POTMETER_MODE_short "pmo"
#define GRID_LUA_FNC_P_POTMETER_MODE_human "potmeter_resolution"

#define GRID_LUA_FNC_P_POTMETER_ELAPSED_index 6
#define GRID_LUA_FNC_P_POTMETER_ELAPSED_helper "6"
#define GRID_LUA_FNC_P_POTMETER_ELAPSED_short "pel"
#define GRID_LUA_FNC_P_POTMETER_ELAPSED_human "potmeter_elapsed_time"

#define GRID_LUA_FNC_P_POTMETER_STATE_index 7
#define GRID_LUA_FNC_P_POTMETER_STATE_helper "7"
#define GRID_LUA_FNC_P_POTMETER_STATE_short "pst"
#define GRID_LUA_FNC_P_POTMETER_STATE_human "potmeter_state"

#define GRID_LUA_FNC_P_ACTION_POTMETERCHANGE_short "pc"
#define GRID_LUA_FNC_P_ACTION_POTMETERCHANGE_human "potmeter_change"

// Encoder parameters
#define GRID_LUA_FNC_P_LIST_length 8

// Encoder init function
#define GRID_LUA_P_LIST_init "init_potmeter = function (e, i) \
	 \
	e." GRID_LUA_FNC_P_ELEMENT_INDEX_short "=function (a) return gtv(i, " GRID_LUA_FNC_P_ELEMENT_INDEX_helper ", a) end \
	\
	e." GRID_LUA_FNC_P_POTMETER_NUMBER_short "=function (a) return gtv(i, " GRID_LUA_FNC_P_POTMETER_NUMBER_helper ", a) end \
	e." GRID_LUA_FNC_P_POTMETER_VALUE_short "=function (a) return gtv(i, " GRID_LUA_FNC_P_POTMETER_VALUE_helper ", a) end \
	e." GRID_LUA_FNC_P_POTMETER_MIN_short "=function (a) return gtv(i, " GRID_LUA_FNC_P_POTMETER_MIN_helper ", a) end \
	e." GRID_LUA_FNC_P_POTMETER_MAX_short "=function (a) return gtv(i, " GRID_LUA_FNC_P_POTMETER_MAX_helper ", a) end \
	e." GRID_LUA_FNC_P_POTMETER_MODE_short "=function (a) return gtv(i, " GRID_LUA_FNC_P_POTMETER_MODE_helper ", a) end \
	e." GRID_LUA_FNC_P_POTMETER_ELAPSED_short "=function (a) return gtv(i, " GRID_LUA_FNC_P_POTMETER_ELAPSED_helper ", a) end \
	e." GRID_LUA_FNC_P_POTMETER_STATE_short "=function (a) return gtv(i, " GRID_LUA_FNC_P_POTMETER_STATE_helper ", a) end \
	\
	e."GRID_LUA_FNC_P_ACTION_POTMETERCHANGE_short" = function (a) print('undefined action') end\
    end"


// ========================= BUTTON =========================== //

#define GRID_LUA_FNC_B_ELEMENT_INDEX_index 0
#define GRID_LUA_FNC_B_ELEMENT_INDEX_helper "0"
#define GRID_LUA_FNC_B_ELEMENT_INDEX_short "ind"
#define GRID_LUA_FNC_B_ELEMENT_INDEX_human "element_index"

#define GRID_LUA_FNC_B_BUTTON_NUMBER_index 1
#define GRID_LUA_FNC_B_BUTTON_NUMBER_helper "1"
#define GRID_LUA_FNC_B_BUTTON_NUMBER_short "bnu"
#define GRID_LUA_FNC_B_BUTTON_NUMBER_human "button_number"

#define GRID_LUA_FNC_B_BUTTON_VALUE_index 2
#define GRID_LUA_FNC_B_BUTTON_VALUE_helper "2"
#define GRID_LUA_FNC_B_BUTTON_VALUE_short "bva"
#define GRID_LUA_FNC_B_BUTTON_VALUE_human "button_value"

#define GRID_LUA_FNC_B_BUTTON_MIN_index 3
#define GRID_LUA_FNC_B_BUTTON_MIN_helper "3"
#define GRID_LUA_FNC_B_BUTTON_MIN_short "bmi"
#define GRID_LUA_FNC_B_BUTTON_MIN_human "button_min"

#define GRID_LUA_FNC_B_BUTTON_MAX_index 4
#define GRID_LUA_FNC_B_BUTTON_MAX_helper "4"
#define GRID_LUA_FNC_B_BUTTON_MAX_short "bma"
#define GRID_LUA_FNC_B_BUTTON_MAX_human "button_max"

#define GRID_LUA_FNC_B_BUTTON_MODE_index 5
#define GRID_LUA_FNC_B_BUTTON_MODE_helper "5"
#define GRID_LUA_FNC_B_BUTTON_MODE_short "bmo"
#define GRID_LUA_FNC_B_BUTTON_MODE_human "button_resolution"

#define GRID_LUA_FNC_B_BUTTON_ELAPSED_index 6
#define GRID_LUA_FNC_B_BUTTON_ELAPSED_helper "6"
#define GRID_LUA_FNC_B_BUTTON_ELAPSED_short "bel"
#define GRID_LUA_FNC_B_BUTTON_ELAPSED_human "button_elapsed_time"

#define GRID_LUA_FNC_B_BUTTON_STATE_index 7
#define GRID_LUA_FNC_B_BUTTON_STATE_helper "7"
#define GRID_LUA_FNC_B_BUTTON_STATE_short "bst"
#define GRID_LUA_FNC_B_BUTTON_STATE_human "button_state"

#define GRID_LUA_FNC_B_ACTION_BUTTONCHANGE_short "bc"
#define GRID_LUA_FNC_B_ACTION_BUTTONCHANGE_human "button_change"

// Encoder parameters
#define GRID_LUA_FNC_B_LIST_length 8

// Encoder init function
#define GRID_LUA_B_LIST_init "init_button = function (e, i) \
	 \
	e." GRID_LUA_FNC_B_ELEMENT_INDEX_short "=function (a) return gtv(i, " GRID_LUA_FNC_B_ELEMENT_INDEX_helper ", a) end \
	\
	e." GRID_LUA_FNC_B_BUTTON_NUMBER_short "=function (a) return gtv(i, " GRID_LUA_FNC_B_BUTTON_NUMBER_helper ", a) end \
	e." GRID_LUA_FNC_B_BUTTON_VALUE_short "=function (a) return gtv(i, " GRID_LUA_FNC_B_BUTTON_VALUE_helper ", a) end \
	e." GRID_LUA_FNC_B_BUTTON_MIN_short "=function (a) return gtv(i, " GRID_LUA_FNC_B_BUTTON_MIN_helper ", a) end \
	e." GRID_LUA_FNC_B_BUTTON_MAX_short "=function (a) return gtv(i, " GRID_LUA_FNC_B_BUTTON_MAX_helper ", a) end \
	e." GRID_LUA_FNC_B_BUTTON_MODE_short "=function (a) return gtv(i, " GRID_LUA_FNC_B_BUTTON_MODE_helper ", a) end \
	e." GRID_LUA_FNC_B_BUTTON_ELAPSED_short "=function (a) return gtv(i, " GRID_LUA_FNC_B_BUTTON_ELAPSED_helper ", a) end \
	e." GRID_LUA_FNC_B_BUTTON_STATE_short "=function (a) return gtv(i, " GRID_LUA_FNC_B_BUTTON_STATE_helper ", a) end \
	\
	e."GRID_LUA_FNC_B_ACTION_BUTTONCHANGE_short" = function (a) print('undefined action') end\
    end"


#define GRID_LUA_KW_ELEMENT_short "ele"
#define GRID_LUA_KW_THIS_short "this"

// element[4].encoder_value()
// ele[4].eva()

// Global parameters
#define GRID_TEMPLATE_Z_PARAMETER_LIST_LENGTH 6

enum grid_template_z_parameter_index_t
{

	GRID_TEMPLATE_Z_PARAMETER_BANK_NUMBER_ACTIVE,

	GRID_TEMPLATE_Z_PARAMETER_BANK_COLOR_RED,
	GRID_TEMPLATE_Z_PARAMETER_BANK_COLOR_GRE,
	GRID_TEMPLATE_Z_PARAMETER_BANK_COLOR_BLU,

	GRID_TEMPLATE_Z_PARAMETER_MAPMODE_STATE,
	GRID_TEMPLATE_Z_PARAMETER_BANK_NEXT,

};

enum grid_ui_element_t
{

	GRID_UI_ELEMENT_SYSTEM,
	GRID_UI_ELEMENT_POTENTIOMETER,
	GRID_UI_ELEMENT_BUTTON,
	GRID_UI_ELEMENT_ENCODER,

};

#define GRID_PARAMETER_ELEMENTTYPE_SYSTEM_code "00"
#define GRID_PARAMETER_ELEMENTTYPE_POTENTIOMETER_code "01"
#define GRID_PARAMETER_ELEMENTTYPE_BUTTON_code "02"
#define GRID_PARAMETER_ELEMENTTYPE_ENCODER_code "03"

enum grid_ui_event_t
{

	GRID_UI_EVENT_INIT,

	GRID_UI_EVENT_AC,
	GRID_UI_EVENT_EC,
	GRID_UI_EVENT_BC,
	GRID_UI_EVENT_MAPMODE_CHANGE,


};

#define 	GRID_EVENT_INIT	"00"
#define 	GRID_EVENT_AC	"01"
#define 	GRID_EVENT_EC	"02"
#define 	GRID_EVENT_BC	"03"
#define 	GRID_EVENT_MAP	"04"

// BANK + ELEMENT NUMBER + EVENT TYPE + PARAMETER

// DEFAULT ACTION:                     FIRST MIDI then LED_SET_PHASE


#define GRID_ACTIONSTRING_INIT_POT "<?lua --[[@glc]]glc(this.ind(), 1, glr(), glg(), glb()) --[[@glp]]glp(this.ind(), 1, this.pva()) ?>"
#define GRID_ACTIONSTRING_INIT_BUT "<?lua --[[@glc]]glc(this.ind(), 1, glr(), glg(), glb()) --[[@glp]]glp(this.ind(), 1, this.bva()) ?>"
#define GRID_ACTIONSTRING_INIT_ENC "<?lua --[[@glc]]glc(this.ind(), 1, glr(), glg(), glb()) --[[@glc]]glc(this.ind(), 2, glr(), glg(), glb()) --[[@glp]]glp(this.ind(), 1, this.bva()) --[[@glp]]glp(this.ind(), 2, this.eva()) ?>"

// locals
//#define GRID_ACTIONSTRING_AC "<?lua --[[@gms]]gms(0, 176, this.ind(), this.pva()) --[[@glp]]glp(this.ind(), 1, this.pva()) ?>"
#define GRID_ACTIONSTRING_AC "<?lua --[[@cb]]local num, val = this.ind(), this.pva() --[[@gms]]gms(0, 176, num, val) --[[@glp]]glp(num, 1, val) ?>"

// locals
//#define GRID_ACTIONSTRING_BC "<?lua --[[@l]]local num = this.ind() local val = this.bva() --[[@gms]]gms(0, 144, num, val) --[[@glp]]glp(num, 1, val) ?>"
#define GRID_ACTIONSTRING_BC "<?lua --[[@cb]]local num, val = this.ind(), this.bva() --[[@gms]]gms(0, 144, num, val) --[[@glp]]glp(num, 1, val) ?>"

//#define GRID_ACTIONSTRING_EC "<?lua --[[@gms]]gms(0, 176, this.ind(), this.eva()) --[[@glp]]glp(this.ind(), 2, this.eva()) ?>"
#define GRID_ACTIONSTRING_EC "<?lua --[[@cb]]local num, val = this.ind(), this.eva() --[[@gms]]gms(0, 176, num, val) --[[@glp]]glp(num, 2, val) ?>"



#define GRID_ACTIONSTRING_PAGE_INIT		 "<?lua --[[@cb]]print('page init') ?>"
#define GRID_ACTIONSTRING_MAPMODE_CHANGE "<?lua --[[@cb]]gpl(gpn()) ?>"


#define GRID_CLASS_EVENT_code 0x050
#define GRID_CLASS_EVENT_frame "%c%03x_........%c", GRID_CONST_STX, GRID_CLASS_EVENT_code, GRID_CONST_ETX

#define GRID_CLASS_EVENT_PAGENUMBER_offset 5
#define GRID_CLASS_EVENT_PAGENUMBER_length 2

#define GRID_CLASS_EVENT_ELEMENTNUMBER_offset 7
#define GRID_CLASS_EVENT_ELEMENTNUMBER_length 2

#define GRID_CLASS_EVENT_EVENTTYPE_offset 9
#define GRID_CLASS_EVENT_EVENTTYPE_length 2

#define GRID_CLASS_EVENT_EVENTPARAM_offset 11
#define GRID_CLASS_EVENT_EVENTPARAM_length 2


#define GRID_CLASS_EVENTPREVIEW_code 0x051
#define GRID_CLASS_EVENTPREVIEW_frame "%c%03x_........%c", GRID_CONST_STX, GRID_CLASS_EVENTPREVIEW_code, GRID_CONST_ETX
#define GRID_CLASS_EVENTPREVIEW_frame_start "%c%03x_....", GRID_CONST_STX, GRID_CLASS_EVENTPREVIEW_code
#define GRID_CLASS_EVENTPREVIEW_frame_end "%c", GRID_CONST_ETX

#define GRID_CLASS_EVENTPREVIEW_LENGTH_offset 5
#define GRID_CLASS_EVENTPREVIEW_LENGTH_length 4

#define GRID_CLASS_EVENTPREVIEW_NUM_offset 9
#define GRID_CLASS_EVENTPREVIEW_NUM_length 2

#define GRID_CLASS_EVENTPREVIEW_VALUE_offset 11
#define GRID_CLASS_EVENTPREVIEW_VALUE_length 2

// CONFIG STORE     Fetch(Read) Configure(Overwrite) Append(Write)
#define GRID_CLASS_CONFIG_code 0x060
#define GRID_CLASS_CONFIG_frame "%c%03x_................%c", GRID_CONST_STX, GRID_CLASS_CONFIG_code, GRID_CONST_ETX
#define GRID_CLASS_CONFIG_frame_start "%c%03x_................", GRID_CONST_STX, GRID_CLASS_CONFIG_code
#define GRID_CLASS_CONFIG_frame_end "%c", GRID_CONST_ETX

#define GRID_CLASS_CONFIG_VERSIONMAJOR_offset 5
#define GRID_CLASS_CONFIG_VERSIONMAJOR_length 2

#define GRID_CLASS_CONFIG_VERSIONMINOR_offset 7
#define GRID_CLASS_CONFIG_VERSIONMINOR_length 2

#define GRID_CLASS_CONFIG_VERSIONPATCH_offset 9
#define GRID_CLASS_CONFIG_VERSIONPATCH_length 2

#define GRID_CLASS_CONFIG_PAGENUMBER_offset 11
#define GRID_CLASS_CONFIG_PAGENUMBER_length 2

#define GRID_CLASS_CONFIG_ELEMENTNUMBER_offset 13
#define GRID_CLASS_CONFIG_ELEMENTNUMBER_length 2

#define GRID_CLASS_CONFIG_EVENTTYPE_offset 15
#define GRID_CLASS_CONFIG_EVENTTYPE_length 2

#define GRID_CLASS_CONFIG_ACTIONLENGTH_offset 17
#define GRID_CLASS_CONFIG_ACTIONLENGTH_length 4

#define GRID_CLASS_CONFIG_ACTIONSTRING_offset 21
#define GRID_CLASS_CONFIG_ACTIONSTRING_length 0


#define GRID_CLASS_CONFIGSTORE_code 0x061
#define GRID_CLASS_CONFIGSTORE_frame "%c%03x_%c", GRID_CONST_STX, GRID_CLASS_CONFIGSTORE_code, GRID_CONST_ETX

#define GRID_CLASS_CONFIGERASE_code 0x062
#define GRID_CLASS_CONFIGERASE_frame "%c%03x_%c", GRID_CONST_STX, GRID_CLASS_CONFIGERASE_code, GRID_CONST_ETX


// RUN imediate
#define GRID_CLASS_IMEDIATE_code 0x085
#define GRID_CLASS_IMEDIATE_frame_start "%c%03x_....", GRID_CONST_STX, GRID_CLASS_IMEDIATE_code
#define GRID_CLASS_IMEDIATE_frame_end "%c", GRID_CONST_ETX

#define GRID_CLASS_IMEDIATE_ACTIONLENGTH_offset 5
#define GRID_CLASS_IMEDIATE_ACTIONLENGTH_length 4

#define GRID_CLASS_IMEDIATE_ACTIONSTRING_offset 9
#define GRID_CLASS_IMEDIATE_ACTIONSTRING_length 0

// 090 HID KEYBOARD STATUS

#define GRID_CLASS_HIDKEYSTATUS_code 0x090
#define GRID_CLASS_HIDKEYSTATUS_frame "%c%03x_..%c", GRID_CONST_STX, GRID_CLASS_HIDKEYSTATUS_code, GRID_CONST_ETX

#define GRID_CLASS_HIDKEYSTATUS_ISENABLED_offset 5
#define GRID_CLASS_HIDKEYSTATUS_ISENABLED_length 2

// 091 HID KEYBOARD LOWLEVEL KEYPRESS/RELEASE

#define GRID_CLASS_HIDKEYBOARD_code 0x091
#define GRID_CLASS_HIDKEYBOARD_frame "%c%03x_........%c", GRID_CONST_STX, GRID_CLASS_HIDKEYBOARD_code, GRID_CONST_ETX
#define GRID_CLASS_HIDKEYBOARD_frame_start "%c%03x_........", GRID_CONST_STX, GRID_CLASS_HIDKEYBOARD_code
#define GRID_CLASS_HIDKEYBOARD_frame_end "%c", GRID_CONST_ETX


#define GRID_CLASS_HIDKEYBOARD_DEFAULTDELAY_offset 5
#define GRID_CLASS_HIDKEYBOARD_DEFAULTDELAY_length 2

#define GRID_CLASS_HIDKEYBOARD_LENGTH_offset 7
#define GRID_CLASS_HIDKEYBOARD_LENGTH_length 2

#define GRID_CLASS_HIDKEYBOARD_KEYISMODIFIER_offset 9
#define GRID_CLASS_HIDKEYBOARD_KEYISMODIFIER_length 1

#define GRID_CLASS_HIDKEYBOARD_KEYSTATE_offset 10
#define GRID_CLASS_HIDKEYBOARD_KEYSTATE_length 1

#define GRID_CLASS_HIDKEYBOARD_KEYCODE_offset 11
#define GRID_CLASS_HIDKEYBOARD_KEYCODE_length 2

#define GRID_CLASS_HIDKEYBOARD_DELAY_offset 10
#define GRID_CLASS_HIDKEYBOARD_DELAY_length 3

#endif /* GRID_PROTOCOL_H_INCLUDED */