#include "input_system.h"

void InputSystem::Init()
{
	currentButton = SDL_GetKeyboardState(NULL);
	SDL_memset(prevButton, 0, SDL_NUM_SCANCODES);
	// Keyboard key binding
	keyBinding[Forward] = SDL_SCANCODE_W;
	keyBinding[Backward] = SDL_SCANCODE_S;
	keyBinding[RotateLeft] = SDL_SCANCODE_Q;
	keyBinding[RotateRight] = SDL_SCANCODE_E;
	keyBinding[Left] = SDL_SCANCODE_A;
	keyBinding[Right] = SDL_SCANCODE_D;
	keyBinding[Exit] = SDL_SCANCODE_ESCAPE;
}

void InputSystem::LateUpdate()
{
	prevMouseButton = currentMouseButton;
	currentMouseButton = SDL_GetMouseState(&mousePos.x, &mousePos.y);
	SDL_memcpy(prevButton, currentButton, SDL_NUM_SCANCODES);
}

bool InputSystem::IsNone(MouseButton button) const
{
	int mask = SDL_BUTTON(button);
	return !(currentMouseButton & mask) && !(prevMouseButton & mask);
}
bool InputSystem::IsPressed(MouseButton button) const
{
	int mask = SDL_BUTTON(button);
	return (currentMouseButton & mask) && !(prevMouseButton & mask);
}
bool InputSystem::IsHold(MouseButton button) const
{
	int mask = SDL_BUTTON(button);
	return (currentMouseButton & mask) && (prevMouseButton & mask);
}
bool InputSystem::IsReleased(MouseButton button) const
{
	int mask = SDL_BUTTON(button);
	return !(currentMouseButton & mask) && (prevMouseButton & mask);
}

bool InputSystem::IsNone(KeyboardButton button) const
{
	return !currentButton[keyBinding[button]] && !prevButton[keyBinding[button]];
}

bool InputSystem::IsPressed(KeyboardButton button) const
{
	return currentButton[keyBinding[button]] && !prevButton[keyBinding[button]];
}

bool InputSystem::IsHold(KeyboardButton button) const
{
	return currentButton[keyBinding[button]] && prevButton[keyBinding[button]];
}

bool InputSystem::IsReleased(KeyboardButton button) const
{
	return !currentButton[keyBinding[button]] && prevButton[keyBinding[button]];
}

void InputSystem::SetForceQuit()
{
	this->forceQuitSignal = true;
}

const SDL_Point& InputSystem::GetMousePos() const
{
	return this->mousePos;
}