#pragma once
#include "xrCore/xrstring.h"
#include "xrCommon/xr_string.h"
#include "xrCommon/xr_map.h"
#include "xrEngine/xr_input.h" // Don't remove this include

enum EGameActions
{
    kLEFT,
    kRIGHT,
    kUP,
    kDOWN,
    kJUMP,
    kCROUCH,
    kACCEL,
    kSPRINT_TOGGLE,

    kFWD,
    kBACK,
    kL_STRAFE,
    kR_STRAFE,

    kL_LOOKOUT,
    kR_LOOKOUT,

    kCAM_1,
    kCAM_2,
    kCAM_3,
    kCAM_ZOOM_IN,
    kCAM_ZOOM_OUT,
	kFLASH,	//Romann
    kTORCH,
    kNIGHT_VISION,
    kDETECTOR,

    kTURN_ENGINE,
    kSWITCH_HORN,
    kCAR_TRUNK,
    kCAR_TRANSMISSION_UP,
    kCAR_TRANSMISSION_DOWN,

    kWPN_1,
    kWPN_2,
    kWPN_3,
    kWPN_4,
    kWPN_5,
    kWPN_6,
    kARTEFACT,
    kWPN_NEXT,
    kWPN_FIRE,
    kWPN_ZOOM,
    kWPN_ZOOM_INC,
    kWPN_ZOOM_DEC,
    kWPN_RELOAD,
    kWPN_FUNC,
    kWPN_FIREMODE_PREV,
    kWPN_FIREMODE_NEXT,
    kWPN_ADDON_LASER,
    kWPN_ADDON_FLASHLIGHT,
    kWPN_AMMO_CHECK,

    kPAUSE,
    kDROP,
    kUSE,
    kSCORES,
    kCHAT,
    kCHAT_TEAM,
    kSCREENSHOT,
    kQUIT,
    kCONSOLE,
    kINVENTORY,
    kBUY,
    kSKIN,
    kTEAM,
    kACTIVE_JOBS,

    kVOTE_BEGIN,
    kSHOW_ADMIN_MENU,
    kVOTE,
    kVOTEYES,
    kVOTENO,

    kNEXT_SLOT,
    kPREV_SLOT,

    kSPEECH_MENU_0,
    kSPEECH_MENU_1,

    kQUICK_USE_1,
    kQUICK_USE_2,
    kQUICK_USE_3,
    kQUICK_USE_4,

    kQUICK_SAVE,
    kQUICK_LOAD,
//    kALIFE_CMD,

    kCUSTOM1,
    kCUSTOM2,
    kCUSTOM3,
    kCUSTOM4,
    kCUSTOM5,
    kCUSTOM6,
    kCUSTOM7,
    kCUSTOM8,
    kCUSTOM9,
    kCUSTOM10,
    kCUSTOM11,
    kCUSTOM12,
    kCUSTOM13,
    kCUSTOM14,
    kCUSTOM15,

    kCAM_AUTOAIM,

    kPDA_TAB1,
    kPDA_TAB2,
    kPDA_TAB3,
    kPDA_TAB4,
    kPDA_TAB5,
    kPDA_TAB6,

#ifdef COC_KICK
    kKICK, // alpet: kick dynamic objects
#endif

    kLASTACTION,
    kNOTBINDED,
    kFORCEDWORD = u32(-1)
};

struct ENGINE_API _keyboard
{
    pcstr key_name;
    int dik;
    xr_string key_local_name;
};
enum _key_group
{
    _both = (1 << 0),
    _sp = _both | (1 << 1),
    _mp = _both | (1 << 2),
};

extern ENGINE_API _key_group g_current_keygroup;

struct ENGINE_API _action
{
    pcstr action_name;
    EGameActions id;
    _key_group key_group;
};

extern ENGINE_API _action actions[];
extern ENGINE_API _keyboard keyboards[];

#define bindings_count kLASTACTION
struct ENGINE_API _binding
{
    _action* m_action;
    _keyboard* m_keyboard[2];
};

extern ENGINE_API _binding g_key_bindings[];

ENGINE_API EGameActions action_name_to_id(pcstr _name);
ENGINE_API _action* action_name_to_ptr(pcstr _name);

ENGINE_API bool is_binded(EGameActions action_id, int dik);
ENGINE_API int get_action_dik(EGameActions action_id, int idx = -1);

ENGINE_API int keyname_to_dik(pcstr _name);
ENGINE_API _keyboard* keyname_to_ptr(pcstr _name);

ENGINE_API void GetActionAllBinding(LPCSTR action, char* dst_buff, int dst_buff_sz);

// 0xED - max vavue in DIK* enum
#define MOUSE_1 (0xED + 100)
#define MOUSE_2 (0xED + 101)
#define MOUSE_3 (0xED + 102)

#define MOUSE_4 (0xED + 103)
#define MOUSE_5 (0xED + 104)
#define MOUSE_6 (0xED + 105)
#define MOUSE_7 (0xED + 106)
#define MOUSE_8 (0xED + 107)
