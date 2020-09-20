#pragma once
#include <string>
#include <vector>
#include "resources.h"
#include "input_system.h"
#include "camera.h"

struct Interactable2D{
	SDL_Rect rect { 0 };
	std::string name {""};
	glm::vec3 backgroundColor { 0 };
	glm::vec3 foregroundColor { 0 };
	// highlight color
	// maybe has priotity layer latter..
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

	InputSystem inputSystem;
	static const int MAX_INTERACTABLE = 20;
	int currentInteractableSize = 0;
	Interactable2D interactableUI[MAX_INTERACTABLE];
	bool isRunning = false;
	Camera camera;
	// TEMP
	glm::vec3 intersectionPoint;

public:
	void Init();
	void Run(const ApplicationState& appState);
	void Free();
};