#include "resources.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

void Resources::InitResources()
{
	if(!Shader::LoadFinalShader(
		"asset/shader/vertex_basic.vert",
		"asset/shader/frag_basic.frag",this->flatShader))
		{
			SDL_Log("Failed to load flat Shader");
		}

	if(!Shader::LoadFinalShader(
		"asset/shader/text.vert",
		"asset/shader/text.frag",this->textShader))
		{
			SDL_Log("Failed to load text Shader");
		}

	if(!Texture::CreateTexture(
		"asset/texture/Simple.png", this->paletteTexture
	))
	{
		SDL_Log("Failed to load Simple Pallete Texture");
	}

	if(!Texture::CreateTexture(
		"asset/texture/Panel.png", this->panelTexture
	))
	{
		SDL_Log("Failed to load Panel Texture");
	}

	if(!Mesh::LoadMeshFromObjFile("asset/mesh/Quad.obj", this->quadMesh))
	{
		SDL_Log("Failed to load cube mesh");
	}
	if(!Mesh::LoadMeshFromObjFile("asset/mesh/Tree.obj", this->treeMesh))
	{
		SDL_Log("Failed to load tree mesh");
	}

	if(!Mesh::LoadMeshFromObjFile("asset/mesh/House.obj", this->houseMesh))
	{
		SDL_Log("Failed to load house mesh");
	}
		if(!Mesh::LoadMeshFromObjFile("asset/mesh/Selection.obj", this->selectionMesh))
	{
		SDL_Log("Failed to load selection mesh");
	}
}

// MOVE to dedicated renderer
void Resources::DrawMesh(float dt, const glm::mat4& projectionTimesView, const glm::vec3& worldCursorPos)
{
	// World
	for (int y = 0; y < 5; y++)
	{
		for (int x = 0; x < 5; x++)
		{
			
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(x * 2, 0.0f, y * 2));
			glm::mat4 final = projectionTimesView * transform;
			this->flatShader.Select();
			this->flatShader.SetMatrixUniform("transform", final);
			this->paletteTexture.Select();
			this->quadMesh.Select();
			this->quadMesh.Draw();
		}
	}

	// Selection
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(worldCursorPos.x, 0.01f, worldCursorPos.z));
		glm::mat4 final = projectionTimesView * transform;
		this->flatShader.Select();
		this->flatShader.SetMatrixUniform("transform", final);
		this->paletteTexture.Select();
		this->selectionMesh.Select();
		this->selectionMesh.Draw();
	}
	
	// Tree
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		glm::mat4 final = projectionTimesView * transform;
		this->flatShader.Select();
		this->flatShader.SetMatrixUniform("transform", final);
		this->paletteTexture.Select();
		this->treeMesh.Select();
		this->treeMesh.Draw();
	}
}

void Resources::FreeResources()
{
	this->quadMesh.Free();
	this->treeMesh.Free();
	this->houseMesh.Free();
	this->selectionMesh.Free();

	this->flatShader.Free();
	this->flatShader.Free();

	this->paletteTexture.Free();
	this->panelTexture.Free();
}

