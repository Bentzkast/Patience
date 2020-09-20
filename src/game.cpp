#include "main.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

void Game::Init()
{
	this->resources.InitResources();
	this->uiRenderer.Initialize(1080, 720);
	this->inputSystem.Init();
	this->isRunning = true;


	// temporary stuff
	interactableUI[currentInteractableSize].rect = SDL_Rect { 10, 10, 350, 50};
	interactableUI[currentInteractableSize].name = "Test Button";
	interactableUI[currentInteractableSize].backgroundColor = {.3, .3, .3};
	interactableUI[currentInteractableSize++].foregroundColor = {0.3f , 0.7f , 0.9f };
}

void Game::Free()
{
	this->uiRenderer.Free();
	this->resources.FreeResources();
}

static void PollEvent(
	const Interactable2D* ,
	InputSystem& inputSystem)
{
	SDL_Event event = {0};
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_F1:
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} break;
			case SDLK_F2:
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			} break;
			default:
				break;
			}
		} break;
		case SDL_QUIT: // X top right
		{
			inputSystem.SetForceQuit();
		}
		break;
		default:
			break;
		}
	}
}

// Game Runs! yehaww
void Game::Run(const ApplicationState& appState)
{
	u32 realTickElapsed = SDL_GetTicks();
	constexpr float deltaTimeLimit = 0.05f;

	glEnable(GL_MULTISAMPLE); // AA?
	glEnable(GL_CULL_FACE); // Face culling
	while (this->isRunning)
	{
		PollEvent(this->interactableUI, inputSystem);

		if(inputSystem.IsReleased(Exit))
		{
			this->isRunning =false;
		}
		
		while (!SDL_TICKS_PASSED(SDL_GetTicks(), realTickElapsed + 16))
		{
			if (!SDL_TICKS_PASSED(SDL_GetTicks(), realTickElapsed + 17))
			{
				SDL_Delay(1);
			}
		}
		// Frame Upper Cap 60
		float deltaTime = (SDL_GetTicks() - realTickElapsed) * 0.001f;
		if (deltaTime > deltaTimeLimit)
		{
			deltaTime = deltaTimeLimit;
		}

		// Update Here --------------------------
		// 3d world mouse
		// Ray cast Object


		// Mouse
		bool isOverUI = false;
		for (int i = 0; i < currentInteractableSize; i++)
		{
			if(SDL_PointInRect(&this->inputSystem.GetMousePos(),&interactableUI[i].rect)){
				SDL_Log("Is over %s", interactableUI[i].name.c_str());
				isOverUI = true;
			}
		}
		
		SDL_Point mousePos;
		bool select = false;

		if(inputSystem.IsPressed(LEFT_MOUSE) && !isOverUI){
			SDL_Log("World Raycast!");
			select = true;
			mousePos = this->inputSystem.GetMousePos();
		}
		// if(inputSystem.IsPressed(RIGHT_MOUSE)){
		// 	SDL_Log("RIGHT CLICK");
		// }

		glm::ivec2 cameraDir{ 0, 0};
		int rotDelta = 0;
		if(inputSystem.IsHold(Forward))
		{
			cameraDir.y = -1;
		}
		if(inputSystem.IsHold(Backward))
		{
			cameraDir.y = 1;
		}
		if(inputSystem.IsHold(Left))
		{
			cameraDir.x = -1;
		}
		if(inputSystem.IsHold(Right))
		{
			cameraDir.x = 1;
		}
		if(inputSystem.IsHold(RotateLeft))
		{
			rotDelta = -1;
		}
		else if(inputSystem.IsHold(RotateRight))
		{
			rotDelta = 1;
		}

		// Late Update Here ---------------------------------
		float width = 1080.0f;
		float height = 720.0f;
		camera.LateUpdate(width, height, cameraDir, rotDelta, deltaTime);
		inputSystem.LateUpdate();

		// RENDER----------------------------
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Enable depth buffering/disable alpha blend
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glDepthFunc(GL_LESS);

		// Draw World


		if(select) // maybe slower interfall ??? idk
		{
			float x = (2.0f * mousePos.x) / width - 1.0f;
			float y = 1.0f - (2.0f * mousePos.y) / height;
			
			glm::vec4 ray = {x, y, -1.0f, 1.0f};
			ray = glm::inverse(camera.projection) * ray;
			ray.z = -1;
			ray.w = 0.0;
			SDL_Log("Ray eye : %s",glm::to_string(ray).c_str());

			glm::vec3 finalRay = glm::inverse(this->camera.view) * ray;
			finalRay = glm::normalize(finalRay);
			SDL_Log("Ray vec : %s",glm::to_string(finalRay).c_str());
			// we use up as the ground place normal
			float intersectionRange = 
				-glm::dot(this->camera.GetCameraPos(),this->camera.UP) / glm::dot(finalRay, this->camera.UP);

			intersectionPoint = this->camera.GetCameraPos() + finalRay * intersectionRange;
			intersectionPoint = glm::floor(intersectionPoint);
			SDL_Log("IntersectionPoint : %s",glm::to_string(intersectionPoint).c_str());
		}

		resources.DrawMesh(deltaTime, this->camera.GetProjectionView(), intersectionPoint);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// DRAW Buttons
		for (int i = 0; i < currentInteractableSize; i++)
		{
			auto ui = interactableUI[i];
			this->uiRenderer.RenderPanel(
				this->resources,
				ui.rect, ui.backgroundColor
			);

			// How big the text? remove scale ?? dont need it :|
			this->uiRenderer.RenderText(
				this->resources,
				ui.name,
				ui.rect.x, ui.rect.y, 1.0f,
				ui.foregroundColor);
		}
		
		char buff[20] {0};
		snprintf(buff, sizeof(buff), "fps %.2f", 1 / deltaTime);
		std::string buffAsStdStr = buff;	

		this->uiRenderer.RenderText(
		this->resources,
		buffAsStdStr,
		60, 60, 1.0f,
		glm::vec3{ 0.3, 0.7 , 0.9 });
		
		SDL_GL_SwapWindow(appState.sdlWindow);
		realTickElapsed = SDL_GetTicks();
	}
}