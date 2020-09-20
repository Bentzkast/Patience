#pragma once
#include <string>
#include <vector>
#include "resources.h"

enum class GameAction
{
	NONE,
	WORLD_SELECT, 
	TEST, // DO WE WANT TO HARD CODE EVERY THING?? make a class instead??
	SCROLL_FORWARD,
	SCROLL_BACK,
	SCROLL_LEFT,
	SCROLL_RIGHT,
	FOCUS_ON, // SETING we may need arg
	HARD_EXIT
};

struct GameInput{
	GameAction action;
	int x; int y; // used for tool tips and such
	GameInput(GameAction b, int x = 0, int y = 0) : action{b}, x{x}, y{y}{}
};

struct Interactable2D{
	SDL_Rect rect;
	std::string name;
	GameAction action;
	glm::vec3 backgroundColor;
	glm::vec3 foregroundColor;
	// highlight color
	// maybe has priotity layer latter..
	Interactable2D(
		SDL_Rect rect, const char* name, GameAction a, glm::vec3 bgc, glm::vec3 fgc) :
		rect{rect}, name {name}, action { a },
		backgroundColor {bgc},foregroundColor {fgc} {}
};

// Think about hiding this later...
struct Character
{
	u32 textureHandle; 		// Id handle of the glyph texture
	glm::ivec2 size;		// size of the glyph
	glm::ivec2 bearing;		// offset from baseline to left/top
	u32 advance;			// offset to advance to the next gylph
};

constexpr u32 characterCount { 128 };
class UIRenderer
{
	struct Character characterTable[characterCount];
	glm::mat4 projection;
	Mesh uiMesh;
	u32 charHeight;
	i32 windowWidth;
	i32 windowHeight;
public:
	bool Initialize(i32 width, i32 height);
	void SetWindowSize(i32 width, i32 height);
	void RenderText(const Resources& resources, std::string& text, float x, float y, float scale, glm::vec3 color);
	void RenderPanel(const Resources& resources, const SDL_Rect& rect, glm::vec3 color);
	void Free();
};


// Priotize POD, class should be a module, not an "object"
struct ApplicationState
{
	u32 windowWidth;
	u32 windowHeight;
	SDL_Window* sdlWindow;
	SDL_GLContext sdlGLContext;
	bool isRunning;
};

class Game
{
	// Fields
	Resources resources;
	UIRenderer uiRenderer;

	std::vector<GameInput> gameInputs; // Do we event need this to be a vector ..
	std::vector<Interactable2D> interactable2D;
	bool isRunning = false;

	// TEMP
	glm::vec2 cameraRigPos;
	glm::vec3 intersectionPoint;

public:
	void Init();
	void Run(const ApplicationState& appState);
	void Free();
};