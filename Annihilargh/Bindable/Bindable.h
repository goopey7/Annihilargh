#pragma once
#include "../Graphics.h"

class Bindable
{
public:
	virtual void Bind(Graphics &gfx) noexcept = 0;
	virtual ~Bindable() = default;
protected:
	// Device and Context are private members of graphics.
	// So if we make Bindable a friend of graphics, Bindable will get access to private members, but its children won't
	// But the children call call these two static functions to get the only two things they need from graphics.
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
};
