/*
  Report.c - reporting and messaging methods
  Part of Grbl-Advanced

  Copyright (c) 2012-2016 Sungeun K. Jeon for Gnea Research LLC
  Copyright (c)	2017-2019 Patrick F.

  Grbl-Advanced is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl-Advanced is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl-Advanced.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
  This file functions as the primary feedback interface for Grbl. Any outgoing data, such
  as the protocol status messages, feedback messages, and status reports, are stored here.
  For the most part, these functions primarily are called from protocol.c methods. If a
  different style feedback is desired (i.e. JSON), then a user can change these following
  methods to accomodate their needs.
*/
#include <string.h>
#include "util.h"
#include "Config.h"
#include "GCode.h"
#include "Limits.h"
#include "Probe.h"
#include "Settings.h"
#include "Stepper.h"
#include "System.h"
#include "Report.h"

#include "Print.h"
#include "usart_fifo.h"
#include "System32.h"
//#define FULL_MESSAGE

// Internal report utilities to reduce flash with repetitive tasks turned into functions.
static void Report_SettingPrefix(uint8_t n)
{
	Putc('$');
	Printf("%d", n);
	Putc('=');
}


static void Report_LineFeed(void)
{
	Putc('\r');
	Putc('\n');
}


static void report_util_feedback_line_feed(void)
{
	Putc(']');
	Report_LineFeed();
}


static void report_util_gcode_modes_G(void)
{
	Printf(" G");
}


static void report_util_gcode_modes_M(void)
{
	Printf(" M");
}


static void Report_AxisValue(float *axis_value)
{
	uint8_t idx;

	for(idx = 0; idx < N_AXIS; idx++) {
		PrintFloat_CoordValue(axis_value[idx]);

		if(idx < (N_AXIS-1)) {
			Putc(',');
		}
	}
}


static void report_util_uint8_setting(uint8_t n, int val)
{
	Report_SettingPrefix(n);
	Printf("%d", val);
	Report_LineFeed(); // report_util_setting_string(n);
}


static void report_util_float_setting(uint8_t n, float val, uint8_t n_decimal)
{
	Report_SettingPrefix(n);
	PrintFloat(val, n_decimal);
	Report_LineFeed(); // report_util_setting_string(n);
}


