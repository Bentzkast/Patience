#include "main.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <stb_image.h>
#include "../external/glm/gtc/type_ptr.hpp"
#include <vector>
#include <unordered_map>
#include <iostream>
#include "../external/glm/gtx/string_cast.hpp"

struct VertEntry {
	int v;
	int n;
	int t;

	bool operator==(const VertEntry &other) const
	{
		return (v == other.v
			&& n == other.n
			&& t == other.t);
	}
};

namespace std {
	template <>
	struct hash<VertEntry>
	{
		std::size_t operator()(const VertEntry& e) const
		{
			using std::hash;
			std::ostringstream oss;
			oss << e.v << "/" << e.n << "/" << e.t;

			return hash<std::string>()(oss.str());
		}
	};
}

namespace PTC_Mesh
{
	static Mesh CreateMesh(
		const float vertices[], const Uint32 vertCount,
		const Uint32 indices[], const Uint32 indicesCount)
	{
		u32 vertexArrayHandle { 0 };
		u32 vertexBufferHandle { 0 };
		u32 elementBufferHandle { 0 };

		SDL_Log("Creating Mesh count vertCount: %d, indexCount: %d", vertCount, indicesCount);
		// create
		glGenVertexArrays(1, &vertexArrayHandle);
		glGenBuffers(1, &vertexBufferHandle);
		glGenBuffers(1, &elementBufferHandle);

		// select buffer 
		glBindVertexArray(vertexArrayHandle);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferHandle);

		// set input data
		glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(float), vertices, GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(Uint32), indices, GL_STATIC_DRAW);

		// Give data to the vertex shader
		const int totalStrideBytes = 8 * sizeof(float); // Stride

		// Position
		{
			constexpr int posAttrIndex = 0;
			constexpr int posDataCount = 3;
			constexpr int posAttrOffsetBytes = 0;
			glVertexAttribPointer(posAttrIndex, posDataCount, GL_FLOAT, GL_FALSE, totalStrideBytes, (void*)posAttrOffsetBytes);
			glEnableVertexAttribArray(posAttrIndex);
		}
		// Normal
		{
			constexpr int normalAttrIndex = 1;
			constexpr int normalDataCount = 3;
			constexpr int normalAttrOffSetBytes = 3 * sizeof(float);
			glVertexAttribPointer(normalAttrIndex, normalDataCount, GL_FLOAT, GL_FALSE, totalStrideBytes, (void*)normalAttrOffSetBytes);
			glEnableVertexAttribArray(normalAttrIndex);
		}
		// Texture
		{
			constexpr int textureAttrIndex = 2;
			constexpr int textureDataSize = 2;
			constexpr int textureAttrOffsetBytes = (3 + 3) * sizeof(float);
			glVertexAttribPointer(
				textureAttrIndex, textureDataSize,
				GL_FLOAT, GL_FALSE,
				totalStrideBytes,
				(void*)textureAttrOffsetBytes);
			glEnableVertexAttribArray(textureAttrIndex);
		}

		return Mesh 
		{
			vertexArrayHandle,
			vertexBufferHandle,
			elementBufferHandle,
			vertCount,
			indicesCount
		};
	}

	static bool LoadMeshFromObjFile(const char* filePath, Mesh& outMesh)
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uv;

		std::vector<float> glVert;
		std::vector<Uint32> glIndices;
		Uint32 glVertCount = 0;
		std::unordered_map<VertEntry, Uint32> vertEntryIndices;
		std::ifstream objFile{ filePath };
		if (objFile.is_open())
		{			
			std::string line;
			while (std::getline(objFile, line))
			{
				if (line.substr(0, 2) == "v ")
				{
					std::istringstream v(line.substr(2));
					double x, y, z;
					v >> x;
					v >> y;
					v >> z;
					vertices.emplace_back(x, y, z);
				}
				else if (line.substr(0, 2) == "vt")
				{
					std::istringstream t(line.substr(3));
					float u, v;
					t >> u; 
					t >> v;
					uv.emplace_back(u, v);
				}
				else if (line.substr(0, 2) == "vn") 
				{
					std::istringstream n(line.substr(3));
					double x, y, z;
					n >> x;
					n >> y;
					n >> z;
					normals.emplace_back(x, y, z);
				}
				else if (line.substr(0, 2) == "f ") 
				{
					const char* faces = line.c_str();
					VertEntry e[3];
					
					sscanf(faces, "f %i/%i/%i %i/%i/%i %i/%i/%i",
						&e[0].v, &e[0].t, &e[0].n,
						&e[1].v, &e[1].t, &e[1].n,
						&e[2].v, &e[2].t, &e[2].n);

					for (auto v : e)
					{
						auto iter = vertEntryIndices.find(v);
						if (iter != vertEntryIndices.end())
						{
							glIndices.emplace_back(iter->second);
						}
						else {
							// New Entry
							vertEntryIndices.emplace(v, glVertCount);
							v.n--; v.v--; v.t--;
							glVert.emplace_back(vertices[v.v].x);
							glVert.emplace_back(vertices[v.v].y);
							glVert.emplace_back(vertices[v.v].z);
							glVert.emplace_back(normals[v.n].x);
							glVert.emplace_back(normals[v.n].y);
							glVert.emplace_back(normals[v.n].z);
							glVert.emplace_back(uv[v.t].x);
							glVert.emplace_back(uv[v.t].y);
							glIndices.emplace_back(glVertCount);
							glVertCount++;
						}
					}
				}
			}
		}
		else
		{
			SDL_Log("Failed to find obj file %s", filePath);
			return false;
		}

		outMesh = CreateMesh(glVert.data(), glVert.size(), glIndices.data(), glIndices.size());
		return true;
	}

	static void FreeMesh(const Mesh& mesh)
	{
		SDL_Log("Destroying Mesh");
		glDeleteBuffers(1, &mesh.vertexBufferHandle);
		glDeleteBuffers(1, &mesh.elementBufferHandle);
		glDeleteBuffers(1, &mesh.vertexArrayHandle);
	}

	static void SelectMesh(const Mesh& mesh)
	{
		glBindVertexArray(mesh.vertexArrayHandle);
	}

	static void DrawMesh(const Mesh& mesh)
	{
		glDrawElements(GL_TRIANGLES, mesh.elementBufferCount, GL_UNSIGNED_INT, 0);
	}


} // namespace PTC_Mesh

