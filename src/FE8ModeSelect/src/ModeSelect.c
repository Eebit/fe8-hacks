#include "global.h"

#include "proc.h"
#include "hardware.h"
#include "bm.h"
#include "m4a.h"
#include "soundwrapper.h"
#include "fontgrp.h"
#include "face.h"
#include "uiutils.h"
#include "ctc.h"
#include "savemenu.h"
#include "ekrbattle.h"
#include "bmlib.h"
#include "mu.h"
#include "bmsave.h"

#include "constants/faces.h"

/*
@ division runtime
fun __divsi3, __aeabi_idiv
fun __modsi3, __aeabi_idivmod
fun __udivsi3, __aeabi_uidiv
fun __umodsi3, __aeabi_uidivmod

dat 0x02000000, gUnk_02000000
dat 0x02000001, gUnk_02000001
dat 0x020000A4, gUnk_020000A4
dat 0x0201E8D4, gUnk_0201E8D4
dat 0x0201E97C, gUnk_0201E97C
dat 0x0201E9F4, gUnk_0201E9F4
*/

void sub_806E8F0(void);

#define ApplySystemObjectsGraphics LoadObjUIGfx
#define SetBgOffset BG_SetPosition
#define Proc_LockEachMarked Proc_BlockEachMarked
#define ApplyPaletteExt CopyToPaletteBuffer
#define SpawnProc Proc_Start
#define SetFaceConfig SetupFaceGfxData
#define TmApplyTsa_thm CallARM_FillTileRect
#define GetBgChrOffset GetBackgroundTileDataOffset
#define fe7u_func_08063FE0 sub_806E8F0
#define fe7u_func_080A8CE8 sub_80ACCF4 // LoadUiSpinningArrowGfx
#define fe7u_func_080A8D70 sub_80ACD7C // SetUiSpinningArrowConfig
#define fe7u_func_080A8D54 sub_80ACD60 // SetUiSpinningArrowPositions
#define fe7u_func_08054E10 sub_805A940
#define GetMsg GetStringFromIndex
#define EnableBgSync BG_EnableSyncByMask
#define fe7u_func_080A9DE8 sub_80ADDFC // affine something or other
#define fe7u_func_080A8D98 sub_80ACDA4 // SetUiSpinningArrowsFast
#define fe7u_func_08054C8C sub_805A7B4
#define fe7u_func_08054EF0 sub_805AA28
#define fe7u_func_080A8CD4 sub_80ACCE0 // StartUiSpinningArrows
#define fe7u_func_080A86A0 sub_80AC6AC
#define fe7u_func_08054EC8 NewEkrUnitMainMini
#define fe7u_func_08007BCC StartFace2
#define fe7u_func_0800751C StartFaceFadeIn
#define fe7u_func_08005AD4 PutDrawText
#define fe7u_func_08001F3C sub_800154C
#define SetOnHBlankA SetPrimaryHBlankHandler
#define InitBgs SetupBackgrounds
#define EndAllMus MU_EndAll
#define EnablePalSync EnablePaletteSync
#define LoadMetaSave ReadGlobalSaveInfo
#define MetaSave_CountCompletedPlaythroughs GetGlobalCompletionCntByInfo
#define fe7u_func_08054E88 NewEfxAnimeDrvProc
#define fe7u_func_08003388 WriteOAMRotScaleData

#define gBg0Tm gBG0TilemapBuffer
#define gBg1Tm gBG1TilemapBuffer
#define gBg3Tm gBG3TilemapBuffer
#define gPal gPaletteBuffer

struct Unk_020000A4
{
    struct Font unk_00;
    struct Text unk_18[7];
};
extern struct Unk_020000A4 gUnk_020000A4;

struct ModeSelectProc
{
    /* 00 */ PROC_HEADER;
    /* 2C */ s32 unk_2c;
    /* 30 */ u16 unk_30;
    /* 32 */ u16 unk_32;
    /* 34 */ s32 unk_34;
    /* 38 */ void * unk_38;
    /* 3C */ void * unk_3c; // pFaceProc
    /* 40 */ u8 unk_40;
    /* 41 */ u8 unk_41;
    /* 42 */ u8 unk_42;
    /* 43 */ u8 unk_43[3];
    /* 46 */ STRUCT_PAD(0x46, 0x49);
    /* 49 */ u8 unk_49[3];
    /* 4C */ u8 unk_4c;
    /* 50 */ s32 unk_50;
};

struct OpInfoData
{
    /* 00 */ u8 unk_00;
    /* 01 */ u8 unk_01;           // generic pal ID
    /* 02 */ u16 unk_02;          // xPos
    /* 04 */ u16 unk_04;          // yPos
    /* 06 */ u16 unk_06;          // animId
    /* 08 */ u16 unk_08;          // charPalId
    /* 0A */ u16 unk_0A;          // AISMode
    /* 0C */ u16 unk_0C;          // state2
    /* 0E */ u16 unk_0E;          // tileOffset
    /* 10 */ u16 unk_10;          // palOffset
    /* 14 */ void * unk_14;       // AIS1
    /* 18 */ void * unk_18;       // AIS2
    /* 1C */ const void * unk_1C; // Huichelaar says this is "sheet"
    /* 20 */ const void * unk_20; // pal
    /* 24 */ const void * unk_24; // rtlOam
    /* 28 */ const void * unk_28; // frameData
    /* 2C */ s32 unk_2C;          // sheetPointer
    /* 30 */ void * unk_30;       // magicEffects
    /* 34 */ void * unk_34;       // ProcPtr; Procs_ekrUnitMainMini
};

extern struct OpInfoData gUnk_0201E8D4[];

struct Unk0201E97C
{
    u16 unk_00;
    u16 unk_02;
    u16 unk_04;
    u16 unk_06;
    u16 unk_08;
    u16 unk_0A;
    u16 unk_0C;
    u16 unk_0E;
    u16 unk_10;
    u16 unk_12;
    int unk_14;
    int unk_18;
    void * unk_1C;
    void * unk_20;
    void * unk_24;
};

extern struct Unk0201E97C gUnk_0201E97C[];

extern u8 gUnk_02000000;
extern u8 gUnk_02000001;

// ==============
// TODO:
// ==============

void fe7u_func_080AACD8(u16 * param_1, u8 * param_2, u16 param_3)
{
    Decompress(param_2, gGenericBuffer);
    TmApplyTsa_thm(param_1, gGenericBuffer, param_3);
    return;
}

u32 fe7u_func_0809EFBC(void)
{
    return 1;
};

