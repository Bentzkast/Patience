#include "main.h"

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

	if(!Mesh::LoadMeshFromObjFile("asset/mesh/CubeSplit.obj", this->cubeMesh))
	{
		SDL_Log("Failed to load cube mesh");
	}
	if(!Mesh::LoadMeshFromObjFile("asset/mesh/Tree.obj", this->treeMesh))
	{
		SDL_Log("Failed to load tree mesh");
	}

	this->textRenderer.Initialize(1080, 720);
}

void Resources::DrawMesh(float dt, const Mouse& mouse)
{
	float width = 1080.0f;
	float height = 720.0f;

	glm::vec3 cameraPos = glm::vec3(0.0f, 9.0f, 1.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 vectorToCamera = glm::normalize(cameraPos - cameraTarget);
	glm::vec3 UP = glm::vec3(0.0f, 1.0, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(UP, vectorToCamera));
	glm::vec3 cameraUp = glm::cross(vectorToCamera, cameraRight);

	glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 200.0f);

	// WOHOO
	if(mouse.click)
	{
		float x = (2.0f * mouse.x) / width - 1.0f;
		float y = 1.0f - (2.0f * mouse.y) / height;
		
		glm::vec4 ray = {x, y, -1.0f, 1.0f};
		ray = glm::inverse(projection) * ray;
		ray.z = -1;
		ray.w = 0.0;
		SDL_Log("Ray eye : %s",glm::to_string(ray).c_str());

		glm::vec3 finalRay = glm::inverse(view) * ray;
		finalRay = glm::normalize(finalRay);
		SDL_Log("Ray vec : %s",glm::to_string(finalRay).c_str());
		// we use up as the ground place normal
		float terrainHeight = 0;
		float intersectionRange = -glm::dot(cameraPos,UP) + terrainHeight/ glm::dot(finalRay, UP);

		glm::vec3 intersectionPoint = cameraPos + finalRay * intersectionRange;
		SDL_Log("IntersectionPoint : %s",glm::to_string(intersectionPoint).c_str());
	}

	// World
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		glm::mat4 final = projection * view * transform;
		this->flatShader.Select();
		this->flatShader.SetMatrixUniform("transform", final);
		this->paletteTexture.Select();
		this->cubeMesh.Select();
		this->cubeMesh.Draw();
	}
	// Tree
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 final = projection * view * transform;
		this->flatShader.Select();
		this->flatShader.SetMatrixUniform("transform", final);
		this->paletteTexture.Select();
		this->treeMesh.Select();
		this->treeMesh.Draw();
	}
}

void Resources::DrawText(const Mouse& mouse)
{
	SDL_Rect rect{ 10, 10, 350, 50};
	if(mouse.click)
	{
		SDL_Point mousePos{ mouse.x, mouse. y};
		if(SDL_PointInRect(&mousePos, &rect))
		{
			SDL_Log("Click over!!");
		}
	}

	this->textRenderer.RenderPanel(
		this->textShader,
		this->panelTexture,
		rect,{0.25f, 0.25f, 0.25f}
	);

	std::string sampleText { "Patience Engine" };
	// How big the text?
	this->textRenderer.RenderText(
		this->textShader,
		sampleText,
		15.0f, 10.0f, 1.0f,
		{ 0.3, 0.7 , 0.9 });
}

void Resources::FreeResources()
{
	this->cubeMesh.Free();
	this->treeMesh.Free();

	this->flatShader.Free();
	this->flatShader.Free();

	this->paletteTexture.Free();
	this->panelTexture.Free();
	
	this->textRenderer.Free();
}