// Handles the primary confirmation protocol response for streaming interfaces and human-feedback.
// For every incoming line, this method responds with an 'ok' for a successful command or an
// 'error:'  to indicate some error event with the line or some critical system error during
// operation. Errors events can originate from the g-code parser, settings module, or asynchronously
// from a critical error, such as a triggered hard limit. Interface should always monitor for these
// responses.
void Report_StatusMessage(uint8_t status_code)
{
    if(status_code == STATUS_OK){
        Printf("ok\r\n");
    } else {
#ifdef FULL_MESSAGE
        Printf("error:%d, ", status_code);
        switch(status_code){
            case 1:
                Printf("G-code words consist of a letter and a value. Letter was not found.\r\n");
                break;
            case 2:
                Printf("Numeric value format is not valid or missing an expected value.\r\n");
                break;
            case 3:
                Printf("Grbl '$' system command was not recognized or supported.\r\n");
                break;
            case 4:
                Printf("Negative value received for an expected positive value.\r\n");
                break;
            case 5:
                Printf("Homing cycle is not enabled via settings.\r\n");
                break;
            case 6:
                Printf("Minimum step pulse time must be greater than 3usec\r\n");
                break;
            case 7:
                Printf("EEPROM read failed. Reset and restored to default values.\r\n");
                break;
            case 8:
                Printf("Grbl '$' command cannot be used unless Grbl is IDLE. Ensures smooth operation during a job.\r\n");
                break;
            case 9:
                Printf("G-code locked out during alarm or jog state\r\n");
                break;
            case 10:
                Printf("Soft limits cannot be enabled without homing also enabled.\r\n");
                break;
            case 11:
                Printf("Max characters per line exceeded. Line was not processed and executed.\r\n");
                break;
            case 12:
                Printf("(Compile Option) Grbl '$' setting value exceeds the maximum step rate supported.\r\n");
                break;
            case 13:
                Printf("Safety door detected as opened and door state initiated.\r\n");
                break;
            case 14:
                Printf("(Grbl-Mega Only) Build info or startup line exceeded EEPROM line length limit.\r\n");
                break;
            case 15:
                Printf("Jog target exceeds machine travel. Command ignored.\r\n");
                break;
            case 16:
                Printf("Jog command with no '=' or contains prohibited g-code.\r\n");
                break;
            case 17:
                Printf("Laser mode requires PWM output.\r\n");
                break;
            case 20:
                Printf("Unsupported or invalid g-code command found in block.\r\n");
                break;
            case 21:
                Printf("More than one g-code command from same modal group found in block.\r\n");
                break;
            case 22:
                Printf("Feed rate has not yet been set or is undefined.\r\n");
                break;
            case 23:
                Printf("G-code command in block requires an integer value.\r\n");
                break;
            case 24:
                Printf("Two G-code commands that both require the use of the XYZ axis words were detected in the block.\r\n");
                break;
            case 25:
                Printf("A G-code word was repeated in the block.\r\n");
                break;
            case 26:
                Printf("A G-code command implicitly or explicitly requires XYZ axis words in the block, but none were detected.\r\n");
                break;
            case 27:
                Printf("N line number value is not within the valid range of 1 - 9,999,999.\r\n");
                break;
            case 28:
                Printf("A G-code command was sent, but is missing some required P or L value words in the line.\r\n");
                break;
            case 29:
                Printf("Grbl supports six work coordinate systems G54-G59. G59.1, G59.2, and G59.3 are not supported.\r\n");
                break;
            case 30:
                Printf("The G53 G-code command requires either a G0 seek or G1 feed motion mode to be active.\r\n"
                       "A different motion was active.\r\n");
                break;
            case 31:
                Printf("There are unused axis words in the block and G80 motion mode cancel is active.\r\n");
                break;
            case 32:
                Printf("A G2 or G3 arc was commanded but there are no XYZ axis words in the selected plane to trace the arc.\r\n");
                break;
            case 33:
                Printf("The motion command has an invalid target. G2, G3, and G38.2 generates this error, \r\n"
                       "if the arc is impossible to generate or if the probe target is the current position.\r\n");
                break;
            case 34:
                Printf("A G2 or G3 arc, traced with the radius definition, had a mathematical error when \r\n"
                       "computing the arc geometry. Try either breaking up the arc into semi-circles or quadrants,\r\n"
                       "or redefine them with the arc offset definition.\r\n");
                break;
            case 35:
                Printf("A G2 or G3 arc, traced with the offset definition, is missing the IJK offset word \r\n"
                       "in the selected plane to trace the arc.");
                break;
            case 36:
                Printf("There are unused, leftover G-code words that aren't used by any command in the block.\r\n");
                break;
            case 37:
                Printf("The G43.1 dynamic tool length offset command cannot apply an offset \r\n"
                       "to an axis other than its configured axis. The Grbl default axis is the Z-axis.\r\n");
                break;
            case 38:
                Printf("Tool number greater than max supported value.\r\n");
                break;
            default:
                break;
        }
#else
        Printf("error:%d\r\n", status_code);
#endif
    }
}


