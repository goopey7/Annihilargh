#pragma once
#include <queue>

// This is just another extension of Window like the Keyboard. Refer to Keyboard.h for comments on the ideas and
// reasoning behind friending Window, Event class, etc.
class Mouse
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Move,
			LeftPress,
			LeftRelease,
			RightPress,
			RightRelease,
			MiddlePress,
			MiddleRelease,
			WheelUp,
			WheelDown,
			Enter,
			Leave,
			Invalid
		};

	private:
		Type type;
		bool bLeftIsPressed;
		bool bRightIsPressed;
		bool bMiddleIsPressed;
		int x, y;
	public:
		Event() noexcept :
			type(Type::Invalid),
			bLeftIsPressed(false), bRightIsPressed(false),
			bMiddleIsPressed(false),
			x(0), y(0)
		{
		}

		Event(Type type, const Mouse& parent) noexcept
			: type(type),
			  bLeftIsPressed(parent.bLeftIsPressed),
			  bRightIsPressed(parent.bRightIsPressed),
			  bMiddleIsPressed(parent.bMiddleIsPressed),
			  x(parent.x), y(parent.y)
		{
		}

		bool IsMove() const noexcept
		{
			return type == Type::Move;
		}

		bool IsWheelUp() const noexcept
		{
			return type == Type::WheelUp;
		}

		bool IsWheelDown() const noexcept
		{
			return type == Type::WheelDown;
		}

		Type GetType() const noexcept
		{
			return type;
		}

		std::pair<int, int> GetPos() const noexcept
		{
			return {x, y};
		}

		int GetXPos() const noexcept
		{
			return x;
		}

		int GetYPos() const noexcept
		{
			return y;
		}

		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}

		bool LeftIsPressed() const noexcept
		{
			return bLeftIsPressed;
		}

		bool RightIsPressed() const noexcept
		{
			return bRightIsPressed;
		}

		bool MiddleIsPressed() const noexcept
		{
			return bMiddleIsPressed;
		}
	};

public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	std::pair<int, int> GetPos() const noexcept;
	int GetXPos() const noexcept;
	int GetYPos() const noexcept;
	bool IsInWindow() const noexcept;
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	bool MiddleIsPressed() const noexcept;
	Event Read() noexcept; // read queue
	bool IsEmpty() const noexcept
	{
		return buffer.empty();
	}

	void Clear() noexcept; // clear/flush queue
private:
	// these functions will register our events and get called by Window in the message handler
	void OnMouseMove(int x, int y) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnMiddlePressed(int x, int y) noexcept;
	void OnMiddleReleased(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void OnWheelDelta(int x, int y, int delta) noexcept;
	void TrimBuffer() noexcept;
private:
	static constexpr unsigned int bufferSize = 16u;
	int x, y;
	bool bLeftIsPressed = false;
	bool bRightIsPressed = false;
	bool bMiddleIsPressed = false;
	bool bIsInWindow = false;
	int wheelDeltaAccumulator = 0;
	std::queue<Event> buffer;
};
