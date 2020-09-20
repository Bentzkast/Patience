#include "resources.h"
#include <stb_image.h>

void Texture::Free() const
{
	glDeleteTextures(1, &this->handle);
}

void Texture::Select() const
{
	glBindTexture(GL_TEXTURE_2D, this->handle);
}

bool Texture::CreateTexture(const char* texturePath, Texture& outTexture)
{
	Uint32 glTextureHandle { 0 };
	int width { 0 };
	int height { 0 };
	stbi_set_flip_vertically_on_load(true);
	// Create glTexture
	glGenTextures(1, &glTextureHandle);
	glBindTexture(GL_TEXTURE_2D, glTextureHandle);
	// glTexture settings
	// texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load image 
	int  nrChannels;
	Uint8 * imageData = stbi_load(texturePath, &width, &height, &nrChannels, 0);
	if (!imageData)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to find image file : %s", texturePath);
		return false;
	}
	int format = GL_RGB;
	if (nrChannels == 4)
	{
		format = GL_RGBA;
	}
	// Push image to glTexture
	glTexImage2D(
		GL_TEXTURE_2D,				// Same as selected texture
		0,							// mipmap level, if we want to do it manually 0 base level
		format,	// Format to store (dest)
		width, height,				// dimension of the texture
		0,							// always zero, legacy stuff
		format,	// Format source
		GL_UNSIGNED_BYTE,			// Format source
		imageData);		// Actual image data
		// filter options, billeniar filtering

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(imageData);

	outTexture.handle = glTextureHandle;
	outTexture.width = width;
	outTexture.height = height;
	return true;
}