void Shader::Select() const
{
	glUseProgram(this->finalShaderHandle);
}

bool Shader::Set3fUniform(const char* name, glm::vec3 value) const
{
	GLint loc = glGetUniformLocation(this->finalShaderHandle, name);
	// Send the matrix data to the uniform
	if (loc != -1)
	{
		glUniform3f(loc, value.x, value.y, value.z);
		return false;
	}
	return true;
}

bool Shader::SetMatrixUniform(const char* name, const glm::mat4& matrix) const
{
	GLint loc = glGetUniformLocation(this->finalShaderHandle, name);
	// Send the matrix data to the uniform
	if (loc != -1)
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
		return false;
	}
	return true;
}

namespace PTC_Shader
{
	static bool LoadSingleShader(const char* filepath, GLenum shaderType, Uint32& outHandle)
	{
		std::ifstream sourceFile{ filepath };
		if (sourceFile.is_open())
		{
			// Read as a stream
			std::stringstream stringStream;
			stringStream << sourceFile.rdbuf();
			std::string contents = stringStream.str();
			const char* contentsChar = contents.c_str();

			outHandle = glCreateShader(shaderType);
			glShaderSource(outHandle, 1, &(contentsChar), NULL);
			glCompileShader(outHandle);

			// Error checking
			int success;
			char infoLog[512];
			glGetShaderiv(outHandle, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(outHandle, 512, NULL, infoLog);
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to compile shader file : %s", filepath);
				return false;
			}
			return true;
		}
		else
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open shader file: %s", filepath);
			return false;
		}
		return true;
	}

	static bool LoadFinalShader(const char* vertexPath, const char* fragPath, Shader& outShader)
	{
		Uint32 vertexShaderHandle { 0 };
		Uint32 fragmentShaderHandle { 0 };

		if (!LoadSingleShader(vertexPath, GL_VERTEX_SHADER, vertexShaderHandle))
		{
			SDL_Log("Failed to Load Vertex Shader");
			return false;
		}
		if (!LoadSingleShader(fragPath, GL_FRAGMENT_SHADER, fragmentShaderHandle))
		{
			SDL_Log("Failed to Load Frag Shader");
			return false;
		}

		Uint32 shaderHandle = glCreateProgram();
		glAttachShader(shaderHandle, vertexShaderHandle);
		glAttachShader(shaderHandle, fragmentShaderHandle);
		glLinkProgram(shaderHandle);
		
		// Check Link error
		int success;
		char infoLog[512];
		glGetProgramiv(shaderHandle, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(fragmentShaderHandle, 512, NULL, infoLog);
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "GL_glLinkProgram: %s", infoLog);
			return false;
		}
		SDL_Log("Loaded & Compiled %s", vertexPath);
		SDL_Log("Loaded & Compiled %s", fragPath);

		outShader.vertexShaderHandle = vertexShaderHandle;
		outShader.fragmentShaderHandle = fragmentShaderHandle;
		outShader.finalShaderHandle = shaderHandle;
		return true;
	}

	static void FreeShader(const Shader& shader)
	{
		glDeleteProgram(shader.finalShaderHandle);
		glDeleteShader(shader.vertexShaderHandle);
		glDeleteShader(shader.fragmentShaderHandle);	
	}


}


