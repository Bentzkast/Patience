#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glad/glad.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef int i32;

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
		const float vertices[], const u32 vertCount,
		const u32 indices[], const u32 indicesCount
	);

	// Pos(3) Normal(3) Texture(2)
	static Mesh CreateMesh3D(
		const float vertices[], const u32 vertCount,
		const u32 indices[], const u32 indicesCount);
	
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

// Just a group of handless, maybe become group of vectors in the future...
struct Resources
{
	Mesh quadMesh;
	Mesh treeMesh;
	Mesh houseMesh;
	Mesh selectionMesh;

	Shader flatShader;
	Shader textShader;

	Texture paletteTexture;
	Texture panelTexture;

	// Helper ?? moveout to free func..!!!
	void InitResources();
	void FreeResources();
	
	// remove
	void DrawMesh(float dt, const glm::mat4& projectionTimesView,
		const glm::vec3& worldCursorPos);
};