#include "resources.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

void Mesh::Select() const
{
	glBindVertexArray(this->vertexArrayHandle);
}

void Mesh::Draw() const
{
	glDrawElements(GL_TRIANGLES, this->elementBufferCount, GL_UNSIGNED_INT, 0);
}

void Mesh::Free() const
{
	SDL_Log("Destroying Mesh");
	glDeleteBuffers(1, &this->vertexBufferHandle);
	glDeleteBuffers(1, &this->elementBufferHandle);
	glDeleteBuffers(1, &this->vertexArrayHandle);
}

Mesh Mesh::CreateSimple2D(
	const float vertices[], const u32 vertCount,
	const u32 indices[], const u32 indicesCount)
{
	u32 vertexArrayHandle { 0 };
	u32 vertexBufferHandle { 0 };
	u32 elementBufferHandle { 0 };

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

	const int totalStrideBytes = 4 * sizeof(float); // Stride
	// position&texture v4 since 2d
	{
		constexpr int posAttrIndex = 0;
		constexpr int posDataCount = 4;
		constexpr int posAttrOffsetBytes = 0;
		glVertexAttribPointer(posAttrIndex, posDataCount, GL_FLOAT, GL_FALSE, totalStrideBytes, (void*)posAttrOffsetBytes);
		glEnableVertexAttribArray(posAttrIndex);
	}

	// Unbind
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
	return Mesh 
	{
		vertexArrayHandle,
		vertexBufferHandle,
		elementBufferHandle,
		vertCount,
		indicesCount
	};
}

Mesh Mesh::CreateMesh3D(
	const float vertices[], const u32 vertCount,
	const u32 indices[], const u32 indicesCount)
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

	// Unbind
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

	return Mesh 
	{
		vertexArrayHandle,
		vertexBufferHandle,
		elementBufferHandle,
		vertCount,
		indicesCount
	};
}

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

bool Mesh::LoadMeshFromObjFile(const char* filePath, Mesh& outMesh)
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

	outMesh = CreateMesh3D(glVert.data(), glVert.size(), glIndices.data(), glIndices.size());
	return true;
}