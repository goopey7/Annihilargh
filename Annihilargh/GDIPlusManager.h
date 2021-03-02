#pragma once
#include <Unknwn.h> 
#include "WindowsWithoutTheCrap.h"
#include <ObjIdl.h>

class GDIPlusManager
{
public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR token;
	static int refCount;
};