#include <windows.h>
#include "gens.h"
#include "Rom.h"
#include "mem_M68K.h"
#include "movie.h"
#include "vdp_io.h"
#include "vdp_rend.h"
#include "G_ddraw.h"

#define GENS_EXPORT extern "C" __declspec(dllexport)

extern Rom *My_Rom;
extern BOOL Init();
extern void Init_Genesis_Bios();
extern void De_Interleave(void);
extern void Fill_Infos(void);
extern int (*Update_Frame)();
extern void MakeBitmapHeader(unsigned char* Dest, int BmpSize);
extern void MakeBitmapInfo(unsigned char* Dest, int Width, int Height, int bpp = 24);
extern void WriteFrame(void* Screen, unsigned char* Dest, int mode, int Hmode, int Vmode, int X, int Y);

GENS_EXPORT int gens_init(char *rom, int romLength)
{
	int interleaved = 0;
	
	Init();
	Init_Genesis_Bios();
	
	//Load ROM
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
	Recalculate_Palettes();
	//FrameCount = 0;
	//LagCount = 0;
	
	Bits32 = 1;
	Do_VDP_Refresh();

	return 0;
}

GENS_EXPORT bool gens_advance(int *vbuff)
{
	Update_Frame();

	int mode = 2;
	int X = IS_FULL_X_RESOLUTION ? 320 : 256;
	int Y = IS_FULL_Y_RESOLUTION ? 240 : 224;
	int i = (X * Y * 3) + 54;
	memset(vbuff, 0, i);

	MakeBitmapHeader((unsigned char *)vbuff, i);
	MakeBitmapInfo((unsigned char *)vbuff + 14, X, Y);

	WriteFrame(
		(unsigned char*)MD_Screen32,
		(unsigned char *)vbuff + 54,
		mode,
		IS_FULL_X_RESOLUTION,
		IS_FULL_Y_RESOLUTION,
		X, Y);

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
