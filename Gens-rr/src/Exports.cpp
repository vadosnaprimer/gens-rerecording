#include <windows.h>
#include "gens.h"

#define GENS_EXPORT extern "C" __declspec(dllexport)

extern BOOL Init();
extern void Init_Genesis_Bios();

GENS_EXPORT int gens_init()
{
	Init();
	Init_Genesis_Bios();

	return 0;
}