// Prints alarm messages.
void Report_AlarmMessage(uint8_t alarm_code)
{
#ifdef FULL_MESSAGE
	Printf("ALARM:%d, ", alarm_code);

	switch(alarm_code){
	    case 1:
	        Printf("Hard limit triggered. Machine position is likely lost due to sudden and immediate halt. \r\n"
                "Re-homing is highly recommended.");
	        break;
	    case 2:
	        Printf("G-code motion target exceeds machine travel. \r\n"
                "Machine position safely retained. Alarm may be unlocked.");
	        break;
	    case 3:
	        Printf("Reset while in motion. Grbl cannot guarantee position. Lost steps are likely. \r\n"
                "Re-homing is highly recommended.");
	        break;
	    case 4:
	        Printf("Probe fail. The probe is not in the expected initial state before starting probe cycle, \r\n"
                "where G38.2 and G38.3 is not triggered and G38.4 and G38.5 is triggered.");
	        break;
	    case 5:
	        Printf("Probe fail. Probe did not contact the workpiece within the programmed travel for G38.2 and G38.4.");
	        break;
	    case 6:
	        Printf("Homing fail. Reset during active homing cycle.");
	        break;
	    case 7:
	        Printf("Homing fail. Safety door was opened during active homing cycle.");
	        break;
	    case 8:
	        Printf("Homing fail. Cycle failed to clear limit switch when pulling off. \r\n"
                "Try increasing pull-off setting or check wiring.");
	        break;
	    case 9:
	        Printf("Homing fail. Could not find limit switch within search distance. \r\n"
                "Defined as 1.5 * max_travel on search and 5 * pulloff on locate phases.");
	        break;
        default:
            break;
	}
#else
    Printf("ALARM:%d", alarm_code);
#endif

	Report_LineFeed();
	Delay_ms(200); // Force delay to ensure message clears serial write buffer.
	//LL_mDelay(200);
}


// Prints feedback messages. This serves as a centralized method to provide additional
// user feedback for things that are not of the status/alarm message protocol. These are
// messages such as setup warnings, switch toggling, and how to exit alarms.
// NOTE: For interfaces, messages are always placed within brackets. And if silent mode
// is installed, the message number codes are less than zero.
void Report_FeedbackMessage(uint8_t message_code)
{
	Printf("[MSG:");

	switch(message_code)
	{
	case MESSAGE_CRITICAL_EVENT:
		Printf("Reset to continue");
		break;

	case MESSAGE_ALARM_LOCK:
		Printf("'$H'|'$X' to unlock");
		break;

	case MESSAGE_ALARM_UNLOCK:
		Printf("Caution: Unlocked");
		break;

	case MESSAGE_ENABLED:
		Printf("Enabled");
		break;

	case MESSAGE_DISABLED:
		Printf("Disabled");
		break;

	case MESSAGE_SAFETY_DOOR_AJAR:
		Printf("Check Door");
		break;

	case MESSAGE_CHECK_LIMITS:
		Printf("Check Limits");
		break;

	case MESSAGE_PROGRAM_END:
		Printf("Pgm End");
		break;

	case MESSAGE_RESTORE_DEFAULTS:
		Printf("Restoring defaults");
		break;

	case MESSAGE_SPINDLE_RESTORE:
		Printf("Restoring spindle");
		break;

	case MESSAGE_SLEEP_MODE:
		Printf("Sleeping");
		break;
	default:
	    Printf("Undefined");
	    break;
	}

	report_util_feedback_line_feed();
}


// Welcome message
void Report_InitMessage(void)
{
	//Printf("\r\nGRBL-Advanced %s ['$' for help]\r\n", GRBL_VERSION);
	Printf("\r\nGrbl 1.1f ['$' for help]\r\n");
}


// Grbl help message
void Report_GrblHelp(void) {
	Printf("[HLP:$$ $# $G $I $N $x=val $Nx=line $J=line $SLP $C $X $H ~ ! ? ctrl-x]\r\n");
}


