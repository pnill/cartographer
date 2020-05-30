#pragma once





enum device_index {
	mouse = 0,
	keyboard = 1,
	controller = 2
};

struct controller_input_state
{
	BYTE trigger_pressure[2];
	BYTE trigger_threshhold[2];
	BYTE button_time_held[16];
	WORD wbutton_time_held[16];
	short sThumbLX;
	short sThumbLY;
	short sThumbRX;
	short sThumbRY;
};


class device_bind
{
public:
	device_index index; //0x0000 
	__int32 key; //0x0004 
	__int32 unknown; //0x0008 

};//Size=0x000C

class keybind_map
{
public:
	__int32 device_count; //0x0000 
	device_bind device[8]; //0x0004 

};//Size=0x0064

struct keybind_base
{
public:
	float N01DA8A10; //0x0000 
	float N01DA8A11; //0x0004 
	float N01DA8A12; //0x0008 
	float N01DA8A13; //0x000C 
	BYTE N01DA8A14; //0x0010 
	BYTE N01DA8A15;
	BYTE N01DA8A16;
	BYTE N01DA8A17;
	keybind_map action_bind[57]; //0x0014 
	DWORD N01E30340; //0x1658 
	float N01E30341; //0x165C 
	float N01E30342; //0x1660 
	float N01E30343; //0x1664 
	float N01E30344; //0x1668 
	float N01E30345; //0x166C 
	BYTE N01E30346; //0x1670 
	BYTE N01E30347;
	BYTE N01E30348;
	BYTE N01E30349;
	float N01E3034A; //0x1674 
	WORD N01E3034E; //0x1678 
	WORD N01E45C19; //0x167A 
	WORD N01E30350; //0x167C 
	WORD N01E4664D; //0x167E 
};//Size=0x1680

struct input_states
{
public:
	float Pressure; //0x0000 
	BYTE hold_time1; //0x0004 
	BYTE unk; //0x0005 
	WORD hold_time2; //0x0006 

};//Size=0x0008


struct keybind_parent
{
public:
	keybind_base base[4]; //0x0000 
	input_states input_state[76]; //0x5A00 
	float mouse_look_LR; //0x5C60 
	float mouse_look_UD; //0x5C64 
	float joystick_look_LR; //0x5C68 
	float joystick_look_UD; //0x5C6C 
	float mouse_look_UD2; //0x5C70 
	float joystick_lookUD2; //0x5C74 
	WORD N01E728EF; //0x5C78 
	WORD N0208421C; //0x5C7A 
	WORD N01E728F0; //0x5C7C 
	WORD N01E7416E; //0x5C7E 
	char _0x5C80[176];
	WORD N01E77DD8; //0x5D30 
	WORD N02073219; //0x5D32 
	char _0x5D34[180];
	WORD N01E7DA83; //0x5DE8 
	WORD N02071603; //0x5DEA 
	char _0x5DEC[180];
	WORD N01E82C10; //0x5EA0 
	WORD N02077775; //0x5EA2 
	char _0x5EA4[4];
	WORD N01E82C12; //0x5EA8 
	WORD N020792D0; //0x5EAA 
	BYTE N01E82C13; //0x5EAC 
	BYTE N0207ACA4; //0x5EAD 
	BYTE N0207B543; //0x5EAE 
	BYTE N0207ACA5; //0x5EAF 
	BYTE N01E82C14; //0x5EB0 
	BYTE N0207D5A0; //0x5EB1 
	BYTE N0207E581; //0x5EB2 
	BYTE N0207D5A1; //0x5EB3 
	char _0x5EB4[8];

};//Size=0x5EBC

struct inputBuffer
{
	BYTE bhold_time[45];
	WORD whold_time[45];
	float LMB_orRTrigger; //0x0088 
	float RMB_or_LTrigger; //0x008C 
	float ForwardVelocity; //0x0090 
	float DirectionalVelocity; //0x0094 
	float MouseLookLeftRight; //0x0098 
	float MouseLookupDown1; //0x009C 
	float ControllerLookLeftRight; //0x00A0 
	float Controller_LookUpDown1; //0x00A4 
	float MouseLookupDown2; //0x00A8 
	float Controller_LookUpDown2; //0x00AC 
	WORD NFF74DD7C; //0x00B0 
	BYTE toggles_between_mk_cnt; //0x00B2 
	BYTE NFFB8E6F5; //0x00B3 
	DWORD NFF74DD7B; //0x00B4 
	DWORD NFF74DD7A; //0x00B8 
	DWORD NFF74DD70; // 0x00BC
};

