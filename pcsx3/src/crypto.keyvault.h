#pragma once
#include "pcsx3.pch.h"
//#include "emu.log.h"

//http://www.psdevwiki.com/ps3/SELF_Types
enum SelfKeyType {
	KEY_LV0 = 1, //LV0 / level 0 
	KEY_LV1 = 2, //LV1 / level 1 
	KEY_LV2 = 3, //LV2 / level 2 
	KEY_APP = 4, //APP / Application 
	KEY_ISO = 5, //ISO / Isolated SPU Module 
	KEY_LDR = 6, //LDR / Secure Loader 
	KEY_UNK7 = 7, //UNK7 / Format looks like a regular Application
	KEY_NPDRM = 8, //NPDRM / NP-DRM Application 
	KEY_PKG = 99, //custom pcsx3's keytype
};

struct SelfKey {
	u32 self_type;
	u64 version;
	u16 key_revision;
	u8 erk[0x20];
	u8 riv[0x10];
};

const SelfKey* getSelfKey(u32 type, u64 version, u16 revision);