// Grbl global settings print out.
// NOTE: The numbering scheme here must correlate to storing in settings.c
void Report_GrblSettings(void) {
	// Print Grbl settings.
	report_util_uint8_setting(0, settings.system_flags);
	report_util_uint8_setting(1, settings.stepper_idle_lock_time);
	report_util_uint8_setting(2, settings.step_invert_mask);
	report_util_uint8_setting(3, settings.dir_invert_mask);
	report_util_uint8_setting(4, BIT_IS_TRUE(settings.flags, BITFLAG_INVERT_ST_ENABLE));
	report_util_uint8_setting(5, BIT_IS_TRUE(settings.flags, BITFLAG_INVERT_LIMIT_PINS));
	report_util_uint8_setting(6, BIT_IS_TRUE(settings.flags, BITFLAG_INVERT_PROBE_PIN));
	report_util_uint8_setting(10, settings.status_report_mask);
	report_util_float_setting(11, settings.junction_deviation, N_DECIMAL_SETTINGVALUE);
	report_util_float_setting(12, settings.arc_tolerance, N_DECIMAL_SETTINGVALUE);
	report_util_uint8_setting(13, BIT_IS_TRUE(settings.flags, BITFLAG_REPORT_INCHES));
	report_util_uint8_setting(14, settings.tool_change);
	report_util_uint8_setting(20, BIT_IS_TRUE(settings.flags, BITFLAG_SOFT_LIMIT_ENABLE));
	report_util_uint8_setting(21, BIT_IS_TRUE(settings.flags, BITFLAG_HARD_LIMIT_ENABLE));
	report_util_uint8_setting(22, BIT_IS_TRUE(settings.flags, BITFLAG_HOMING_ENABLE));
	report_util_uint8_setting(23, settings.homing_dir_mask);
	report_util_float_setting(24, settings.homing_feed_rate, N_DECIMAL_SETTINGVALUE);
	report_util_float_setting(25, settings.homing_seek_rate, N_DECIMAL_SETTINGVALUE);
	report_util_uint8_setting(26, settings.homing_debounce_delay);
	report_util_float_setting(27, settings.homing_pulloff, N_DECIMAL_SETTINGVALUE);
	report_util_float_setting(30, 25000.0f, N_DECIMAL_RPMVALUE);
	report_util_float_setting(31, 0, N_DECIMAL_RPMVALUE);

	report_util_uint8_setting(32, BIT_IS_TRUE(settings.flags,BITFLAG_LASER_MODE));

	Delay_ms(5);
	//LL_mDelay(5);

	// Print axis settings
	uint8_t idx, set_idx;
	uint8_t val = AXIS_SETTINGS_START_VAL;

	for(set_idx = 0; set_idx < AXIS_N_SETTINGS; set_idx++)
    {
		for(idx = 0; idx < N_AXIS; idx++)
        {
			switch(set_idx)
            {
			case 0:
				report_util_float_setting(val+idx,settings.steps_per_mm[idx],N_DECIMAL_SETTINGVALUE);
				break;

			case 1:
				report_util_float_setting(val+idx,settings.max_rate[idx],N_DECIMAL_SETTINGVALUE);
				break;

			case 2:
				report_util_float_setting(val+idx,settings.acceleration[idx]/(60*60),N_DECIMAL_SETTINGVALUE);
				break;

			case 3:
				report_util_float_setting(val+idx,-settings.max_travel[idx],N_DECIMAL_SETTINGVALUE);
				break;

            case 4:
                report_util_float_setting(val+idx,settings.backlash[idx],N_DECIMAL_SETTINGVALUE);
                break;

			default:
				break;
			}
		}

		val += AXIS_SETTINGS_INCREMENT;
	}
}


// Prints current probe parameters. Upon a probe command, these parameters are updated upon a
// successful probe or upon a failed probe with the G38.3 without errors command (if supported).
// These values are retained until Grbl is power-cycled, whereby they will be re-zeroed.
void Report_ProbeParams(void)
{
	float print_position[N_AXIS];

	// Report in terms of machine position.
	Printf("[PRB:");
	System_ConvertArraySteps2Mpos(print_position, sys_probe_position);
	Report_AxisValue(print_position);
	Putc(':');
	Printf("%d", sys.probe_succeeded);
	report_util_feedback_line_feed();
}