struct input_process_array
{
	float bButtonA; //0x0000 
	BYTE holdButtonA; //0x0004 
	BYTE NFFE3C403; //0x0005 
	WORD __holdButtonA; //0x0006 
	float _bButtonA; //0x0008 
	BYTE _holdButtonA; //0x000C 
	BYTE NFFE3C404; //0x000D 
	WORD ___holdButtonA; //0x000E 
	float __bButtonA; //0x0010 
	BYTE ____holdButtonA; //0x0014 
	BYTE NFFE610EE; //0x0015 
	WORD _____holdButtonA; //0x0016 
	float bButtonRB; //0x0018 
	BYTE holdButtonRB; //0x001C 
	BYTE NFFE78BA1; //0x001D 
	WORD _holdButtonRB; //0x001E 
	float bButtonY; //0x0020 
	BYTE holdButtonY; //0x0024 
	BYTE NFFE7DF75; //0x0025 
	WORD _holdButtonY; //0x0026 
	float bButtonB; //0x0028 
	BYTE holdButtonB; //0x002C 
	BYTE NFFEC1638; //0x002D 
	WORD _holdButtonB; //0x002E 
	float _bButtonRB; //0x0030 
	BYTE __holdButtonRB; //0x0034 
	BYTE NFFEE57A4; //0x0035 
	WORD ___holdButtonRB; //0x0036 
	float bRMB_orLT; //0x0038 
	BYTE holdRMB_orLT; //0x003C 
	BYTE NFFEF1B1A; //0x003D 
	WORD _holdRMB_orLT; //0x003E 
	float __bRMB_orLT; //0x0040 
	BYTE __holdRMB_orLT; //0x0044 
	BYTE NFFEE5668; //0x0045 
	WORD ___holdRMB_orLT; //0x0046 
	float ___bRMB_orLT; //0x0048 
	BYTE ____holdRMB_orLT; //0x004C 
	BYTE NFFEF8530; //0x004D 
	WORD _____holdRMB_orLT; //0x004E 
	float bLMB_orRT; //0x0050 
	BYTE _holdLMB_orRT; //0x0054 
	BYTE NFFEFA8FB; //0x0055 
	WORD __holdLMB_orRT; //0x0056 
	float bStartButton; //0x0058 
	BYTE holdStartButton; //0x005C 
	BYTE NFFEFCD43; //0x005D 
	WORD _holdStartButton; //0x005E 
	float bBackButton; //0x0060 
	BYTE holdBackButton; //0x0064 
	BYTE NFFEFF0A1; //0x0065 
	WORD _holdBackButton; //0x0066 
	float bCrouchButton; //0x0068 
	BYTE holdCrouchButton; //0x006C 
	BYTE NFFF00EEE; //0x006D 
	WORD _holdCrouchButton; //0x006E 
	float bZoomButton; //0x0070 
	BYTE holdZoomButton; //0x0074 
	BYTE NFFF0AA9C; //0x0075 
	WORD _holdZoomButton; //0x0076 
	float bButtonUnknown; //0x0078 
	BYTE holdButtonUnknown; //0x007C 
	BYTE NFFF0CD3E; //0x007D 
	WORD _holdButtonUnknown; //0x007E 
	float bButtonUnknown2; //0x0080 
	BYTE holdButtonUnknown2; //0x0084 
	BYTE NFFF0F232; //0x0085 
	WORD _holdButtonUnknown2; //0x0086 
	float bDPADL; //0x0088 
	BYTE holdDPADL; //0x008C 
	BYTE NFFF11BE9; //0x008D 
	WORD _holdDPADL; //0x008E 
	float bDPADR; //0x0090 
	BYTE holdDPADR; //0x0094 
	BYTE NFFF13C2A; //0x0095 
	WORD _holdDPADR; //0x0096 
	float bButtonAccept; //0x0098 
	BYTE holdButtonAccept; //0x009C 
	BYTE NFFF15D17; //0x009D 
	WORD _holdButtonAccept; //0x009E 
	float bButtonB_; //0x00A0 
	BYTE holdButtonB_; //0x00A4 
	BYTE NFFF18AD5; //0x00A5 
	WORD _holdButtonB_; //0x00A6 
	float _bButtonB_; //0x00A8 
	BYTE __holdButtonB_; //0x00AC 
	BYTE NFFF1AEFE; //0x00AD 
	WORD ___holdButtonB__; //0x00AE 
	float bRT_Pressure; //0x00B0 
	BYTE _holdRT_; //0x00B4 
	BYTE NFFF1E25B; //0x00B5 
	WORD __holdRT__; //0x00B6 
	float bLT_Pressure; //0x00B8 
	BYTE _bHoldRT_; //0x00BC 
	BYTE NFFF20403; //0x00BD 
	WORD __bHoldRT__; //0x00BE 
	float bRT_Pressure_; //0x00C0 
	BYTE bHoldRT__; //0x00C4 
	BYTE NFFF26E4D; //0x00C5 
	WORD _bHoldRT__; //0x00C6 
	float _bLT_Pressure_; //0x00C8 
	BYTE __holdLT__; //0x00CC 
	BYTE NFFF294F7; //0x00CD 
	WORD ___holdLT___; //0x00CE 
	float bF1_Button; //0x00D0 
	BYTE __F1Hold; //0x00D4 
	BYTE NFFF2D5A4; //0x00D5 
	WORD __F1Hold__; //0x00D6 
	float _bButtonUnknown; //0x00D8 
	BYTE holdButtonUknown; //0x00DC 
	BYTE NFFF32E70; //0x00DD 
	WORD _holdButtonUknown; //0x00DE 
	float bButtonUknown2; //0x00E0 
	BYTE holdButtonUknown2; //0x00E4 
	BYTE NFFF35846; //0x00E5 
	WORD holdButtonUknown2_; //0x00E6 
	float bButtonUknown3; //0x00E8 
	BYTE holdButtonUknown3; //0x00EC 
	BYTE NFFF381BE; //0x00ED 
	WORD holdButtonUknown3_; //0x00EE 
	float bVoiceChat; //0x00F0 
	BYTE holdVoiceChat; //0x00F4 
	BYTE NFFF3AA5C; //0x00F5 
	WORD holdVoiceChat_; //0x00F6 
	float bActionButton; //0x00F8 
	BYTE holdActionButton; //0x00FC 
	BYTE NFFF3D8A7; //0x00FD 
	WORD holdActionButton_; //0x00FE 
	float bActionButton2; //0x0100 
	BYTE holdActionButton2; //0x0104 
	BYTE NFFF69742; //0x0105 
	WORD _holdActionButton2; //0x0106 
	float bActionButton3; //0x0108 
	BYTE holdActionButton3; //0x010C 
	BYTE NFFF6CF61; //0x010D 
	WORD _holdActionButton3; //0x010E 
	float bActionButton4; //0x0110 
	BYTE holdActionButton4; //0x0114 
	BYTE NFFF70879; //0x0115 
	WORD _holdActionButton4; //0x0116 
	float bActionButton5; //0x0118 
	BYTE holdActionbutton5; //0x011C 
	BYTE NFFF722AF; //0x011D 
	WORD _holdActionbutton5; //0x011E 
	float bActionbutton6; //0x0120 
	BYTE holdActionButton6; //0x0124 
	BYTE NFFF75233; //0x0125 
	WORD _holdActionButton6; //0x0126 
	float bActionbutton7; //0x0128 
	BYTE holdActionButton7; //0x012C 
	BYTE NFFF78976; //0x012D 
	WORD _holdActionButton7; //0x012E 
	float bActionButton8; //0x0130 
	BYTE holdActionbutton8; //0x0134 
	BYTE NFFF7DF00; //0x0135 
	WORD _holdActionButton8; //0x0136 
	float bYButton1; //0x0138 
	BYTE holdYButton1; //0x013C 
	BYTE NFFF7FE56; //0x013D 
	WORD _holdYButton1; //0x013E 
	float bYButton2; //0x0140 
	BYTE holdYButton2; //0x0144 
	BYTE NFFF95A63; //0x0145 
	WORD _holdYButton2; //0x0146 
	float bYButton3; //0x0148 
	BYTE holdYButton3; //0x014C 
	BYTE NFFF97C4A; //0x014D 
	WORD _holdYButton3; //0x014E 
	float bYButton4; //0x0150 
	BYTE holdYButton4; //0x0154 
	BYTE NFFFB258C; //0x0155 
	WORD _holdYButton4; //0x0156 
	float bActionButton9; //0x0158 
	BYTE holdActionButton9; //0x015C 
	BYTE NFFFB7048; //0x015D 
	WORD _holdActionButton9; //0x015E 
	float bActionButton10; //0x0160 
	BYTE holdAction10; //0x0164 
	BYTE NFFFB9443; //0x0165 
	WORD _holdActionButton10; //0x0166 
	float bWalkF; //0x0168 
	BYTE holdWalkF; //0x016C 
	BYTE NFFFC6DC1; //0x016D 
	WORD _holdWalkF; //0x016E 
	float bWalkB; //0x0170 
	BYTE holdWalkB; //0x0174 
	BYTE NFFFD456C; //0x0175 
	WORD _holdWalkB; //0x0176 
	float bWalkL; //0x0178 
	BYTE holdWalkL; //0x017C 
	BYTE NFFFE5C51; //0x017D 
	WORD _holdWalkL; //0x017E 
	float bWalkR; //0x0180 
	BYTE holdWalkR; //0x0184 
	BYTE NFFFE7EE4; //0x0185 
	WORD _holdWalkR; //0x0186 
	float bMouseLookL; //0x0188 
	BYTE holdMouseLookL; //0x018C 
	BYTE NFFFEA56E; //0x018D 
	WORD _holdMouseLookL; //0x018E 
	float bMouseLookR; //0x0190 
	BYTE holdMouseLookR; //0x0194 
	BYTE N0000039E; //0x0195 
	WORD _holdMouseLookR; //0x0196 
	float bMouseLookUP; //0x0198 
	BYTE holdMouseLookUP; //0x019C 
	BYTE N00004949; //0x019D 
	WORD _holdMouseLookUP; //0x019E 
	float bMouseLookD; //0x01A0 
	BYTE holdMouseLookD; //0x01A4 
	BYTE N00008F54; //0x01A5 
	WORD _holdMouseLookD; //0x01A6 
	float bRJ_LookL; //0x01A8 
	BYTE holdRJ_LookL; //0x01AC 
	BYTE N000428BC; //0x01AD 
	WORD _holdRJ_LookL; //0x01AE 
	float bRJ_LookR; //0x01B0 
	BYTE holdRJ_LookR; //0x01B4 
	BYTE N00044EAA; //0x01B5 
	WORD _holdRJ_LookR; //0x01B6 
	float bRJ_LookUP; //0x01B8 
	BYTE holdRJ_LookUP; //0x01BC 
	BYTE N000492FD; //0x01BD 
	WORD _holdRJ_LookUP; //0x01BE 
	float bRJ_LookD; //0x01C0 
	BYTE holdRJ_LookD; //0x01C4 
	BYTE N0004F067; //0x01C5 
	WORD _holdRJ_LookD; //0x01C6 

};//Size=0x01C8



