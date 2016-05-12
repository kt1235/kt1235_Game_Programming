#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"

class SheetSprite {
public:
	SheetSprite(GLuint textureID, int spriteCountX, int spriteCountY);

	void Draw(ShaderProgram *program);

	int index;
	GLuint textureID;
	int spriteCountX, spriteCountY;
};