int fe7u_func_0809E9FC(void)
{
    int cVar1;
    int iVar2;
    int uVar3;
    struct GlobalSaveInfo auStack_70;

    uVar3 = 0;
/*
    if (LoadMetaSave(&auStack_70))
    {
        iVar2 = MetaSave_CountCompletedPlaythroughs(&auStack_70);

        // FIXME
        cVar1 = fe7u_func_0809EFBC();
        uVar3 = (int)(-cVar1 | cVar1) >> 0x1f & 2;

        if ((auStack_70.completed) != 0)
        {
            if ((auStack_70.Eirk_mode_easy || auStack_70.Eirk_mode_norm) != 0)
            {
                uVar3 = 0xf;
            }

            if ((auStack_70.Eirk_mode_hard) != 0)
            {
                uVar3 = uVar3 | 0x10;
            }

            if (iVar2 > 4)
            {
                uVar3 = uVar3 | 0x10;
            }
        }
    }
*/
    uVar3 = 0x1f;
    return uVar3;
}

// clang-format off

int const gUnk_08CE48C0[4][3] = {
    {
        // 0x000004DE, // Lyn
        // 0x000012B3, // Swordfighter of Sacae[.]
        // 0x000012B8, // Swd[.]
        0x212, // "Eirika"
        0x78B, // "The Valiant"
        0x4FE, // "Str"
    },
    {
        // 0x000004DC, // Eliwood
        // 0x000012B4, // Nobleman of Pherae
        // 0x000012B8, // Swd[.]
        0x220, // "Ephraim"
        0x78B, // "The Valiant"
        0x4FE, // "Str"
    },
    {
        //0x000004DD, // Hector
        //0x000012B5, // Marquess Ostia's brother
        //0x000012B9, // Axe[.]
        0x234, // "Lyon"
        0x78B, // "The Valiant"
        0x4FF, // "Mag"
    },
    {
        // 0x000012B6, // Main character:[.]
        // 0x00000000, 
        // 0x000012B7, // Weapon:[.]
        0x5A1, // "Pick"
        0,
        0x4F2, // "Equip"
    }
};

// clang-format on

void fe7u_func_080A9E7C(u8 param_1, s16 param_2, s16 param_3)
{
    struct BG2Stuff * puVar1 = NULL;

    if (param_1 == 2) {
        puVar1 = &gLCDControlBuffer.bg2stuff;
    }

    puVar1->bg2pb = (puVar1->bg2pb * param_2) >> 8;
    puVar1->bg2pd = (puVar1->bg2pd * param_2) >> 8;

    puVar1->bg2pa = (puVar1->bg2pa * param_3) >> 8;
    puVar1->bg2pc = (param_3 * puVar1->bg2pc) >> 8;

    return;
}

void fe7u_func_080A9ECC(u8 a, s16 b, s16 c, s16 d, s16 e)
{
    struct BG2Stuff * puVar1 = NULL;

    if (a == 2) {
        puVar1 = &gLCDControlBuffer.bg2stuff;
    }

    puVar1->bg2x = puVar1->bg2pa * -b + puVar1->bg2pb * -c + ((d << 0x10) >> 8);
    puVar1->bg2y = puVar1->bg2pc * -b + puVar1->bg2pd * -c + ((e << 0x10) >> 8);

    return;
}

void fe7u_func_08054E5C(struct OpInfoData * param_1)
{
    ((struct Anim *)(param_1->unk_14))->state3 |= 8;
    ((struct Anim *)(param_1->unk_18))->state3 |= 8;
    return;
}

// palette
void fe7u_func_080A73F8(s32 param_1)
{
}

// clang-format off

u16 const BgConfig_08CE3C0C[] = {
    0x0000, 0x6000, 0x0000, 
    0xC000, 0x6800, 0x0000, 
    0x8000, 0x7800, 0x0000, 
    0x8000, 0x7800, 0x0000,
};

// clang-format on

extern u16 Pal_084138F0[]; // pal
extern u16 Pal_0840F9A0[]; // pal

extern u8 Img_08418E44[]; // gfx
extern u8 Img_0840FEB4[]; // gfx

extern u8 Tsa_0840FA00[]; // tsa?
extern u8 Tsa_08411F34[]; // tsa?

void fe7u_func_080A32D4(void)
{
    u16 vcount = REG_VCOUNT + 1;

    if (vcount > 0xa0) {
        vcount = 0;
    }

    if ((vcount & 1) != 0) {
        return;
    }

    if (vcount < gUnk_02000000) {
        REG_BLDCNT = 0xc1;
        REG_BLDY = (gUnk_02000000 != 0) 
            ? (gUnk_02000000 - vcount) * 0x10 / gUnk_02000000 
            : 0;

    } else {
        REG_BLDCNT = 0x144;
        REG_BLDALPHA = gUnk_02000001 | 0x1000;
    }

    return;
}


void fe7u_func_080A4E58(void)
{
    InitBgs((u16 *)BgConfig_08CE3C0C);

    gLCDControlBuffer.dispcnt.mode = 1;

    gLCDControlBuffer.bg2cnt.screenSize = 1;
    gLCDControlBuffer.bg2cnt.areaOverflowMode = 0;

    gLCDControlBuffer.bg0cnt.priority = 3;
    gLCDControlBuffer.bg1cnt.priority = 0;
    gLCDControlBuffer.bg2cnt.priority = 2;
    gLCDControlBuffer.bg3cnt.priority = 2;

    EndAllMus();

    SetDispEnable(0, 0, 0, 0, 0);

    gUnk_02000001 = 10;
    gUnk_02000000 = 100;

    SetOnHBlankA(fe7u_func_080A32D4);

    ApplyPaletteExt(Pal_084138F0, 0x220, 0x100);
    ApplyPaletteExt(Pal_0840F9A0, 0, 0x60);

    Decompress(Img_08418E44, (void *)(GetBgChrOffset(0) + 0x6000000));
    TmApplyTsa_thm(gBg0Tm, Tsa_0840FA00, 0);

    Decompress(Img_0840FEB4, (void *)(GetBgChrOffset(3) + 0x6000000));
    fe7u_func_08001F3C(gBg3Tm, Tsa_08411F34, 0, 5);

    EnableBgSync(8);

    return;
}

// =========
// end todo
// =========

struct ModeSelectSpriteDrawProc
{
    /* 00 */ PROC_HEADER;
    /* 2C */ s32 unk_2c;
    /* 30 */ s32 unk_30;
    /* 34 */ s32 unk_34;