void Report_TLSParams(void)
{
    float print_position[N_AXIS];
    uint8_t idx = 0;

    // Report in terms of machine position.
	Printf("[TLS:");
	System_ConvertArraySteps2Mpos(print_position, settings.tls_position);

	for(idx = 0; idx < 3; idx++) {
		PrintFloat_CoordValue(print_position[idx]);

		if(idx < (3-1)) {
			Putc(',');
		}
	}

	Putc(':');
	Printf("%d", settings.tls_valid);
	report_util_feedback_line_feed();
}


// Prints Grbl NGC parameters (coordinate offsets, probing)
void Report_NgcParams(void)
{
	float coord_data[N_AXIS];
	uint8_t coord_select;

	for(coord_select = 0; coord_select <= SETTING_INDEX_NCOORD; coord_select++) {
		if(!(Settings_ReadCoordData(coord_select,coord_data))) {
			Report_StatusMessage(STATUS_SETTING_READ_FAIL);

			return;
		}

		Printf("[G");
		switch(coord_select)
		{
		case 6:
			Printf("28");
			break;

		case 7:
			Printf("30");
			break;

		default:
			Printf("%d", coord_select+54);
			break; // G54-G59

		}

		Putc(':');
		Report_AxisValue(coord_data);
		report_util_feedback_line_feed();
	}

	Printf("[G92:");        // Print G92,G92.1 which are not persistent in memory
	Report_AxisValue(gc_state.coord_offset);
	report_util_feedback_line_feed();
	Printf("[TLO:");        // Print tool length offset value
	PrintFloat_CoordValue(gc_state.tool_length_offset);
	report_util_feedback_line_feed();
	Report_ProbeParams();   // Print probe parameters. Not persistent in memory.
	Report_TLSParams();     // Print tls position. Persistent in memory.
}


// Print current gcode parser mode state
void Report_GCodeModes(void)
{
	Printf("[GC:G");

	if(gc_state.modal.motion >= MOTION_MODE_PROBE_TOWARD) {
		Printf("38.");
		Printf("%d", gc_state.modal.motion - (MOTION_MODE_PROBE_TOWARD-2));
	}
	else {
		Printf("%d", gc_state.modal.motion);
	}

	report_util_gcode_modes_G();
	Printf("%d", gc_state.modal.coord_select+54);

	report_util_gcode_modes_G();
	Printf("%d", gc_state.modal.plane_select+17);

	report_util_gcode_modes_G();
	Printf("%d", 21-gc_state.modal.units);

	report_util_gcode_modes_G();
	Printf("%d", gc_state.modal.distance+90);

	report_util_gcode_modes_G();
	Printf("%d", 94-gc_state.modal.feed_rate);

	report_util_gcode_modes_G();
	Printf("%d", 98+gc_state.modal.retract);

	if(gc_state.modal.program_flow) {
		report_util_gcode_modes_M();

		switch(gc_state.modal.program_flow)
		{
		case PROGRAM_FLOW_PAUSED:
			Putc('0');
			break;

		// case PROGRAM_FLOW_OPTIONAL_STOP : Putc('1'); break; // M1 is ignored and not supported.
		case PROGRAM_FLOW_COMPLETED_M2:
		case PROGRAM_FLOW_COMPLETED_M30:
			Printf("%d", gc_state.modal.program_flow);
			break;

		default:
			break;
		}
	}

	report_util_gcode_modes_M();
    Putc('3');

#ifdef ENABLE_M7
	if(gc_state.modal.coolant) { // Note: Multiple coolant states may be active at the same time.
		if (gc_state.modal.coolant & PL_COND_FLAG_COOLANT_MIST) { report_util_gcode_modes_M(); Putc('7'); }
		if (gc_state.modal.coolant & PL_COND_FLAG_COOLANT_FLOOD) { report_util_gcode_modes_M(); Putc('8'); }
	}
	else {
		report_util_gcode_modes_M(); Putc('9');
	}
#else
	report_util_gcode_modes_M();
    Putc('9');
#endif

#ifdef ENABLE_PARKING_OVERRIDE_CONTROL
	if(sys.override_ctrl == OVERRIDE_PARKING_MOTION) {
		report_util_gcode_modes_M();
		Printf("%d", 56);
	}
#endif

	Printf(" T");
	Printf("%d", gc_state.tool);

	Printf(" F");
	PrintFloat_RateValue(gc_state.feed_rate);

	// Printf(" S");
	// PrintFloat(gc_state.spindle_speed, N_DECIMAL_RPMVALUE);

	report_util_feedback_line_feed();
}


