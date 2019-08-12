#include <Windows.h>

extern int WinMain_(void**);

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdArgs, int nCmdShow)
{
	void* data[4] = { &hInstance, &hPrevInstance, &lpCmdArgs, &nCmdShow };
	return WinMain_(data);
}