    /* 38 */ s32 unk_38;
    /* 3C */ u8 unk_3c;
    /* 3E */ u16 unk_3e;
    /* 40 */ s32 unk_40;
    /* 44 */ s32 unk_44;
    /* 48 */ s32 unk_48;
    /* 4C */ u8 unk_4c;
    /* 4D */ u8 unk_4d;
    /* 4E */ u8 unk_4e;
};

const u8 * const gUnk_08CE480C[] = {
    (u8 *)0x020000F4,
    (u8 *)0x020020F4,
    (u8 *)0x020040F4,
};

const u8 * const gUnk_08CE4818[] = {
    (u8 *)0x020060F4,
    (u8 *)0x0200B8F4,
    (u8 *)0x020110F4,
};

const u8 * const gUnk_08CE4824[] = {
    (u8 *)0x020168F4,
    (u8 *)0x02016994,
    (u8 *)0x02016A34,
};

const u8 * const gUnk_08CE4830[] = {
    (u8 *)0x02016AD4,
    (u8 *)0x020194D4,
    (u8 *)0x0201BED4,
};

void fe7u_func_080A7480(int param_1, u8 * param_2)
{
    // anims?
    int gUnk_08418DA8[] = {
        2,
        0,
        0x9c,
    };

    int i;

    for (i = 0; i < param_1; i++)
    {
        gUnk_0201E8D4[i].unk_02 = 0x140;
        gUnk_0201E8D4[i].unk_04 = 0x58;
        gUnk_0201E8D4[i].unk_06 = gUnk_08418DA8[param_2[i]];
        gUnk_0201E8D4[i].unk_0A = 6;
        gUnk_0201E8D4[i].unk_01 = 0;
        gUnk_0201E8D4[i].unk_0C = 1;
        gUnk_0201E8D4[i].unk_0E = (i * 0x2000 + 0x2000) >> 5;
        gUnk_0201E8D4[i].unk_10 = i + 0xd;

        gUnk_0201E8D4[i].unk_1C = gUnk_08CE480C[i];
        gUnk_0201E8D4[i].unk_24 = gUnk_08CE4818[i];
        gUnk_0201E8D4[i].unk_20 = gUnk_08CE4824[i];
        gUnk_0201E8D4[i].unk_28 = gUnk_08CE4830[i];

        gUnk_0201E8D4[i].unk_08 = 0xffff;

        gUnk_0201E8D4[i].unk_30 = &gUnk_0201E97C[i];

        gUnk_0201E97C[i].unk_00 = 0;
        gUnk_0201E97C[i].unk_02 = 0;
        gUnk_0201E97C[i].unk_04 = 0;
        gUnk_0201E97C[i].unk_06 = 0;
        gUnk_0201E97C[i].unk_08 = 0;
        gUnk_0201E97C[i].unk_0E = 0;
        gUnk_0201E97C[i].unk_10 = 0;
        gUnk_0201E97C[i].unk_0A = 0;
        gUnk_0201E97C[i].unk_0C = 0;
        gUnk_0201E97C[i].unk_12 = 0;

        gUnk_0201E97C[i].unk_14 = 0;
        gUnk_0201E97C[i].unk_18 = 0;
        gUnk_0201E97C[i].unk_1C = 0;
        gUnk_0201E97C[i].unk_20 = 0;
        gUnk_0201E97C[i].unk_24 = 0;

        fe7u_func_08054EC8(&gUnk_0201E8D4[i]);
    }

    return;
}

void fe7u_func_080A75CC(s32 param_1)
{
    int i;

    for (i = 0; i < param_1; i++)
    {
        fe7u_func_08054EF0(&gUnk_0201E8D4[i]);
    }

    return;
}

const char StrModeSelect_MainCharacter[] = "Main character:";
const char StrModeSelect_Weapon[] = "Weapon:";

void fe7u_func_080A75F0(void)
{
    ClearText(&gUnk_020000A4.unk_18[5]);
    ClearText(&gUnk_020000A4.unk_18[6]);

    fe7u_func_08005AD4(&gUnk_020000A4.unk_18[5], gBg1Tm + 0xCE, 0, 0, 0, StrModeSelect_MainCharacter);  // PutDrawText
    fe7u_func_08005AD4(&gUnk_020000A4.unk_18[6], gBg1Tm + 0x14E, 0, 0, 0, StrModeSelect_Weapon); // PutDrawText

    EnableBgSync(2);

    return;
}

const char StrModeSelect_Swd[] = "Swd";
const char StrModeSelect_Lnc[] = "Lnc";
const char StrModeSelect_Mag[] = "Mag";

const char * const StrModeSelect_Wpn[] = {
    StrModeSelect_Swd,
    StrModeSelect_Lnc,
    StrModeSelect_Mag,
};

void fe7u_func_080A7668(s32 index)
{
    ClearText(&gUnk_020000A4.unk_18[2]);
    ClearText(&gUnk_020000A4.unk_18[3]);
    ClearText(&gUnk_020000A4.unk_18[4]);

    fe7u_func_08005AD4(&gUnk_020000A4.unk_18[2], gBg1Tm + 0x10E, 2, 0, 0, GetMsg(gUnk_08CE48C0[index][0]));
    fe7u_func_08005AD4(&gUnk_020000A4.unk_18[4], gBg1Tm + 0x153, 2, 0, 0, StrModeSelect_Wpn[index]);

    EnableBgSync(2);

    return;
}

void fe7u_func_080A76F8(struct ModeSelectProc * proc)
{
    int cVar1 = proc->unk_43[proc->unk_41];

    ClearText(&gUnk_020000A4.unk_18[0]);
    ClearText(&gUnk_020000A4.unk_18[1]);

    //fe7u_func_08005AD4(&gUnk_020000A4.unk_18[0], gBg1Tm + 0x18f, cVar1 == 0 ? 3 : 1, 0, 0, GetMsg(0x12ba));
    fe7u_func_08005AD4(&gUnk_020000A4.unk_18[0], gBg1Tm + 0x18f, cVar1 == 0 ? 3 : 1, 0, 0, GetMsg(0x053E));

    EnableBgSync(2);

    switch (proc->unk_49[proc->unk_41])
    {
        case 0:
            if (!(1 & proc->unk_40))
            {
                return;
            }

            break;

        case 1:
            if (!(4 & proc->unk_40))
            {
                return;
            }

            break;

        case 2:
            if (!(0x10 & proc->unk_40))
            {
                return;
            }

            break;
    }

    //fe7u_func_08005AD4(&gUnk_020000A4.unk_18[1], gBg1Tm + 0x1CF, cVar1 == 1 ? 3 : 1, 0, 0, GetMsg(0x12bb));
    fe7u_func_08005AD4(&gUnk_020000A4.unk_18[1], gBg1Tm + 0x1CF, cVar1 == 1 ? 3 : 1, 0, 0, GetMsg(0x053F));

    return;
}