// Prints specified startup line
void Report_StartupLine(uint8_t n, char *line)
{
	Printf("$N");
	Printf("%d", n);
	Putc('=');
	Printf("%s", line);
	Report_LineFeed();
}


void Report_ExecuteStartupMessage(char *line, uint8_t status_code)
{
	Putc('>');
	Printf("%s", line);
	Putc(':');
	Report_StatusMessage(status_code);
}


// Prints build info line
void Report_BuildInfo(char *line)
{
	Printf("[VER: %s %s:", GRBL_VERSION, GRBL_VERSION_BUILD);
	Printf("%s", line);
	report_util_feedback_line_feed();
	Printf("[OPT:"); // Generate compile-time build option list
    Putc('V');

//#ifdef USE_LINE_NUMBERS
	Putc('N');
//#endif
#ifdef ENABLE_M7
	Putc('M');
#endif
#ifdef COREXY
	Putc('C');
#endif
#ifdef PARKING_ENABLE
	Putc('P');
#endif
#ifdef HOMING_FORCE_SET_ORIGIN
	Putc('Z');
#endif
#ifdef HOMING_SINGLE_AXIS_COMMANDS
	Putc('H');
#endif
#ifdef LIMITS_TWO_SWITCHES_ON_AXES
	Putc('T');
#endif
#ifdef ALLOW_FEED_OVERRIDE_DURING_PROBE_CYCLES
	Putc('A');
#endif
#ifdef SPINDLE_ENABLE_OFF_WITH_ZERO_SPEED
	Putc('0');
#endif
#ifdef ENABLE_SOFTWARE_DEBOUNCE
	Putc('S');
#endif
#ifdef ENABLE_PARKING_OVERRIDE_CONTROL
	Putc('R');
#endif
#ifndef ENABLE_RESTORE_EEPROM_WIPE_ALL // NOTE: Shown when disabled.
	Putc('*');
#endif
#ifndef ENABLE_RESTORE_EEPROM_DEFAULT_SETTINGS // NOTE: Shown when disabled.
	Putc('$');
#endif
#ifndef ENABLE_RESTORE_EEPROM_CLEAR_PARAMETERS // NOTE: Shown when disabled.
	Putc('#');
#endif
#ifndef ENABLE_BUILD_INFO_WRITE_COMMAND // NOTE: Shown when disabled.
	Putc('I');
#endif
#ifndef FORCE_BUFFER_SYNC_DURING_EEPROM_WRITE // NOTE: Shown when disabled.
	Putc('E');
#endif
#ifndef FORCE_BUFFER_SYNC_DURING_WCO_CHANGE // NOTE: Shown when disabled.
	Putc('W');
#endif
#ifndef HOMING_INIT_LOCK
	Putc('L');
#endif
#ifdef ENABLE_SAFETY_DOOR_INPUT_PIN
    Putc('+');
#endif

	// NOTE: Compiled values, like override increments/max/min values, may be added at some point later.
	Putc(',');
	Printf("%d", BLOCK_BUFFER_SIZE-1);
	Putc(',');
	Printf("%d", LINE_BUFFER_SIZE);

	report_util_feedback_line_feed();
}


