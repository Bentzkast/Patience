#pragma once
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <string>
#include <glad/glad.h>

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

	void Select() const;
	void Draw() const;
	void Free() const;

	// Function
	// Pos(2) Texture(2)
	static Mesh CreateSimple2D(
		const float vertices[], const Uint32 vertCount,
		const Uint32 indices[], const Uint32 indicesCount
	);

	// Pos(3) Normal(3) Texture(2)
	static Mesh CreateMesh3D(
		const float vertices[], const Uint32 vertCount,
		const Uint32 indices[], const Uint32 indicesCount);
	
	static bool LoadMeshFromObjFile(const char* filePath, Mesh& outMesh);
};

struct Shader
{
	// Field
	u32 vertexShaderHandle;
	u32 fragmentShaderHandle;
	u32 finalShaderHandle;
	
	// Method
	void Select() const;
	bool Set3fUniform(const char* name, glm::vec3 value) const; 
	bool SetMatrixUniform(const char* name, const glm::mat4& matrix) const;
	void Free() const;

	// Function
	static bool LoadSingleShader(const char* filepath, GLenum shaderType, Uint32& outHandle);
	static bool LoadFinalShader(const char* vertexPath, const char* fragPath, Shader& outShader);
};

struct Texture
{
	// Field
	u32 handle;
	u32 width;
	u32 height;

	// Method
	void Free() const;
	void Select() const;

	// Function
	static bool CreateTexture(const char* texturePath, Texture& outTexture);
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
	Mesh textMesh;
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


