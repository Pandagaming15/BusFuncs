#include "library.h"
#include "mod/amlmod.h"
#include "mod/config.h"
#include "mod/logger.h"
#include "GTASA/common.h"
#include "shared/CEvents.h"
#include "shared/ScriptCommands.h"
#include "GTASA/CTimer.h"
#include "GTASA/CPools.h"
#include "GTASA/CCamera.h"
#include "funcs/Panda.h"
//#include "funcs/Shadow.h"
#include "GlossHook/include/Gloss.h"
#include "shared/Screen.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <map>

#include "iimgui.h"
IImGui* imgui = NULL;

#define HOOKBW(_name, _fnAddr)                                    \
    CPatch::TrampolinesRedirectCall(SET_THUMB, _fnAddr, (void*)(&HookOf_##_name), (void**)(&_name), BW_THUMB32);
    
MYMODCFG(BusDoors, GTASA, 1.0, Pandagaming)
NEEDGAME(com.rockstargames.gtasa)

using namespace plugin;
using namespace std;

#include "GTASA/CClumpModelInfo.h"

RwFrame *door_1_r, *door_1_l, *door_2_r, *door_2_l, *stop_1, *stop_2;

void RotateFrame(RwFrame* frame, float angle) {
    if (frame) {
        // Call the RwFrameRotate function with the correct combine type
        RwV3d axis = {0.0f,0.0f,1.0f};
        RwFrameRotate(frame, &axis, angle, rwCOMBINEPRECONCAT);
    }
}

static bool open = false;
static bool closed = false;
static int handle2 = 0;
static int handle = 0;
static bool isClosed = false;
static bool isOpen = false;

void BusDoors(CVehicle *vehicle)
{
	if(vehicle->m_pRwClump)
	{
			door_1_r = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "door_1_r");
			door_1_l = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "door_1_l");
			door_2_r = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "door_2_r");
			door_2_l = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "door_2_l");
			stop_1 = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "stop_1");
			stop_2 = CClumpModelInfo::GetFrameFromName(vehicle->m_pRwClump, "stop_2");
		
		if(door_1_r)
		{
			if(Command<Commands::IS_WIDGET_SWIPED_LEFT>(WIDGET_HORN) && !isOpen)
            {
                open = true;
                isOpen = true;
                isClosed = false;
            }
			
			if(handle < 80 && open)
            {
                if(door_1_r) RotateFrame(door_1_r, 1.0f);
				if(door_1_l) RotateFrame(door_1_l, -1.0f);
				if(door_2_r) RotateFrame(door_2_r, 1.0f);
				if(door_2_l) RotateFrame(door_2_l, -1.0f);
				if(stop_1) RotateFrame(stop_1, -1.0f);
				if(stop_2) RotateFrame(stop_2, -1.0f);
                handle += 1;
            }
			
			if(handle >= 80)
            {
                open = false;
                handle = 0;
            }
            if(Command<Commands::IS_WIDGET_SWIPED_RIGHT>(WIDGET_HORN) && !isClosed)
            {
                closed = true;
                isClosed = true;
                isOpen = false;
            }
            if(handle2 < 80 && closed)
            {
                if(door_1_r) RotateFrame(door_1_r, -1.0f);
				if(door_1_l) RotateFrame(door_1_l, 1.0f);
				if(door_2_r) RotateFrame(door_2_r, -1.0f);
				if(door_2_l) RotateFrame(door_2_l, 1.0f);
				if(stop_1) RotateFrame(stop_1, 1.0f);
				if(stop_2) RotateFrame(stop_2, 1.0f);
                handle2 += 1;
            }
			
            if(handle2 >= 80)
            {
                closed = false;
                handle2 = 0;
            }
		}
	}
}

processScriptsEvent = []
{
	CPed *player = FindPlayerPed();
	if(player)
	{
		CVehicle *vehicle = player->m_pMyVehicle;
		if(vehicle)
		{
			BusDoors(vehicle);
		}
	}
};

extern "C" void OnModLoad()
{
	SetEventBefore(processScripts);
}
