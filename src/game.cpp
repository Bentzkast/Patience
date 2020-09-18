#include "main.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

void Game::Init()
{
	this->resources.InitResources();
	this->uiRenderer.Initialize(1080, 720);
	this->gameInputs.reserve(20);
	this->interactable2D.reserve(30);
	this->isRunning = true;


	// temporary stuff
	SDL_Rect rect{ 10, 10, 350, 50};
	this->interactable2D.emplace_back(
		rect, "Test", GameAction::TEST,
		glm::vec3 {0.25f, 0.25f, 0.25f},
		glm::vec3{ 0.3, 0.7 , 0.9 });
}

void Game::Free()
{
	this->uiRenderer.Free();
	this->resources.FreeResources();
}

static void PollEvent(
	const std::vector<Interactable2D>& interactable2D,
	std::vector<GameInput>& outGameInput)
{
	SDL_Event event = {0};
	outGameInput.clear();
	bool isMouseIdle = true;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			{
				outGameInput.emplace_back(GameAction::HARD_EXIT);
			} break;
			case SDLK_F1:
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} break;
			case SDLK_F2:
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			} break;
			case SDLK_w:
			{
				outGameInput.emplace_back(GameAction::SCROLL_FORWARD);
			} break;
			case SDLK_s:
			{	
				outGameInput.emplace_back(GameAction::SCROLL_BACK);
			} break;
			case SDLK_a:
			{
				outGameInput.emplace_back(GameAction::SCROLL_LEFT);
			} break;
			case SDLK_d:
			{
				outGameInput.emplace_back(GameAction::SCROLL_RIGHT);
			} break;
			default:
				break;
			}
		} break;
		case SDL_MOUSEBUTTONDOWN:
		{
			isMouseIdle = false;
			if(event.button.button == SDL_BUTTON_LEFT)
			{
				// if no overlap with any UI player is interacting with world,
				GameAction action = GameAction::WORLD_SELECT;
				// Select for overlap button
				SDL_Point mousePoint { event.button.x, event.button.y };
				// maybe has priotity layer latter.. now the latestone..
				for (size_t i = 0; i < interactable2D.size(); i++)
				{
					if(SDL_PointInRect(&mousePoint, &interactable2D[i].rect))
					{
						action = interactable2D[i].action;
					}
				}

				outGameInput.emplace_back(action, event.button.x, event.button.y);
			}
			if(event.button.button == SDL_BUTTON_RIGHT)
			{
				SDL_Log("Right Click");
			}
		}break;
		case SDL_MOUSEMOTION:
		{
			isMouseIdle = false;
		} break;
		case SDL_QUIT: // X top right
		{
			outGameInput.emplace_back(GameAction::HARD_EXIT, 0, 0);
		}
		break;
		default:
			break;
		}
	}
	// TODO add idle timer??
	if(isMouseIdle)
	{
		// Hover action
		int x, y;
		SDL_GetMouseState(&x, &y);
		// LATER implement other game play element and see how to pass stuff
		// after we hone down the 3d world interaction
		outGameInput.emplace_back(GameAction::FOCUS_ON, x, y);
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
		PollEvent(this->interactable2D, this->gameInputs);

		// Exit if there're a hard exit request
		for (size_t i = 0; i < this->gameInputs.size(); i++)
		{
			if(this->gameInputs[i].action == GameAction::HARD_EXIT)
			{
				this->isRunning = false;
				break;
			}
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
		int mouseX = 0;
		int mouseY = 0;
		bool select = false;
		for (size_t i = 0; i < this->gameInputs.size(); i++)
		{
			switch (this->gameInputs[i].action)
			{
			case GameAction::TEST:
			{
				SDL_Log("TEST BUTTON SELECTED");
			} break;
			case GameAction::SCROLL_FORWARD:
			{
				cameraRigPos.y -= 1;
			} break;
			case GameAction::SCROLL_BACK:
			{
				cameraRigPos.y += 1;
			} break;
			case GameAction::SCROLL_RIGHT:
			{
				cameraRigPos.x += 1;
			} break;
			case GameAction::SCROLL_LEFT:
			{
				cameraRigPos.x -= 1;
			} break;
			case GameAction::WORLD_SELECT:
			{
				mouseX = this->gameInputs[i].x;
				mouseY = this->gameInputs[i].y;
				SDL_Log("%d %d", mouseX, mouseY);
				select = true;
			} break;		
			default:
				break;
			}
		}

		// RENDER----------------------------
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Enable depth buffering/disable alpha blend
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glDepthFunc(GL_LESS);

		// Draw World
		float width = 1080.0f;
		float height = 720.0f;

		glm::vec3 cameraPos = glm::vec3(cameraRigPos.x, 10.0f, cameraRigPos.y + 10.f);
		glm::vec3 cameraTarget = glm::vec3(cameraRigPos.x, 0.0f, cameraRigPos.y);
		glm::vec3 vectorToCamera = glm::normalize(cameraPos - cameraTarget);
		glm::vec3 UP = glm::vec3(0.0f, 1.0, 0.0f);
		glm::vec3 cameraRight = glm::normalize(glm::cross(UP, vectorToCamera));
		glm::vec3 cameraUp = glm::cross(vectorToCamera, cameraRight);

		glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 200.0f);

		if(select) // maybe slower interfall ??? idk
		{
			float x = (2.0f * mouseX) / width - 1.0f;
			float y = 1.0f - (2.0f * mouseY) / height;
			
			glm::vec4 ray = {x, y, -1.0f, 1.0f};
			ray = glm::inverse(projection) * ray;
			ray.z = -1;
			ray.w = 0.0;
			SDL_Log("Ray eye : %s",glm::to_string(ray).c_str());

			glm::vec3 finalRay = glm::inverse(view) * ray;
			finalRay = glm::normalize(finalRay);
			SDL_Log("Ray vec : %s",glm::to_string(finalRay).c_str());
			// we use up as the ground place normal
			float intersectionRange = -glm::dot(cameraPos,UP)/ glm::dot(finalRay, UP);

			intersectionPoint = cameraPos + finalRay * intersectionRange;
			intersectionPoint = glm::floor(intersectionPoint);
			SDL_Log("IntersectionPoint : %s",glm::to_string(intersectionPoint).c_str());
		}

		resources.DrawMesh(deltaTime, projection * view, intersectionPoint);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// DRAW Buttons
		for(auto& ui : this->interactable2D)
		{
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
		
		SDL_GL_SwapWindow(appState.sdlWindow);
		realTickElapsed = SDL_GetTicks();
	}
}