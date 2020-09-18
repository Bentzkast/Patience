#include "main.h"

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
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				outGameInput.emplace_back(GameAction::HARD_EXIT, 0, 0);
			}
			if (event.key.keysym.sym == SDLK_F1)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			if (event.key.keysym.sym == SDLK_F2)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
		case SDL_MOUSEBUTTONDOWN:
		{
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
		}
		break;
		case SDL_QUIT: // X top right
		{
			outGameInput.emplace_back(GameAction::HARD_EXIT, 0, 0);
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
		for (size_t i = 0; i < this->gameInputs.size(); i++)
		{
			if(this->gameInputs[i].action == GameAction::TEST)
			{
				SDL_Log("TEST BUTTON SELECTED");
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
		resources.DrawMesh(deltaTime);

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