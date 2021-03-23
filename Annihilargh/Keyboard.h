#pragma once
#include <bitset>
#include <optional>
#include <queue>

class Keyboard
{
	// the keyboard befriends Window because we want Window to be able to call our private event functions
	// ex. OnKeyPressed
	// this does mean that everything private is exposed between Window and Keyboard, but that should be fine.
	// Keyboard can be seen as an extension of Window anyways
	// Just good design not to have Window doing everything
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};

	private:
		Type type;
		unsigned char code;
	public:
		Event() noexcept : type(Type::Invalid), code(0u)
		{
		}

		Event(Type type, unsigned char code) noexcept
			: type(type), code(code)
		{
		}

		bool IsPress() const noexcept
		{
			return type == Type::Press;
		}

		bool IsRelease() const noexcept
		{
			return type == Type::Release;
		}

		unsigned char GetCode() const noexcept
		{
			return code;
		}

		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}

		Type GetType() const noexcept
		{
			return type;
		}
	};

	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;

	// KEY EVENTS
	// pass a keycode, and it will determine if the key is being pressed
	bool KeyIsPressed(unsigned char keyCode) const noexcept;
	std::optional<Event> ReadKey() noexcept; // pulls event off of the event queue
	bool KeyIsEmpty() const noexcept; // checks to see if any keys are in the event queue
	void ClearKey() noexcept; // clears event queue

	// CHAR EVENTS
	std::optional<char> ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void ClearChar() noexcept;

	void Clear() noexcept; // Flushes both Key and Char

	// AUTOREPEAT
	void EnableAutoRepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool IsAutoRepeat() noexcept;
private:
	// EXPOSED TO WINDOW - Get called by message handler
	void OnKeyPressed(unsigned char keyCode) noexcept;
	void OnKeyReleased(unsigned char keyCode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept; // clear the bitset that holds key states

	template <typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;
	static constexpr unsigned int nKeys = 256u; // there are 256 virtual keycodes
	static constexpr unsigned int bufferSize = 16u;
	bool bIsAutoRepeat = false;
	std::bitset<nKeys> keyStates;
	std::queue<Event> keyBuffer;
	std::queue<char> charBuffer;
};

template <typename T> // templated so it works on both char queues and event queues
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	// make sure our buffer doesn't exceed bufferSize by popping elements until
	// our buffer is no more than our buffer size
	while (buffer.size() > bufferSize)
		buffer.pop();
}
