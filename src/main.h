#pragma once
#include <SDL2/SDL.h>
#include "../external/glm/glm.hpp"
typedef uint64_t u64;
typedef uint32_t u32;
typedef int i32;

struct ApplicationState
{
	u32 windowWidth;
	u32 windowHeight;
	SDL_Window* sdlWindow;
	SDL_GLContext sdlGLContext;
	bool isRunning;
};

struct GameState
{
	void* memory;
	u64 used;
	u64 size;
};

struct Mesh
{
	u32 vertexArrayHandle;
	u32 vertexBufferHandle;
	u32 elementBufferHandle;
	u32 vertexArrayCount;
	u32 elementBufferCount;  
};

struct Shader
{
	u32 vertexShaderHandle;
	u32 fragmentShaderHandle;
	u32 finalShaderHandle;
};

struct Texture
{
	u32 handle;
	u32 width;
	u32 height;
};

struct GameResources
{
	Mesh quadMesh;
	Mesh cubeMesh;
	Shader flatShader;
	Texture paletteTexture;
	float rotation;
};

struct Player
{
	i32 x;
	i32 y;
};

typedef void PlayerAction(Player* player);
struct PlayerActionTable
{
	const char* name;
	const PlayerAction* action;
};

// Interface
namespace Resources
{
	void InitResources();
	void FreeResources();
	void DrawMesh(float dt);
}