enum button_types
{
	action_button1 = 0,
	action_button2 = 1,
	action_button3 = 2,
	rbumper_button = 3,
	ybutton_1 = 4,
	bbutton_1 = 5,
	rbumper_button2 = 6,
	rmb_button1 = 7,
	rmb_button2 = 8,
	rmb_button3 = 9,
	lmb_button = 10,
	start_button = 11,
	back_button = 12,
	crouch_button = 13,
	zoom_button = 14,
	unknown_button = 15,
	unknown_button2 = 16,
	dpadl_button = 17,
	dpadr_button = 18,
	accept_button = 19,
	bbutton_2 = 20,
	bbutton_3 = 21,
	rt_button = 22,
	lt_button = 23,
	rt_button2 = 24,
	lt_button2 = 25,
	f1_button = 26,
	unknown_button3 = 27,
	unknown_button4 = 28,
	unknown_button5 = 29,
	voice_chat = 30,
	action_button4 = 31,
	action_button5 = 32,
	action_button6 = 33,
	action_button7 = 34,
	action_button8 = 35,
	action_button9 = 36,
	action_button10 = 37,
	action_button11 = 38,
	ybutton_2 = 39,
	ybutton_3 = 40,
	ybutton_4 = 41,
	ybutton_5 = 42,
	action_button12 = 43,
	action_button13 = 44,
	button_walkf = 45,
	button_walkb = 46,
	button_walkl = 47,
	button_walkr = 48,
	button_mouselook_l = 49,
	button_mouselook_r = 50,
	button_mouselook_u = 51,
	button_mouselook_d = 52,
	joystick_look_l = 53,
	joystick_look_r = 54,
	joystick_look_u = 55,
	joystick_look_d = 56
};

struct blam_mouse_input
{
	LONG mouse_lX;
	LONG mouse_lY;
	LONG mouse_lZ;
	BYTE byte_button_status[8];
	WORD word_button_status[8];
};


struct button_held_state {
	DWORD isPressed;
	float time_pressed;
	DWORD isPressed2;
};

/*struct keybind_map
{
	__int32 KB_C_OR_M; //0x0000 
	__int32 C_ONLY; //0x0004 
	__int32 controller_button; //0x0008 
	float controller_related; //0x000C 
	__int32 keyboard_keystate; //0x0010 
	__int32 keyboard_button; //0x0014 
	char _0x0018[76];
};//Size=0x0064

*/

/*struct __declspec(align(4)) button_mapping
{
	int KB_C_OR_M;
	int C_ONLY;
	int controller_button;
	float ImpactsController;
	int keyboard_keystate;
	int keyboard_button;
};*/

struct control_float_double {
	float param1;
	float param2;
};

/*struct input_states
{
	float Pressure;
	BYTE unk;
	BYTE hold_time1;
	BYTE hold_time2;
};*/
void InitializeInputFixes();