void * fe7u_func_080A77C0(int index)
{
    int gUnk_08418DB4[3] = { 
        FID_EIRIKA, 
        FID_EPHRAIM, 
        0x46 // FID_LYON
    };

    struct FaceProc * pFaceProc = fe7u_func_08007BCC(0, gUnk_08418DB4[index], 0xcc, 0x48, 0x42); // StartFace2
    fe7u_func_0800751C(pFaceProc);                                                    // StartFaceFadeIn

    return pFaceProc;
}

extern u8 Img_08415BE8[]; // img, lyn chapter top left
extern u8 Img_08415CB0[]; // img, lyn chapter bottom left
extern u8 Img_08415DC4[]; // img, lyn chapter top right
extern u8 Img_08415E04[]; // img, lyn chapter bottom right

extern u8 Img_08415E54[]; // img, eliwood chapter top left
extern u8 Img_08415F14[]; // img, eliwood chapter bottom left
extern u8 Img_08415FF0[]; // img, eliwood chapter top right
extern u8 Img_0841601C[]; // img, eliwood chapter bottom right

extern u8 Img_08416058[]; // img, hector chapter top left
extern u8 Img_08416118[]; // img, hector chapter bottom left
extern u8 Img_084161F4[]; // img, hector chapter top right
extern u8 Img_08416220[]; // img, hector chapter bottom right

void fe7u_func_080A77F8(s32 param_1)
{
    void * gUnk_08418DC0[3][4] = {
        {
            Img_08415BE8,
            Img_08415CB0,
            Img_08415DC4,
            Img_08415E04,
        },
        {
            Img_08415E54,
            Img_08415F14,
            Img_08415FF0,
            Img_0841601C,
        },
        {
            Img_08416058,
            Img_08416118,
            Img_084161F4,
            Img_08416220,
        },
    };

    Decompress(gUnk_08418DC0[param_1][0], (void *)0x60102C0);
    Decompress(gUnk_08418DC0[param_1][1], (void *)0x60106C0);
    Decompress(gUnk_08418DC0[param_1][2], (void *)0x6010AC0);
    Decompress(gUnk_08418DC0[param_1][3], (void *)0x6010EC0);

    return;
}

extern u16 gUnk_0201E9F4[];

void fe7u_func_080A7860(s32 palId)
{
    int i;

    u16 * palIt = gPal + (palId + 0xd) * 0x10 + 0x101;

    for (i = 0; i < 0xf; i++)
    {
        gUnk_0201E9F4[i + palId * 0xf] = *palIt++;
    }

    return;
}

void fe7u_func_080A7890(int a, int b) {

}

/*
NAKEDFUNC
void fe7u_func_080A7890(int a, int b)
{
    asm("\n\
        .syntax unified\n\
        push {r4, r5, r6, r7, lr}\n\
        adds r2, r0, #0\n\
        adds r4, r1, #0\n\
        adds r0, #0xd\n\
        lsls r0, r0, #5\n\
        ldr r1, .L080A78D8 @ =0x02022A62\n\
        adds r5, r0, r1\n\
        cmp r4, #0x40\n\
        ble .L080A78A4\n\
        movs r4, #0x40\n\
    .L080A78A4:\n\
        ldr r0, .L080A78DC @ =0x02000001\n\
        ldrb r0, [r0]\n\
        subs r0, #0xa\n\
        lsls r0, r0, #1\n\
        adds r4, r4, r0\n\
        lsls r0, r2, #4\n\
        ldr r1, .L080A78E0 @ =0x0201E9F4\n\
        subs r0, r0, r2\n\
        movs r2, #0x1f\n\
        mov ip, r2\n\
        lsls r0, r0, #1\n\
        adds r3, r0, r1\n\
        movs r6, #0xe\n\
    .L080A78BE:\n\
        mov r0, ip\n\
        ldrh r7, [r3]\n\
        ands r0, r7\n\
        muls r0, r4, r0\n\
        asrs r0, r0, #6\n\
        cmp r0, #0x1f\n\
        bgt .L080A78E4\n\
        cmp r0, #0\n\
        bge .L080A78D2\n\
        movs r0, #0\n\
    .L080A78D2:\n\
        mov r1, ip\n\
        ands r1, r0\n\
        b .L080A78E6\n\
        .align 2, 0\n\
    .L080A78D8: .4byte 0x02022A62\n\
    .L080A78DC: .4byte 0x02000001\n\
    .L080A78E0: .4byte 0x0201E9F4\n\
    .L080A78E4:\n\
        movs r1, #0x1f\n\
    .L080A78E6:\n\
        movs r2, #0xf8\n\
        lsls r2, r2, #2\n\
        adds r0, r2, #0\n\
        ldrh r7, [r3]\n\
        ands r0, r7\n\
        muls r0, r4, r0\n\
        asrs r0, r0, #6\n\
        cmp r0, r2\n\
        bgt .L080A7904\n\
        cmp r0, #0\n\
        bge .L080A78FE\n\
        movs r0, #0\n\
    .L080A78FE:\n\
        ands r0, r2\n\
        adds r1, r1, r0\n\
        b .L080A7906\n\
    .L080A7904:\n\
        adds r1, r1, r2\n\
    .L080A7906:\n\
        movs r2, #0xf8\n\
        lsls r2, r2, #7\n\
        adds r0, r2, #0\n\
        ldrh r7, [r3]\n\
        ands r0, r7\n\
        muls r0, r4, r0\n\
        asrs r0, r0, #6\n\
        cmp r0, r2\n\
        bgt .L080A7924\n\
        cmp r0, #0\n\
        bge .L080A791E\n\
        movs r0, #0\n\
    .L080A791E:\n\
        ands r0, r2\n\
        adds r0, r1, r0\n\
        b .L080A7926\n\
    .L080A7924:\n\
        adds r0, r1, r2\n\
    .L080A7926:\n\
        strh r0, [r5]\n\
        adds r5, #2\n\
        adds r3, #2\n\
        subs r6, #1\n\
        cmp r6, #0\n\
        bge .L080A78BE\n\
        bl EnablePaletteSync\n\
        pop {r4, r5, r6, r7}\n\
        pop {r0}\n\
        bx r0\n\
        .syntax divided\n\
    ");
}
*/

