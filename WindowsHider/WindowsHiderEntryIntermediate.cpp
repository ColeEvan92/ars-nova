#include "WindowsHider.h"

extern int BleakMain(bleak::StartupData);

int WinMain_(void** data)
{
	using namespace bleak::windows;
	return BleakMain({ *reinterpret_cast<HINSTANCE*>(data[0]), *reinterpret_cast<HINSTANCE*>(data[1]), *reinterpret_cast<LPSTR*>(data[2]), *reinterpret_cast<int*>(data[3]) });
}