#include "GDIPlusManager.h"
#include <Unknwn.h> 
#include "WindowsWithoutTheCrap.h"
#include <ObjIdl.h>
#include <algorithm>

namespace Gdiplus
{
	using std::min;
	using std::max;
}
#include <gdiplus.h>
#pragma comment( lib,"gdiplus.lib" )
ULONG_PTR GDIPlusManager::token = 0;
int GDIPlusManager::refCount = 0;
GDIPlusManager::GDIPlusManager()
{
	if(refCount==0)
	{
		Gdiplus::GdiplusStartupInput input;
		input.GdiplusVersion = 1;
		input.DebugEventCallback = nullptr;
		input.SuppressBackgroundThread = false;
		GdiplusStartup(&token,&input,nullptr);
	}
	refCount++;
}

GDIPlusManager::~GDIPlusManager()
{
	refCount--;
	if(refCount==0)
		Gdiplus::GdiplusShutdown(token);
}
