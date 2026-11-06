#include <windows.h>
#include "gens.h"
#include "Rom.h"
#include "mem_M68K.h"
#include "movie.h"
#include "vdp_io.h"
#include "vdp_rend.h"
#include "G_ddraw.h"
#include "io.h"

#define GENS_EXPORT extern "C" __declspec(dllexport)

extern Rom *My_Rom;
extern BOOL Init();
extern struct ControlPadData controllerVariables[8];
extern int (*Update_Frame)();
extern void Init_Genesis_Bios();
extern void De_Interleave();
extern void Fill_Infos();

GENS_EXPORT int gens_init(char *rom, int romLength)
{
	int interleaved = 0;
	
	Bits32 = 1;

	Init();
	Init_Genesis_Bios();
	
	if (strnicmp("SEGA", &rom[0x100], 4))
	{
		if (!strnicmp("EA", &rom[0x200 + (0x100 / 2)], 2)) interleaved = 1;
		if ((rom[0x08] == 0xAA) && (rom[0x09] == 0xBB) && (rom[0x0A] == 0x06)) interleaved = 1;
	}

	memset(Rom_Data, 0, sizeof(Rom_Data));

	if (romLength <= sizeof(Rom_Data))
	{
		memcpy(Rom_Data, rom, romLength);
		Rom_Size = romLength;
	}

	My_Rom = (Rom *)malloc(sizeof(Rom)); // freed later in Free_Rom

	if(!Rom_Size || !My_Rom)
		return -1;

	if (interleaved)
		De_Interleave();

	Fill_Infos();
	Game = My_Rom;
	Genesis_Started = Init_Genesis(Game);

	return 0;
}

GENS_EXPORT bool gens_advance(int *vbuff, int input)
{
	Update_Frame();
	unsigned int *Source = MD_Screen32 + 8;

	int Y = IS_FULL_Y_RESOLUTION ? 240 : 224;
	int X = IS_FULL_X_RESOLUTION ? 320 : 256;

	for (int y = 0; y < Y; y++)
	{
		for (int x = 0; x < X; x++)
		{
			*vbuff++ = *Source++;
		}
		Source += 16;
	}
	
	Controller_1_Up    = (input & (1 << 0x00)) ? 0 : 1;
	Controller_1_Down  = (input & (1 << 0x01)) ? 0 : 1;
	Controller_1_Left  = (input & (1 << 0x02)) ? 0 : 1;
	Controller_1_Right = (input & (1 << 0x03)) ? 0 : 1;
	Controller_1_Start = (input & (1 << 0x04)) ? 0 : 1;
	Controller_1_A     = (input & (1 << 0x05)) ? 0 : 1;
	Controller_1_B     = (input & (1 << 0x06)) ? 0 : 1;
	Controller_1_C     = (input & (1 << 0x07)) ? 0 : 1;
	Controller_1_X     = (input & (1 << 0x08)) ? 0 : 1;
	Controller_1_Y     = (input & (1 << 0x09)) ? 0 : 1;
	Controller_1_Z     = (input & (1 << 0x0a)) ? 0 : 1;
	Controller_1_Mode  = (input & (1 << 0x0b)) ? 0 : 1;
	Controller_2_Up    = (input & (1 << 0x10)) ? 0 : 1;
	Controller_2_Down  = (input & (1 << 0x11)) ? 0 : 1;
	Controller_2_Left  = (input & (1 << 0x12)) ? 0 : 1;
	Controller_2_Right = (input & (1 << 0x13)) ? 0 : 1;
	Controller_2_Start = (input & (1 << 0x14)) ? 0 : 1;
	Controller_2_A     = (input & (1 << 0x15)) ? 0 : 1;
	Controller_2_B     = (input & (1 << 0x16)) ? 0 : 1;
	Controller_2_C     = (input & (1 << 0x17)) ? 0 : 1;
	Controller_2_X     = (input & (1 << 0x18)) ? 0 : 1;
	Controller_2_Y     = (input & (1 << 0x19)) ? 0 : 1;
	Controller_2_Z     = (input & (1 << 0x1a)) ? 0 : 1;
	Controller_2_Mode  = (input & (1 << 0x1b)) ? 0 : 1;

	return false;
}

GENS_EXPORT char gens_peek_m68k(int addr)
{
	return Ram_68k[addr];
}

GENS_EXPORT char gens_peek_rom(int addr)
{
	return Rom_Data[addr ^ 1];
}
