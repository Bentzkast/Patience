#include "main.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
// Hack scoping..
namespace Resources {
	static GameResources g_resources = { 0 };
	void InitResources()
	{
		if(!Shader::LoadFinalShader(
			"asset/shader/vertex_basic.vert",
			"asset/shader/frag_basic.frag",g_resources.flatShader))
			{
				SDL_Log("Failed to load flat Shader");
			}

		if(!Shader::LoadFinalShader(
			"asset/shader/text.vert",
			"asset/shader/text.frag",g_resources.textShader))
			{
				SDL_Log("Failed to load text Shader");
			}


		if(!Texture::CreateTexture(
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
		g_resources.quadMesh = Mesh::CreateMesh3D(quadVertices, 4 * 8, quadIndices, 6);
		if(!Mesh::LoadMeshFromObjFile("asset/mesh/CubeSplit.obj", g_resources.cubeMesh))
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
			g_resources.paletteTexture.Select();
			g_resources.cubeMesh.Select();
			g_resources.cubeMesh.Draw();
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
		g_resources.quadMesh.Free();
		g_resources.cubeMesh.Free();
		g_resources.flatShader.Free();
		g_resources.paletteTexture.Free();
		g_resources.textRenderer.Free();
	}
}