void fe7u_func_080A793C(s32 param_1, s32 param_2)
{
    s32 a = (param_2 & 0xff);
    s32 tmp = (((a >= 0x81) ? a - 0x80 : 0x80 - a) * 0x30 >> 7);
    fe7u_func_080A7890(param_1, tmp + 0x10);

    return;
}

void fe7u_func_080A796C(struct ModeSelectSpriteDrawProc * param_1)
{
    param_1->unk_30 = 0;
    param_1->unk_3e = 0;
    param_1->unk_3c = 0;
    param_1->unk_34 = 0x78;
    param_1->unk_38 = 0xa0;
    param_1->unk_40 = 0;
    param_1->unk_44 = 0;
    param_1->unk_3c = 0;
    param_1->unk_48 = 0;
    param_1->unk_4c = 0;
    param_1->unk_2c = 0;
    param_1->unk_4e = 0;

    return;
}

// sprites

// clang-format off

u16 const gUnk_08CE483C[] = {
    0x0004, 
    0x4000, 0x8000, 0x0000, 
    0x4000, 0x8020, 0x0004, 
    0x4010, 0x4000, 0x0040, 
    0x4010, 0x4020, 0x0044,
};

u16 const gUnk_08CE4856[] = {
    0x0006, 
    0x4000, 0x8000, 0x0008, 
    0x4000, 0x8020, 0x000C, 
    0x8000, 0x0040, 0x0010,
    0x4010, 0x4000, 0x0060, 
    0x4010, 0x4020, 0x0064, 
    0x0010, 0x0040, 0x0068,
};

u16 const gUnk_08CE487C[] = {
    0x0005, 
    0x4000, 0x4000, 0x0011, 
    0x4008, 0x4000, 0x0049, 
    0x4000, 0x4020, 0x0031, 
    0x4008, 0x4020, 0x004D, 
    0x8000, 0x0040, 0x0055,
};

u16 const gUnk_08CE489C[] = {
    0x0001,
    0x4000, 0x4000, 0x0051,
};

u16 const gUnk_08CE48A4[] = {
    0x0004, 
    0x4100, 0x8000, 0x0016, 
    0x4100, 0x8020, 0x001A, 
    0x0100, 0x4040, 0x001E, 
    0x4100, 0x8050, 0x0056,
};

// clang-format on

void fe7u_func_08054E10(struct OpInfoData *, s16, s16);
s32 fe7u_func_080A86A0(s32, s32, s32, s32, s32);

void fe7u_func_080A79A4(struct ModeSelectSpriteDrawProc * proc)
{
    s32 i;

    if (proc->unk_3c != 0)
    {
        for (i = 0; i < proc->unk_40; i++)
        {
            s32 uVar2 = ((proc->unk_3e >> 4) + i * proc->unk_44 + 0x28);
            s32 x = (proc->unk_34 << 0xc) + (SIN(uVar2) * 0x46);
            s32 y = (((proc->unk_38 << 0xc) + (COS(uVar2) * 0x1c)) >> 0xc) - 0x10;

            fe7u_func_08054E10(&gUnk_0201E8D4[i], x >> 0xc, y);

            fe7u_func_080A793C(i, (proc->unk_3e >> 4) + i * proc->unk_44);
        }
    }

    sub_80ADDFC(2, proc->unk_3e, 0, 0, 0x160, 0x160);
    fe7u_func_080A9E7C(2, 0x280, 0x100);
    fe7u_func_080A9ECC(2, proc->unk_34, proc->unk_38, 0x4c, 0x4c);

    gUnk_02000001 = fe7u_func_080A86A0(8, 8, 0x10, 0x10, proc->unk_48);

    if (proc->unk_4c == 0)
    {
        proc->unk_48 += 8;
        if (proc->unk_48 >= 0x400)
        {
            proc->unk_4c = 1;
        }
    }
    else
    {
        proc->unk_48 -= 8;

        if (proc->unk_48 <= 0)
        {
            proc->unk_4c = 0;
        }
    }

    if (proc->unk_4e & 2)
    {
        DisplayFrozenUiHandExt(0x6c, (proc->unk_4d & 1) * 0x10 + 0x68, 0xbc0);
    }
    else
    {
        DisplayUiHandExt(0x6c, proc->unk_4d * 0x10 + 0x68, 0xbc0);
    }

    PutSpriteExt(0xd, 0, 8, gUnk_08CE483C, 0xb000);
    PutSpriteExt(0xd, 0x14, 0x1c, gUnk_08CE4856, 0xb000);
    PutSpriteExt(0xd, 0x28, 0x40, gUnk_08CE489C, 0xb000);

    if ((proc->unk_2c >> 2 & 1) == 0)
    {
        PutSpriteExt(0xd, 8, 0x82, gUnk_08CE487C, 0xb000);
    }

    if (proc->unk_2c != 0)
    {
        proc->unk_2c++;
    }

    PutSpriteExt(0xd, 0x6c, 0x18, gUnk_08CE48A4, 0xa000);

    fe7u_func_080A73F8(proc->unk_30);
    proc->unk_30++;

    return;
}

struct ProcCmd const ProcScr_08CE48F0[] = {
    PROC_CALL(fe7u_func_080A796C),
    PROC_YIELD,
    PROC_REPEAT(fe7u_func_080A79A4),

    PROC_END,
};

void fe7u_func_080A7B7C(void)
{
    struct ModeSelectSpriteDrawProc * proc = Proc_Find(ProcScr_08CE48F0);

    if (proc != 0)
    {
        proc->unk_2c = 1;
    }

    return;
}

void fe7u_func_080A7B98(void)
{
    struct ModeSelectSpriteDrawProc * proc = Proc_Find(ProcScr_08CE48F0);

    if (proc != 0)
    {
        proc->unk_3c = 1;
    }

    return;
}

void fe7u_func_080A7BB4(s32 param_1)
{
    struct ModeSelectSpriteDrawProc * proc = Proc_Find(ProcScr_08CE48F0);

    if (proc != 0)
    {
        proc->unk_40 = param_1;
        proc->unk_44 = 0x100 / param_1;
    }

    return;
}

void fe7u_func_080A7BDC(s32 param_1, s32 param_2)
{
    struct ModeSelectSpriteDrawProc * proc = Proc_Find(ProcScr_08CE48F0);
    if (proc != 0)
    {
        proc->unk_34 = param_1;
        proc->unk_38 = param_2;
    }

    gUnk_02000000 = param_2 - 0x3c;

    return;
}

