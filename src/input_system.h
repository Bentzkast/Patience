#include <glm/glm.hpp>
#include <SDL2/SDL.h>

enum EButtonState{
	None,
	Pressed,
	Released,
	Hold,
};


enum KeyboardButton{
	Forward,
	Backward,
	Left,
	Right,
	RotateLeft,
	RotateRight,
	Exit,
	GBCount,
};

enum MouseButton{
	LEFT_MOUSE = SDL_BUTTON_LEFT,
	RIGHT_MOUSE = SDL_BUTTON_RIGHT
};


class InputSystem
{
	Uint8 prevButton[SDL_NUM_SCANCODES];
	SDL_Point mousePos;
	Uint32 currentMouseButton;
	Uint32 prevMouseButton;

	const Uint8* currentButton;

	Uint8 keyBinding[GBCount];
	bool forceQuitSignal = false;
	
public:
	void Init();
	void LateUpdate();
	bool IsNone(KeyboardButton button) const;
	bool IsPressed(KeyboardButton button) const;
	bool IsHold(KeyboardButton button) const;
	bool IsReleased(KeyboardButton button) const;

	bool IsNone(MouseButton button) const;
	bool IsPressed(MouseButton button) const;
	bool IsHold(MouseButton button) const;
	bool IsReleased(MouseButton button) const;
	void SetForceQuit();

	const SDL_Point& GetMousePos() const;
};