// Prints the character string line Grbl has received from the user, which has been pre-parsed,
// and has been sent into protocol_execute_line() routine to be executed by Grbl.
void Report_EchoLineReceived(char *line)
{
	Printf("[echo: ");
	Printf("%s", line);
	report_util_feedback_line_feed();
}


 // Prints real-time data. This function grabs a real-time snapshot of the stepper subprogram
 // and the actual location of the CNC machine. Users may change the following function to their
 // specific needs, but the desired real-time data report must be as short as possible. This is
 // requires as it minimizes the computational overhead and allows grbl to keep running smoothly,
 // especially during g-code programs with fast, short line segments and high frequency reports (5-20Hz).
void Report_RealtimeStatus(void)
{
	uint8_t idx;
	int32_t current_position[N_AXIS]; // Copy current state of the system position variable
	float print_position[N_AXIS];

	memcpy(current_position,sys_position,sizeof(sys_position));
	System_ConvertArraySteps2Mpos(print_position, current_position);

	// Report current machine state and sub-states
	//Putc('\n');
	Putc('<');

	switch(sys.state)
	{
	case STATE_IDLE:
		Printf("Idle");
		break;

	case STATE_CYCLE:
		Printf("Run");
		break;

	case STATE_HOLD:
		if(!(sys.suspend & SUSPEND_JOG_CANCEL)) {
			Printf("Hold:");

			if(sys.suspend & SUSPEND_HOLD_COMPLETE) {
				Putc('0');
			} // Ready to resume
			else {
				Putc('1');
			} // Actively holding
			break;
		} // Continues to print jog state during jog cancel.

	case STATE_JOG: Printf("Jog"); break;
	case STATE_HOMING: Printf("Home"); break;
	case STATE_ALARM: Printf("Alarm"); break;
	case STATE_CHECK_MODE: Printf("Check"); break;
	case STATE_SAFETY_DOOR:
		Printf("Door:");
		if (sys.suspend & SUSPEND_INITIATE_RESTORE) {
			Putc('3'); // Restoring
		}
		else {
			if(sys.suspend & SUSPEND_RETRACT_COMPLETE) {
				if(sys.suspend & SUSPEND_SAFETY_DOOR_AJAR) {
					Putc('1'); // Door ajar
				}
				else {
					Putc('0');
				} // Door closed and ready to resume
			}
			else {
				Putc('2'); // Retracting
			}
		}
		break;

	case STATE_SLEEP:
		Printf("Sleep");
		break;

    case STATE_FEED_DWELL:
        Printf("Dwell");
        break;

    case STATE_TOOL_CHANGE:
        Printf("Tool");
        break;

	default:
		break;
	}

	float wco[N_AXIS];
	if(BIT_IS_FALSE(settings.status_report_mask,BITFLAG_RT_STATUS_POSITION_TYPE) || (sys.report_wco_counter == 0) ) {
		for (idx = 0; idx <  N_AXIS; idx++) {
			// Apply work coordinate offsets and tool length offset to current position.
			wco[idx] = gc_state.coord_system[idx]+gc_state.coord_offset[idx];

			if(idx == TOOL_LENGTH_OFFSET_AXIS) {
				wco[idx] += gc_state.tool_length_offset;
			}

			if(BIT_IS_FALSE(settings.status_report_mask, BITFLAG_RT_STATUS_POSITION_TYPE)) {
				print_position[idx] -= wco[idx];
			}
		}
	}

	// Report machine position
	if(BIT_IS_TRUE(settings.status_report_mask, BITFLAG_RT_STATUS_POSITION_TYPE)) {
		Printf("|MPos:");
	} else {
		Printf("|WPos:");
	}

	Report_AxisValue(print_position);

	// Returns planner and serial read buffer states.
#ifdef REPORT_FIELD_BUFFER_STATE
	if(BIT_IS_TRUE(settings.status_report_mask, BITFLAG_RT_STATUS_BUFFER_STATE)) {
		Printf("|Bf:");
		Printf("%d", Planner_GetBlockBufferAvailable());
		Putc(',');
		Printf("%d", available_in_rx_fifo());
	}
#endif

#ifdef REPORT_FIELD_LINE_NUMBERS
	// Report current line number
	Planner_Block_t * cur_block = Planner_GetCurrentBlock();
	if(cur_block != NULL) {
		uint32_t ln = cur_block->line_number;

		if(ln > 0) {
			Printf("|Ln:");
			Printf("%d", ln);
		}
	}
#endif

	// Report realtime feed speed
#ifdef REPORT_FIELD_CURRENT_FEED_SPEED
	Printf("|FS:");
	PrintFloat_RateValue(Stepper_GetRealtimeRate());
	Putc(',');
	PrintFloat(8000, N_DECIMAL_RPMVALUE);
#endif

#ifdef REPORT_FIELD_PIN_STATE
	uint8_t lim_pin_state = Limits_GetState();
	uint8_t ctrl_pin_state = System_GetControlState();
	uint8_t prb_pin_state = Probe_GetState();

	if(lim_pin_state | ctrl_pin_state | prb_pin_state) {
		Printf("|Pn:");
		if(prb_pin_state) {
			Putc('P');
		}

		if(lim_pin_state) {
			if (BIT_IS_TRUE(lim_pin_state, BIT(X_AXIS))) { Putc('X'); }
			if (BIT_IS_TRUE(lim_pin_state, BIT(Y_AXIS))) { Putc('Y'); }
			if (BIT_IS_TRUE(lim_pin_state, BIT(Z_AXIS))) { Putc('Z'); }
		}

		if(ctrl_pin_state) {
			if (BIT_IS_TRUE(ctrl_pin_state, CONTROL_PIN_INDEX_SAFETY_DOOR)) { Putc('D'); }
			if (BIT_IS_TRUE(ctrl_pin_state, CONTROL_PIN_INDEX_RESET)) { Putc('R'); }
			if (BIT_IS_TRUE(ctrl_pin_state, CONTROL_PIN_INDEX_FEED_HOLD)) { Putc('H'); }
			if (BIT_IS_TRUE(ctrl_pin_state, CONTROL_PIN_INDEX_CYCLE_START)) { Putc('S'); }
		}
	}
#endif

#ifdef REPORT_FIELD_WORK_COORD_OFFSET
	if(sys.report_wco_counter > 0) {
		sys.report_wco_counter--;
	}
	else {
		if(sys.state & (STATE_HOMING | STATE_CYCLE | STATE_HOLD | STATE_JOG | STATE_SAFETY_DOOR)) {
			sys.report_wco_counter = (REPORT_WCO_REFRESH_BUSY_COUNT-1); // Reset counter for slow refresh
		}
		else {
			sys.report_wco_counter = (REPORT_WCO_REFRESH_IDLE_COUNT-1);
		}

		if(sys.report_ovr_counter == 0) {
			sys.report_ovr_counter = 1;
		} // Set override on next report.

		Printf("|WCO:");
		Report_AxisValue(wco);
	}
#endif

#ifdef REPORT_FIELD_OVERRIDES
	if(sys.report_ovr_counter > 0) {
		sys.report_ovr_counter--;
	}
	else {
		if(sys.state & (STATE_HOMING | STATE_CYCLE | STATE_HOLD | STATE_JOG | STATE_SAFETY_DOOR)) {
			sys.report_ovr_counter = (REPORT_OVR_REFRESH_BUSY_COUNT-1); // Reset counter for slow refresh
		}
		else {
			sys.report_ovr_counter = (REPORT_OVR_REFRESH_IDLE_COUNT-1);
		}

		Printf("|Ov:");
		Printf("%d", sys.f_override);
		Putc(',');
		Printf("%d", sys.r_override);
		Putc(',');
		Printf("100");
		//Printf("%d", sys.spindle_speed_ovr);
	}
#endif

	Putc('>');
	Report_LineFeed();
}
