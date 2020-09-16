#include "main.h"

#include <glm/gtc/type_ptr.hpp>
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
		SDL_Log("Failed to load Texture");
	}

	float quadVertices[] = {
		// Pos		  // texture mapping
		0.0f,  	1.0f, 	0.0f, 	0.0f, // top left
		1.0f,  	1.0f,  	1.0f, 	0.0f, // top right
		1.0f, 	0.0f,   1.0f, 	1.0f, // bottom right
		0.0f, 	0.0f, 	0.0f, 	1.0f  // bottom left
	};

	Uint32 quadIndices[] = {
		2, 1, 0,
		0, 3, 2,
	};

	this->quadMesh = Mesh::CreateSimple2D(quadVertices, 16, quadIndices, 6);
	if(!Mesh::LoadMeshFromObjFile("asset/mesh/CubeSplit.obj", this->cubeMesh))
	{
		SDL_Log("Failed to load cube mesh");
	}

	this->rotation = 0;
	this->textRenderer.Initialize();
}

void Resources::DrawMesh(float dt)
{
	// World
	{
		this->rotation += dt * 10.0f;
		glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(this->rotation), glm::vec3(1.0f, 0.0f, 1.0f));
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -.5f, -25.0f));
		//view = glm::rotate(view, glm::radians(45.f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 projection = glm::perspective(glm::radians(60.0f), 1080.0f / 720.0f, 0.1f, 200.0f);
		glm::mat4 final = projection * view * transform;
		this->flatShader.Select();
		this->flatShader.SetMatrixUniform("transform", final);
		this->paletteTexture.Select();
		this->cubeMesh.Select();
		this->cubeMesh.Draw();
	}
	// UI
	{
		glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(1080.0f, 75.0f, 1.0f));
		//transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 50.0f, 1.0f)) * transform;
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
		glm::mat4 projectionScreen = glm::ortho(0.0f, 1080.0f, 0.0f, 720.0f, 0.1f, 100.0f);

		glm::mat4 screenFinal = projectionScreen * view * transform;

		this->textShader.Select();
		this->textShader.SetMatrixUniform("transform", screenFinal);
		this->textShader.Set3fUniform("textColor", glm::vec3(0.25f, 0.25f, 0.25f));
		this->paletteTexture.Select();
		this->quadMesh.Select();
		this->quadMesh.Draw();
	}
}

void Resources::DrawText()
{
	std::string sampleText { "Patience Engine" };
	// How big the text?
	this->textRenderer.RenderText(
		this->textShader,
		sampleText,
		25.0f, 25.0f, 1.0f,
		{ 0.3, 0.7 , 0.9 });
}

void Resources::FreeResources()
{
	this->quadMesh.Free();
	this->cubeMesh.Free();
	this->flatShader.Free();
	this->paletteTexture.Free();
	this->textRenderer.Free();
}