void fe7u_func_080A7C08(u16 param_1)
{
    struct ModeSelectSpriteDrawProc * proc = Proc_Find(ProcScr_08CE48F0);
    if (proc != 0)
    {
        proc->unk_3e = param_1;
    }

    return;
}

void fe7u_func_080A7C24(u8 param_1, u8 param_2)
{
    struct ModeSelectSpriteDrawProc * proc = Proc_Find(ProcScr_08CE48F0);
    if (proc != 0)
    {
        proc->unk_4d = param_1;
        proc->unk_4e = param_2;
    }

    return;
}

s32 fe7u_func_080A7C4C(void)
{
    struct ModeSelectSpriteDrawProc * proc = Proc_Find(ProcScr_08CE48F0);
    return proc->unk_44;
}

struct UnkProc
{
    PROC_HEADER;
    STRUCT_PAD(0x29, 0x34);
    s16 unk_34;
    s16 unk_36;
};

void fe7u_func_080A7C60(struct UnkProc * proc, s32 param_2, s32 param_3)
{
    if (proc != 0)
    {
        proc->unk_34 = param_2;
        proc->unk_36 = param_3;
    }

    return;
}

void fe7u_func_080A4E58(void);

void fe7u_func_080A7C6C(struct ModeSelectProc * proc)
{
    if (proc->unk_42 & 1)
    {
        fe7u_func_080A4E58();
    }

    return;
}

extern struct ProcCmd const ProcScr_08CE48F0[];

struct FaceVramEntry const gUnk_08CE4910[] = {
    {
        0x00001000,
        0x0000000C,
    },
    {
        0x00001000,
        0x0000000C,
    },
    {
        0x00001000,
        0x0000000C,
    },
    {
        0x00001000,
        0x0000000C,
    },
};

extern u16 Pal_084150C0[]; // pal
extern u8 Img_08414940[];  // gfx
extern u8 Tsa_084150E0[];  // tsa

extern u16 Pal_08415AA0[]; // pal
extern u8 Img_08415594[];  // gfx
extern u8 Tsa_08415AC0[];  // tsa

extern u16 Pal_0841625C[]; // pal

int const gUnk_08418DF0[] = {
    4,
    0x10,
};

void fe7u_func_080A7C84(struct ModeSelectProc * proc) {
    int i;

    ApplySystemObjectsGraphics();
    
    SetBgOffset(1,8,0xfff8);
    
    Proc_LockEachMarked(PROC_MARK_C);
    Proc_LockEachMarked(PROC_MARK_D);
    
    gUnk_02000000 = 100;
    
    SetFaceConfig(gUnk_08CE4910);
    
    ApplyPaletteExt(Pal_08415AA0, 0x1e0, 0x20);
    
    Decompress(Img_08415594, (void *)(0x6000000 + GetBgChrOffset(1)));
    TmApplyTsa_thm(gBg0Tm, Tsa_084150E0, 0);
    fe7u_func_080AACD8(gBg1Tm, Tsa_08415AC0, 0xf000); // this loads the "claw menu" and bg of the chapters
    ApplyPaletteExt(Pal_084150C0, 0x360, 0x20);
    
    Decompress(Img_08414940, (void *)0x6010000);
    ApplyPaletteExt(Pal_0841625C, 0x340, 0x20);
    
    fe7u_func_08054E88();
    fe7u_func_08063FE0();
    
    proc->unk_38 = SpawnProc(ProcScr_08CE48F0, proc);
    fe7u_func_080A7BDC(0, 0x70);

    proc->unk_41 = 0;
    proc->unk_4c = 0;

    proc->unk_40 = fe7u_func_0809E9FC();

    if ((proc->unk_42 & 1) == 0) {
        int gUnk_08418DF0[] = {
            4, 0x10, 
        };
        
        proc->unk_4c = 2;
        proc->unk_49[0] = 1;
        proc->unk_49[1] = 2;

        for (i = 0; i < proc->unk_4c; i++) {
            int bVar1 = (gPlaySt.chapterStateBits & PLAY_FLAG_HARD);
            if ((gPlaySt.chapterStateBits & PLAY_FLAG_HARD) && (proc->unk_40 & gUnk_08418DF0[i])) {
                bVar1 = 1;
            }

            proc->unk_43[i] = bVar1;
        }
    } else {
        proc->unk_49[0] = 0;
        proc->unk_4c++;

        if ((proc->unk_40 & 2) != 0) {
            proc->unk_49[proc->unk_4c] = 1;
            proc->unk_4c++;
        }

        if ((proc->unk_40 & 8) != 0) {
            proc->unk_49[proc->unk_4c] = 2;
            proc->unk_4c++;
        }
    
        for (i = 0; i < proc->unk_4c; i++) {
            proc->unk_43[i] = 0;
        }
    }

    fe7u_func_080A7BB4(proc->unk_4c);
    fe7u_func_080A7480(proc->unk_4c, proc->unk_49);

    for (i = 0; i < proc->unk_4c; i++) {
        fe7u_func_080A7860(i);
    }

    fe7u_func_080A7B98();
    fe7u_func_080A8CD4(proc);
    fe7u_func_080A8CE8(0, 0xd20, 9);
    fe7u_func_080A8CE8(0, 0xd20, 9);
    fe7u_func_080A8D70(30, 61, 68, 61);
    fe7u_func_080A8D54(3);

    InitTextFont(&gUnk_020000A4.unk_00, (void *)0x600E000, 0x100, 0xe);
    
    InitText(&gUnk_020000A4.unk_18[0], 5);
    InitText(&gUnk_020000A4.unk_18[1], 9);
    InitText(&gUnk_020000A4.unk_18[2], 5);
    InitText(&gUnk_020000A4.unk_18[3], 8);
    InitText(&gUnk_020000A4.unk_18[4], 4);
    InitText(&gUnk_020000A4.unk_18[5], 10);
    InitText(&gUnk_020000A4.unk_18[6], 5);

    proc->unk_30 = proc->unk_41 * fe7u_func_080A7C4C() * 0x10;

    proc->unk_3c = (void *)fe7u_func_080A77C0(proc->unk_49[proc->unk_41]);
    fe7u_func_080A75F0();
    fe7u_func_080A7668(proc->unk_49[proc->unk_41]);
    fe7u_func_080A76F8(proc);
    fe7u_func_080A7C24(proc->unk_43[proc->unk_41], proc->unk_42);
    fe7u_func_080A7C08(proc->unk_30);

    EnableBgSync(3);

    proc->unk_2c = 0;
    proc->unk_50 = 0;

    SetWinEnable(1, 0, 0);
    SetWin0Layers(1, 1, 1, 1, 1);
    SetWin0Box(0, 0x50, 0xf0, 0x50);
    SetWOutLayers(0, 0, 0, 0, 0);

    fe7u_func_080A77F8(proc->unk_49[proc->unk_41]);

    // clang-format off
    fe7u_func_08003388(
        0,
        Div(+COS(0) * 16, 0x100),
        Div(-SIN(0) * 16, 0x100),
        Div(+SIN(0) * 16, 0x100),
        Div(+COS(0) * 16, 0x100)
    );
    // clang-format on

    return;
}

