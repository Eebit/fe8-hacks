#include "global.h"

#include "prepscreen.h"
#include "bmguide.h"
#include "bm.h"
#include "fontgrp.h"
#include "bmlib.h"
#include "m4a.h"
#include "soundwrapper.h"

#include "../../Text/TextDefinitions.h"

void Guide_Init(ProcPtr);
void Guide_SetBlend(ProcPtr);
void Guide_MainLoop(ProcPtr);
void Guide_OnEnd(ProcPtr);

const struct ProcCmd ProcScr_PrepScreenGuide_MapMenu[] = {
    PROC_NAME("E_Guide"),

    PROC_CALL(LockGame),

    PROC_YIELD,

    PROC_CALL(Guide_Init),
    PROC_CALL(StartGreenText),

    PROC_CALL(StartFastFadeFromBlack),
    PROC_REPEAT(WaitForFade),

    PROC_CALL(Guide_SetBlend),
    PROC_REPEAT(Guide_MainLoop),

    PROC_CALL(StartFastFadeToBlack),
    PROC_REPEAT(WaitForFade),

    PROC_CALL(EndGreenText),

    PROC_CALL(Guide_OnEnd),
    PROC_YIELD,

    PROC_CALL(UnlockGame),

    PROC_END,
};

void Guide_Init_Colors(ProcPtr proc) {
    ResetText();
    LoadObjUIGfx();
    return;
}

const struct ProcCmd ProcScr_PrepScreenGuide[] = {
    PROC_NAME("E_Guide"),
    PROC_YIELD,

    PROC_CALL(Guide_Init_Colors),
    PROC_CALL(Guide_Init),
    PROC_CALL(StartGreenText),

    PROC_CALL(StartFastFadeFromBlack),
    PROC_REPEAT(WaitForFade),

    PROC_CALL(Guide_SetBlend),
    PROC_REPEAT(Guide_MainLoop),

    PROC_CALL(StartFastFadeToBlack),
    PROC_REPEAT(WaitForFade),

    PROC_CALL(EndGreenText),

    PROC_CALL(Guide_OnEnd),
    PROC_YIELD,

    PROC_END,
};


// ==================================
// MAIN PREP MENU
// ==================================

// Autohook this function, we don't need it (vanilla function for calling chapter status screen from prep)
void sub_808E79C(ProcPtr proc) {
    Proc_StartBlocking(ProcScr_PrepScreenGuide, proc);
    return;
}

void PrepScreenMenu_OnGuide(struct ProcAtMenu * proc) {
    proc->state = 5;
    Proc_Goto(proc, 10); // Prep "Start Submenu" label

    return;
}

// hook at 0x955A0 (InitPrepScreenMainMenu)
void PrepScreenGuide_AddExtraEntry(struct ProcAtMenu * proc) {
    int color;

    // Preserve what we've replaced

    color = TEXT_COLOR_SYSTEM_WHITE;
    if (!CanPrepScreenSave()) {
        color = TEXT_COLOR_SYSTEM_GRAY;
    }

    SetPrepScreenMenuItem(PREP_MAINMENU_SAVE, PrepScreenMenu_OnSave, color, 0x579, 0);

    color = TEXT_COLOR_SYSTEM_WHITE;
    if (IsGuideLocked()) {
        color = TEXT_COLOR_SYSTEM_GRAY;
    }

    // Use index 5 because it's easiest; there is already a function in the jump table
    SetPrepScreenMenuItem(5, PrepScreenMenu_OnGuide, color, Guide_PrepMenu_Text, 0);

    return;
}

// ==================================
// PREP MAP MENU
// ==================================

void PrepScreenGuide_MapMenu_OnGuide(struct ProcPrepSallyCursor * proc) {
    proc->unk_58 = 4;
    Proc_Goto(proc, 55); // Take over the Merlinus intro cutscene in the Sallycursor proc
    return;
}

// hook at 0x33696 (PrepScreenProc_StartMapMenu)
void PrepScreenGuide_MapMenu_AddExtraEntry(struct ProcPrepSallyCursor * proc) {
    // Preserve what we've replaced
    SetPrepScreenMenuItem(8, PrepMapMenu_OnOptions, 0, 0x592, 0x5BD);

    // 0x06E5 = vanilla Guide help text ID
    if (!IsGuideLocked()) {
        SetPrepScreenMenuItem(4, PrepScreenGuide_MapMenu_OnGuide, TEXT_COLOR_SYSTEM_WHITE, Guide_PrepMenu_Text, 0x06E5);
    } else {
        SetPrepScreenMenuItem(4, PrepScreenGuide_MapMenu_OnGuide, TEXT_COLOR_SYSTEM_GRAY, Guide_PrepMenu_Text, 0x06E5);
    }

    return;
}

void PrepScreenGuide_StartGuide(ProcPtr proc) {
    Proc_StartBlocking(ProcScr_PrepScreenGuide_MapMenu, proc);
    return;
}
