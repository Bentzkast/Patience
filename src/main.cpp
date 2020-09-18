#include "main.h"
#include <glad/glad.h>

static Game* g_game = nullptr;
static ApplicationState g_applicationState = {0};

static int ShutDown()
{
	SDL_GL_DeleteContext(g_applicationState.sdlGLContext);
	SDL_DestroyWindow(g_applicationState.sdlWindow);
	SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	SDL_Quit();
	return 0;
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
			"Patience Engine", // Window title
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
	g_game = new Game();
	g_game->Init(); // Check return value ?
	g_game->Run(g_applicationState);
	g_game->Free();
	delete g_game;

	return ShutDown();
}