namespace PTC_Texture
{
	static bool CreateTexture(const char* texturePath, Texture& outTexture)
	{
		Uint32 glTextureHandle { 0 };
		int width { 0 };
		int height { 0 };
		stbi_set_flip_vertically_on_load(true);
		// Create glTexture
		glGenTextures(1, &glTextureHandle);
		glBindTexture(GL_TEXTURE_2D, glTextureHandle);
		// glTexture settings
		// texture wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load image 
		int  nrChannels;
		Uint8 * imageData = stbi_load(texturePath, &width, &height, &nrChannels, 0);
		if (!imageData)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to find image file : %s", texturePath);
			return false;
		}
		int format = GL_RGB;
		if (nrChannels == 4)
		{
			format = GL_RGBA;
		}
		// Push image to glTexture
		glTexImage2D(
			GL_TEXTURE_2D,				// Same as selected texture
			0,							// mipmap level, if we want to do it manually 0 base level
			format,	// Format to store (dest)
			width, height,				// dimension of the texture
			0,							// always zero, legacy stuff
			format,	// Format source
			GL_UNSIGNED_BYTE,			// Format source
			imageData);		// Actual image data
			// filter options, billeniar filtering

		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(imageData);

		outTexture.handle = glTextureHandle;
		outTexture.width = width;
		outTexture.height = height;
		return true;
	}

	static void FreeTexture(const Texture& texture)
	{
		glDeleteTextures(1, &texture.handle);
	}

	static void SelectTexture(const Texture& texture)
	{
		glBindTexture(GL_TEXTURE_2D, texture.handle);
	}
}

// Hack scoping..
namespace Resources {
	static GameResources g_resources = { 0 };
	void InitResources()
	{
		if(!PTC_Shader::LoadFinalShader(
			"asset/shader/vertex_basic.vert",
			"asset/shader/frag_basic.frag",g_resources.flatShader))
			{
				SDL_Log("Failed to load flat Shader");
			}

		if(!PTC_Shader::LoadFinalShader(
			"asset/shader/text.vert",
			"asset/shader/text.frag",g_resources.textShader))
			{
				SDL_Log("Failed to load text Shader");
			}


		if(!PTC_Texture::CreateTexture(
			"asset/texture/Simple.png", g_resources.paletteTexture
		))
		{
			SDL_Log("Failed to load Texture");
		}

		// Create Quad model
		float quadVertices[] = {
			// Pos			// normal	  // texture mapping
			-0.5f,  0.5f, 0.f, .0f, .0f, .0f, .0f, 1.0f, // top left
			0.5f,  0.5f, 0.f, .0f, .0f, .0f, 1.0f, 1.0f, // top right
			0.5f, -0.5f, 0.f, .0f, .0f, .0f, 1.0f, .0f, // bottom right
			-0.5f, -0.5f, 0.f, .0f, .0f, .0f, .0f, .0f  // bottom left
		};
		Uint32 quadIndices[] = {
			0, 1, 2,
			2, 3, 0,
		};
		g_resources.quadMesh = PTC_Mesh::CreateMesh(quadVertices, 4 * 8, quadIndices, 6);
		if(!PTC_Mesh::LoadMeshFromObjFile("asset/mesh/CubeSplit.obj", g_resources.cubeMesh))
		{
			SDL_Log("Failed to load cube mesh");
		}

		g_resources.rotation = 0;
		g_resources.textRenderer.Initialize();
	}

	void DrawMesh(float dt)
	{
		// {
		// 	glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(64.0f,64.0f, 1.0f));
		// 	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
		// 	glm::mat4 projectionScreen = glm::ortho(0.0f, 1080.0f, 0.0f, 720.0f, 0.1f, 100.0f);

		// 	glm::mat4 screenFinal = projectionScreen * view * transform;

		// 	g_resources.flatShader.Select();
		// 	g_resources.flatShader.SetMatrixUniform("transform", screenFinal);
		// 	PTC_Texture::SelectTexture(g_resources.paletteTexture);
		// 	PTC_Mesh::SelectMesh(g_resources.quadMesh);
		// 	PTC_Mesh::DrawMesh(g_resources.quadMesh);
		// }

		{
			g_resources.rotation += dt * 10.0f;
			glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(g_resources.rotation), glm::vec3(1.0f, 0.0f, 1.0f));
			glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -.5f, -25.0f));
			//view = glm::rotate(view, glm::radians(45.f), glm::vec3(1.0f, 0.0f, 0.0f));
			glm::mat4 projection = glm::perspective(glm::radians(60.0f), 1080.0f / 720.0f, 0.1f, 200.0f);
			glm::mat4 final = projection * view * transform;
			g_resources.flatShader.Select();
			g_resources.flatShader.SetMatrixUniform("transform", final);
			PTC_Texture::SelectTexture(g_resources.paletteTexture);
			PTC_Mesh::SelectMesh(g_resources.cubeMesh);
			PTC_Mesh::DrawMesh(g_resources.cubeMesh);

			//std::cout << glm::to_string(g_resources.temp);
		}
	}

	void DrawText()
	{
		std::string sampleText { "This is sample text" };
		g_resources.textRenderer.RenderText(
			g_resources.textShader,
			sampleText,
			25.0f, 25.0f, 1.0f,
			{ 0.3, 0.7 , 0.9 });
	}

	void FreeResources()
	{
		PTC_Mesh::FreeMesh(g_resources.quadMesh);
		PTC_Mesh::FreeMesh(g_resources.cubeMesh);
		PTC_Shader::FreeShader(g_resources.flatShader);
		PTC_Texture::FreeTexture(g_resources.paletteTexture);
		g_resources.textRenderer.Free();
	}
}
