#include "Keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char keyCode) const noexcept
{
	return keyStates[keyCode];
}

std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept
{
	// if there are events in the queue
	if (keyBuffer.size() > 0u)
	{
		// get event in front of the queue
		Event e = keyBuffer.front();
		keyBuffer.pop(); // remove event from the queue
		return e;
	}
	return {}; // returns invalid event
}

bool Keyboard::KeyIsEmpty() const noexcept
{
	return keyBuffer.empty();
}

void Keyboard::ClearKey() noexcept
{
	keyBuffer = std::queue<Event>();
}

std::optional<char> Keyboard::ReadChar() noexcept
{
	// if there are events in the queue
	if (charBuffer.size() > 0u)
	{
		// get char in front of the queue
		unsigned char charCode = charBuffer.front();
		charBuffer.pop(); // remove event from the queue
		return charCode;
	}
	return {};
}

bool Keyboard::CharIsEmpty() const noexcept
{
	return charBuffer.empty();
}

void Keyboard::ClearChar() noexcept
{
	charBuffer = std::queue<char>();
}

void Keyboard::Clear() noexcept
{
	ClearKey();
	ClearChar();
}

void Keyboard::EnableAutoRepeat() noexcept
{
	bIsAutoRepeat = true;
}

void Keyboard::DisableAutoRepeat() noexcept
{
	bIsAutoRepeat = false;
}

bool Keyboard::IsAutoRepeat() noexcept
{
	return bIsAutoRepeat;
}

void Keyboard::OnKeyPressed(unsigned char keyCode) noexcept
{
	keyStates[keyCode] = true; // key is being pressed
	keyBuffer.push(Event(Event::Type::Press, keyCode)); // construct Press event
	TrimBuffer(keyBuffer); // just in case buffer gets too big
}

void Keyboard::OnKeyReleased(unsigned char keyCode) noexcept
{
	keyStates[keyCode] = false; // key is not being pressed
	keyBuffer.push(Event(Event::Type::Release, keyCode)); // construct Release event
	TrimBuffer(keyBuffer); // just in case buffer gets too big
}

void Keyboard::OnChar(char character) noexcept
{
	// we don't have to worry about states here since char events are
	// for typing and stuff not game inputs
	charBuffer.push(character);
	TrimBuffer(charBuffer);
}

void Keyboard::ClearState() noexcept
{
	keyStates.reset();
}
