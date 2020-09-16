#include "main.h"
#include <glad/glad.h>

static GameState g_gameState = {0};
static ApplicationState g_applicationState = {0};

void MovePlayer(Player *player)
{
	SDL_Log("Player Moved to new location");
	player->x += 1;
}

PlayerActionTable playerActionTable[] =
	{
		{"Move", MovePlayer}
	};

static int ShutDown()
{
	SDL_GL_DeleteContext(g_applicationState.sdlGLContext);
	SDL_DestroyWindow(g_applicationState.sdlWindow);
	SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	SDL_Quit();
	return 0;
}

static void PollEvent()
{
	SDL_Event event = {0};
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				g_applicationState.isRunning = false;
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
		break;
		case SDL_QUIT: // X top right
		{
			g_applicationState.isRunning = false;
		}
		break;
		default:
			break;
		}
	}
}

static void SetOpenGlAttributes()
{
	// Context Profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Versions
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Color Buffer
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	// Enable double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Force OpenGL to use hardware acceleration
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
}

int main(int argc, char *argv[])
{
	// Setup Window Context
	// Load from file later
	g_applicationState.windowWidth = 1080;
	g_applicationState.windowHeight = 720;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, " Tea::CreateContext::SDL_Init : %s.", SDL_GetError());
		return ShutDown();
	}

	SetOpenGlAttributes();

	// Init Window
	{
		g_applicationState.sdlWindow = SDL_CreateWindow(
			"Tea Engine", // Window title
			100,		  // Top left x-coordinate of window
			100,		  // Top left y-coordinate of window
			g_applicationState.windowWidth,
			g_applicationState.windowHeight,
			SDL_WINDOW_OPENGL // Flags (0 for no flags set)
		);

		if (!g_applicationState.sdlWindow)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Tea::CreateContext::SDL_CreateWindow : %s.", SDL_GetError());
			return ShutDown();
		}
	}

	{
		g_applicationState.sdlGLContext = SDL_GL_CreateContext(g_applicationState.sdlWindow);
		if (g_applicationState.sdlGLContext == NULL)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext : %s.", SDL_GetError());
			return ShutDown();
		}

		// Init GLAD
		if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "V_gladLoadGLLoader : %d.", __LINE__);
			return ShutDown();
		}
		SDL_Log("Vendor:   %s\n", glGetString(GL_VENDOR));
		SDL_Log("Renderer: %s\n", glGetString(GL_RENDERER));
		SDL_Log("Version:  %s\n", glGetString(GL_VERSION));
		// VSYNC
		SDL_GL_SetSwapInterval(1);
	}

	// Game State
	g_gameState.resources = new Resources();
	g_gameState.resources->InitResources();

	// Game Runs!
	u32 realTickElapsed = SDL_GetTicks();
	constexpr float deltaTimeLimit = 0.05f;
	g_applicationState.isRunning = true;

	glEnable(GL_MULTISAMPLE); // AA?
	glEnable(GL_CULL_FACE); // Face culling
	while (g_applicationState.isRunning)
	{
		PollEvent();

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

		// Tick here

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Enable depth buffering/disable alpha blend
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glDepthFunc(GL_LESS);

		g_gameState.resources->DrawMesh(deltaTime);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		g_gameState.resources->DrawText();

		SDL_GL_SwapWindow(g_applicationState.sdlWindow);
		realTickElapsed = SDL_GetTicks();
	}

	g_gameState.resources->FreeResources();
	delete g_gameState.resources;

	return ShutDown();
}