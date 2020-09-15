#pragma once
#include <SDL2/SDL.h>
#include "../external/glm/glm.hpp"
#include <string>
typedef uint64_t u64;
typedef uint32_t u32;
typedef int i32;

// Priotize POD, class should be a module, not an "object"
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

	void Select() const;
	bool Set3fUniform(const char* name, glm::vec3 value) const; 
	bool SetMatrixUniform(const char* name, const glm::mat4& matrix) const;
};

struct Texture
{
	u32 handle;
	u32 width;
	u32 height;
};

struct Character
{
	u32 textureHandle; 		// Id handle of the glyph texture
	glm::ivec2 size;		// size of the glyph
	glm::ivec2 bearing;		// offset from baseline to left/top
	u32 advance;			// offset to advance to the next gylph
};


constexpr u32 characterCount { 128 };
class TextRenderer
{
	Character characterTable[characterCount];
	glm::mat4 projection;
	u32 vertexArrayHandle, vertexBufferHandle, elementBufferHandle;
public:
	bool Initialize();
	void RenderText(Shader& shader, std::string& text, float x, float y, float scale, glm::vec3 color);
	void Free();
};

struct GameResources
{
	Mesh quadMesh;
	Mesh cubeMesh;
	Shader flatShader;
	Shader textShader;
	Texture paletteTexture;
	float rotation;
	TextRenderer textRenderer;
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

// Interface Should be a class
namespace Resources
{
	void InitResources();
	void FreeResources();
	void DrawMesh(float dt);
	void DrawText();
}