void fe7u_func_080A8054(struct ModeSelectProc * proc)
{
    s32 tmp;
    s32 unk_2c;

    unk_2c = proc->unk_2c + 1;
    proc->unk_2c = unk_2c;

    SetDispEnable(1, 1, 1, 1, 1);

    tmp = 0x48 - (((0x10 - unk_2c) * 0x48) * (0x10 - unk_2c) / 256);

    SetWin0Box(0, 0x50 - tmp, 0xf0, (tmp) + 0x50);

    if (unk_2c == 0x10)
    {
        Proc_Break(proc);
    }

    return;
}

void fe7u_func_080A80C4(struct ModeSelectProc * proc)
{
    s32 tmp;
    s32 unk_2c;

    unk_2c = proc->unk_2c + 1;
    proc->unk_2c = unk_2c;

    tmp = 0x48 - (((0x10 - unk_2c) * 0x48) * (0x10 - unk_2c) / 256);

    SetWin0Box(0, tmp + 8, 0xf0, -0x68 - (tmp));

    if (unk_2c == 0x10)
    {
        Proc_Break(proc);
    }

    return;
}

void fe7u_func_08054E5C(struct OpInfoData *);

void fe7u_func_080A8120(struct ModeSelectProc * proc)
{
    s32 i;

    for (i = 0; i < proc->unk_4c; i++)
    {
        fe7u_func_08054E5C(&gUnk_0201E8D4[i]);
    }

    proc->unk_50 = 0;

    return;
}

void fe7u_func_080A76F8(struct ModeSelectProc *);

void fe7u_func_080A8150(struct ModeSelectProc * proc, s32 param_2)
{
    proc->unk_43[proc->unk_41] = param_2;

    fe7u_func_080A76F8(proc);
    fe7u_func_080A7C24(param_2, proc->unk_42);

    return;
}

void fe7u_func_08054C8C(struct OpInfoData *);

void fe7u_func_080A817C(struct ModeSelectProc * proc)
{
    if (((gKeyStatusPtr->repeatedKeys & 0x40) != 0) && (proc->unk_43[proc->unk_41]) == 1)
    {
        if (!gPlaySt.config.disableSoundEffects)
        {
            // m4aSongNumStart(0x386);
            m4aSongNumStart(0x66);
        }

        fe7u_func_080A8150(proc, 0);
        return;
    }

    if ((gKeyStatusPtr->repeatedKeys & 0x80) != 0)
    {
        if (proc->unk_43[proc->unk_41] == 0)
        {

            if ((proc->unk_49[proc->unk_41] == 0) && ((proc->unk_40 & 1) == 0))
            {
                if (!gPlaySt.config.disableSoundEffects)
                {
                    //m4aSongNumStart(0x38c);
                    m4aSongNumStart(0x6c);
                }

                return;
            }

            if ((proc->unk_49[proc->unk_41] == 1) && ((proc->unk_40 & 4) == 0))
            {
                if (!gPlaySt.config.disableSoundEffects)
                {
                    //m4aSongNumStart(0x38c);
                    m4aSongNumStart(0x6c);
                }

                return;
            }

            if ((proc->unk_49[proc->unk_41] == 2) && ((proc->unk_40 & 0x10) == 0))
            {
                if (!gPlaySt.config.disableSoundEffects)
                {
                    //m4aSongNumStart(0x38c);
                    m4aSongNumStart(0x6c);
                }

                return;
            }

            if (!gPlaySt.config.disableSoundEffects)
            {
                // m4aSongNumStart(0x386);
                m4aSongNumStart(0x66);
            }

            fe7u_func_080A8150(proc, 1);

            return;
        }
    }

    if ((gKeyStatusPtr->heldKeys & 0x220) != 0)
    {
        Proc_Goto(proc, 1);

        fe7u_func_080A8D98(0);

        if (!gPlaySt.config.disableSoundEffects)
        {
            //m4aSongNumStart(0x387);
            m4aSongNumStart(0x67);
        }

        fe7u_func_080A8120(proc);

        return;
    }

    if ((gKeyStatusPtr->heldKeys & 0x110) != 0)
    {
        Proc_Goto(proc, 2);

        fe7u_func_080A8D98(1);

        if (!gPlaySt.config.disableSoundEffects)
        {
            //m4aSongNumStart(0x387);
            m4aSongNumStart(0x67);
        }

        fe7u_func_080A8120(proc);

        return;
    }

    if ((gKeyStatusPtr->newKeys & 9) != 0)
    {
        proc->unk_2c = 0;

        if (!gPlaySt.config.disableSoundEffects)
        {
            //m4aSongNumStart(0x38a);
            m4aSongNumStart(0x6a);
        }

        Proc_Goto(proc, 3);

        gUnk_0201E8D4[proc->unk_41].unk_0A = 0;
        fe7u_func_08054C8C(&gUnk_0201E8D4[proc->unk_41]);

        if ((proc->unk_42 & 1) != 0)
        {
            if (proc->unk_41 == 0)
            {
                gPlaySt.chapterModeIndex = 2;
            }

            if (proc->unk_41 == 1)
            {
                gPlaySt.chapterModeIndex = 3;
            }

            if (proc->unk_43[proc->unk_41] != 0)
            {
                gPlaySt.chapterStateBits |= 0x40;
            }
            else
            {
                gPlaySt.chapterStateBits &= 0xbf;
            }
        }
        else
        {
            savemenu_SetDifficultyChoice(proc->unk_49[proc->unk_41], proc->unk_43[proc->unk_41]);
            fe7u_func_080A7C24(proc->unk_43[proc->unk_41], proc->unk_42 | 2);
        }

        fe7u_func_080A7B7C();
        return;
    }

    if (((gKeyStatusPtr->newKeys & 2) != 0) && ((proc->unk_42 & 1) == 0))
    {
        proc->unk_2c = 0;
        if (!gPlaySt.config.disableSoundEffects)
        {
            //m4aSongNumStart(0x38b);
            m4aSongNumStart(0x6b);
        }

        Proc_Goto(proc, 4);
        savemenu_SetDifficultyChoice(3, 0);
    }

    proc->unk_50++;

    if ((proc->unk_50 & 0x1ff) == 0x20)
    {
        gUnk_0201E8D4[proc->unk_41].unk_0A = 2;
        fe7u_func_08054C8C(&gUnk_0201E8D4[proc->unk_41]);
    }

    if ((proc->unk_50 & 0x1ff) != 0x80)
    {
        return;
    }

    fe7u_func_08054E5C(&gUnk_0201E8D4[proc->unk_41]);

    return;
}

