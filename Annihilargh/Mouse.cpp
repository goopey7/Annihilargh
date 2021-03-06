﻿#include "Mouse.h"
#include "WindowsWithoutTheCrap.h"

std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept
{
	// if queue is empty, return null optional which means there is no data in the buffer
	if(rawDeltaBuffer.empty())
		return std::nullopt;
	// if there is stuff in the queue, get the front, and pop it.
	const RawDelta rd = rawDeltaBuffer.front();
	rawDeltaBuffer.pop();
	return rd;
}

std::pair<int, int> Mouse::GetPos() const noexcept
{
	return {x, y};
}

int Mouse::GetXPos() const noexcept
{
	return x;
}

int Mouse::GetYPos() const noexcept
{
	return y;
}

bool Mouse::IsInWindow() const noexcept
{
	return bIsInWindow;
}

bool Mouse::LeftIsPressed() const noexcept
{
	return bLeftIsPressed;
}

bool Mouse::RightIsPressed() const noexcept
{
	return bRightIsPressed;
}

bool Mouse::MiddleIsPressed() const noexcept
{
	return bMiddleIsPressed;
}

Mouse::Event Mouse::Read() noexcept
{
	// if there are events in the queue
	if (buffer.size() > 0u)
	{
		// get event in front of the queue
		Event e = buffer.front();
		buffer.pop(); // remove event from the queue
		return e;
	}
	return Event(); // returns invalid event
}

void Mouse::Clear() noexcept
{
	buffer = std::queue<Event>();
	bLeftIsPressed = false;
	bRightIsPressed = false;
	bMiddleIsPressed = false;
	bIsInWindow=false;
}

void Mouse::EnableRawInput() noexcept
{
	bIsRawEnabled=true;
}

void Mouse::DisableRawInput() noexcept
{
	bIsRawEnabled=false;
}

bool Mouse::IsRawInputEnabled() const noexcept
{
	return bIsRawEnabled;
}

void Mouse::OnRawDelta(int dx, int dy) noexcept
{
	rawDeltaBuffer.push({dx,dy});
	TrimRawInputBuffer();
}

void Mouse::OnMouseMove(int x, int y) noexcept
{
	this->x = x;
	this->y = y;
	buffer.push(Event(Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
	bIsInWindow = false;
	buffer.push(Event(Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	bIsInWindow = true;
	buffer.push(Event(Event::Type::Enter, *this));
	TrimBuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
	bLeftIsPressed = true;
	buffer.push(Event(Event::Type::LeftPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
	bLeftIsPressed = false;
	buffer.push(Event(Event::Type::LeftRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
	bRightIsPressed = true;
	buffer.push(Event(Event::Type::RightPress, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
	bRightIsPressed = false;
	buffer.push(Event(Event::Type::RightRelease, *this));
	TrimBuffer();
}

void Mouse::OnMiddlePressed(int x, int y) noexcept
{
	bMiddleIsPressed = true;
	buffer.push(Event(Event::Type::MiddlePress, *this));
	TrimBuffer();
}

void Mouse::OnMiddleReleased(int x, int y) noexcept
{
	bMiddleIsPressed = false;
	buffer.push(Event(Event::Type::MiddleRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
	buffer.push(Event(Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
	buffer.push(Event(Event::Type::WheelDown, *this));
	TrimBuffer();
}

void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
	// to support various mice including the ones with free scrolling with no notches
	// or ones with different notch gaps, we make sure that the mice are treated equally
	// and get to 120 (WHEEL_DELTA) fairly. As opposed to just checking to see if wheel delta
	// is positive or negative for up or down.
	wheelDeltaAccumulator += delta;
	while (wheelDeltaAccumulator >= WHEEL_DELTA)
	{
		wheelDeltaAccumulator -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}
	while (wheelDeltaAccumulator <= -WHEEL_DELTA)
	{
		wheelDeltaAccumulator += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
}

void Mouse::TrimBuffer() noexcept
{
	// make sure our buffer doesn't exceed bufferSize by popping elements until
	// our buffer is no more than our buffer size
	while (buffer.size() > bufferSize)
		buffer.pop();
}

void Mouse::TrimRawInputBuffer() noexcept
{
	while (rawDeltaBuffer.size() > bufferSize)
		rawDeltaBuffer.pop();
}
