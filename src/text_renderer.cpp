#include <ft2build.h>
#include FT_FREETYPE_H
#include "main.h"
#include <glad/glad.h>
#include "../external/glm/gtc/matrix_transform.hpp"

bool TextRenderer::Initialize()
{
	FT_Library ft;
	if(FT_Init_FreeType(&ft))
	{
		SDL_Log("Failed to init FreeType Library");
		return false;
	}

	FT_Face face;
	if(FT_New_Face(ft, "asset/font/arial.ttf", 0, &face))
	{
		SDL_Log("Failed to load Font");
		return false;
	}

	constexpr i32 dynamicWidth { 0 };
	constexpr i32 height { 48 };
	FT_Set_Pixel_Sizes( face , dynamicWidth , height );

	// DISABLE byte alightment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

	for (unsigned char c = 0; c < characterCount; c++)
	{
		if(FT_Load_Char( face , c , FT_LOAD_RENDER))
		{
			SDL_Log("Failed to load Glyph %c", c);
			continue;
		}

		glm::ivec2 size {face->glyph->bitmap.width, face->glyph->bitmap.rows};
		glm::ivec2 offset {face->glyph->bitmap_left, face->glyph->bitmap_top};
		u32 textureHandle;
		glGenTextures(1, & textureHandle);
		glBindTexture(GL_TEXTURE_2D, textureHandle);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			size.x,
			size.y,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		characterTable[c].textureHandle = textureHandle;
		characterTable[c].size = size;
		characterTable[c].bearing = offset;
		characterTable[c].advance = face->glyph->advance.x;
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	

	this->projection = glm::ortho(0.0f, 1080.0f, 0.0f, 720.0f);
	
	// Create Quad model left origin upside down origin
	float quadVertices[] = {
		// Pos		  // texture mapping
		0.0f,  	1.0f, 	0.0f, 	0.0f, // top left
		1.0f,  	1.0f,  	1.0f, 	0.0f, // top right
		1.0f, 	0.0f,  1.0f, 	1.0f, // bottom right
		0.0f, 	0.0f, 	0.0f, 	1.0f  // bottom left
	};
	Uint32 quadIndices[] = {
		0, 1, 2,
		2, 3, 0,
	};
	
	// Create
	glGenVertexArrays(1, &vertexArrayHandle);
	glGenBuffers(1, &vertexBufferHandle);
	glGenBuffers(1, &elementBufferHandle);

	// select buffer 
	glBindVertexArray(vertexArrayHandle);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferHandle);

	// set input data
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), quadVertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(Uint32), quadIndices, GL_STATIC_DRAW);

	const int totalStrideBytes = 4 * sizeof(float); // Stride
	// position&texture v4 since 2d
	{
		constexpr int posAttrIndex = 0;
		constexpr int posDataCount = 4;
		constexpr int posAttrOffsetBytes = 0;
		glVertexAttribPointer(posAttrIndex, posDataCount, GL_FLOAT, GL_FALSE, totalStrideBytes, (void*)posAttrOffsetBytes);
		glEnableVertexAttribArray(posAttrIndex);
	}
	
	// Unbind
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);
	return true;
}

void TextRenderer::RenderText(Shader& shader, std::string& text, float x, float y, float scale, glm::vec3 color)
{
	shader.Select();
	shader.Set3fUniform("textColor", color);
	glBindVertexArray(vertexArrayHandle);
	// glBindTexture(GL_TEXTURE_2D, 1);
	// glm::mat4 transform = glm::scale(glm::mat4{1.0f}, glm::vec3{100, 100, 1.0f});
	// shader.SetMatrixUniform("transform", this->projection * transform);
	// glDrawElements(GL_TRIANGLES, 6 , GL_UNSIGNED_INT, 0);
	for (auto c = text.begin(); c != text.end(); c++)
	{
		Character ch = characterTable[*c];

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;

		float xPos = x + (ch.bearing.x) * scale;
		float yPos = y - (ch.size.y - ch.bearing.y);   

		glm::mat4 transform = glm::scale(glm::mat4{1.0f}, glm::vec3{w, h, 1.0f});
		glm::mat4 translate = glm::translate(glm::mat4{1.0f}, glm::vec3{xPos , yPos, 0});
		//SDL_Log("printing %c %f %f , %f %f", *c , xPos, yPos, w, h);

        // // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureHandle);
        // // update content of VBO memory
        // glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

		shader.SetMatrixUniform("transform", this->projection * translate * transform);

		glDrawElements(GL_TRIANGLES, 6 , GL_UNSIGNED_INT, 0);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
}

void TextRenderer::Free()
{
	SDL_Log("Callled");
	// glDeleteBuffers(1, &this->vertexBufferHandle);
	// glDeleteBuffers(1, &this->vertexArrayHandle);
	// glDeleteBuffers(1, &this->elementBufferHandle);
}