void fe7u_func_080A8424(struct ModeSelectProc * proc)
{
    proc->unk_34 = -1;
    proc->unk_2c = 0;

    StartFaceFadeOut(proc->unk_3c);

    if (proc->unk_41 == 0)
    {
        proc->unk_41 = proc->unk_4c - 1;
    }
    else
    {
        proc->unk_41--;
    }

    proc->unk_32 = (0x100 - fe7u_func_080A7C4C() * proc->unk_41) << 4;

    fe7u_func_080A8150(proc, proc->unk_43[proc->unk_41]);

    if (proc->unk_32 < proc->unk_30)
    {
        proc->unk_32 += 0x1000;
    }

    return;
}

void fe7u_func_080A848C(struct ModeSelectProc * proc)
{
    proc->unk_34 = 1;
    proc->unk_2c = 0;

    StartFaceFadeOut(proc->unk_3c);

    if (proc->unk_41 < (proc->unk_4c - 1))
    {
        proc->unk_41++;
    }
    else
    {
        proc->unk_41 = 0;
    }

    proc->unk_32 = (0x100 - fe7u_func_080A7C4C() * proc->unk_41) << 4;

    fe7u_func_080A8150(proc, proc->unk_43[proc->unk_41]);

    if (proc->unk_32 > proc->unk_30)
    {
        proc->unk_30 += 0x1000;
    }

    return;
}

void fe7u_func_080A84F8(struct ModeSelectProc * proc)
{
    s32 iVar5;
    s32 iVar7;
    s32 iVar9;
    u16 uVar10;
    s32 r9;

    iVar9 = (proc->unk_32 - proc->unk_30) * proc->unk_34;
    r9 = 0x100;
    proc->unk_2c++;

    iVar7 = iVar9 >> 2;

    iVar5 = iVar7 * (0x1e - proc->unk_2c) * (0x1e - proc->unk_2c) / 900;
    uVar10 = (proc->unk_30 + proc->unk_34 * 4 * (iVar7 - iVar5));

    if (proc->unk_2c == 0xd)
    {
        fe7u_func_080A77F8(proc->unk_49[proc->unk_41]);
    }

    if (proc->unk_2c == 0xe)
    {
        proc->unk_3c = fe7u_func_080A77C0(proc->unk_49[proc->unk_41]);
    }

    if (proc->unk_2c == 0x14)
    {
        fe7u_func_080A7668(proc->unk_49[proc->unk_41]);
    }

    if (proc->unk_2c == 0x1e)
    {
        uVar10 = proc->unk_32 & 0xfff;
        proc->unk_30 = proc->unk_32 & 0xfff;
        Proc_Break(proc);
    }

    // clang-format off
    WriteOAMRotScaleData(
        0,
        Div(+COS(0) * 16, r9),
        Div(-SIN(0) * 16, r9),
        Div(+SIN(0) * 16, r9),
        Div(+COS(0) * 16, r9)
    );
    // clang-format on

    fe7u_func_080A7C08(uVar10);

    return;
}

void fe7u_func_080A8624(struct ModeSelectProc * proc)
{
    fe7u_func_080A75CC(proc->unk_4c);
    EndEfxAnimeDrvProc();
    EndFaceById(0);

    if (!(proc->unk_42 & 1))
    {
        StartBgmVolumeChange(0x100, 0xc0, 0x10, 0);
    }
    else
    {
        SetPrimaryHBlankHandler(NULL);
    }

    return;
}

void fe7u_func_080A95B4(void);

// clang-format off

struct ProcCmd const ProcCmd_08CE4930[] = {
    PROC_CALL(fe7u_func_080A95B4),
    PROC_YIELD,

    PROC_CALL(fe7u_func_080A7C6C),
    PROC_YIELD,

    PROC_CALL(fe7u_func_080A7C84),
    PROC_YIELD,

    PROC_REPEAT(fe7u_func_080A8054),

PROC_LABEL(0),
    PROC_REPEAT(fe7u_func_080A817C),

PROC_LABEL(1),
    PROC_CALL(fe7u_func_080A848C),
    PROC_REPEAT(fe7u_func_080A84F8),

    PROC_GOTO(0),

PROC_LABEL(2),
    PROC_CALL(fe7u_func_080A8424),
    PROC_REPEAT(fe7u_func_080A84F8),

    PROC_GOTO(0),

PROC_LABEL(3),
    PROC_SLEEP(60),

PROC_LABEL(4),
    PROC_REPEAT(fe7u_func_080A80C4),
    PROC_CALL(fe7u_func_080A8624),

    PROC_END,
};

// clang-format on

void fe7u_func_080A8664(ProcPtr parent)
{
    struct ModeSelectProc * proc = Proc_StartBlocking(ProcCmd_08CE4930, parent);
    proc->unk_42 = 1;
    return;
}

void fe7u_func_080A95B4(void)
{
    SetDispEnable(0, 0, 0, 0, 0);
    return;
}

// HOOKS

extern u16 gBgConfig_SaveMenu[];

//! Hook; overwrite function at FE8U:0x080AA1EC
void sub_80AA1EC(struct SaveMenuProc* proc) {
    proc->unk_29 = 0;

    SetupBackgrounds(gBgConfig_SaveMenu);

    SetWinEnable(0, 0, 0);

    SetWin0Layers(1, 1, 1, 1, 1);
    SetWOutLayers(0, 0, 0, 0, 0);

    gLCDControlBuffer.dispcnt.mode = 0;

    gLCDControlBuffer.bg2cnt.screenSize = 0;
    gLCDControlBuffer.bg2cnt.areaOverflowMode = 0;

    gLCDControlBuffer.bg0cnt.priority = 0;
    gLCDControlBuffer.bg1cnt.priority = 1;
    gLCDControlBuffer.bg2cnt.priority = 2;
    gLCDControlBuffer.bg3cnt.priority = 3;

    return;
}