#pragma once
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

/*	The SheetSprite class is used to manage textures for objects.
*/
class DrawSpriteSheetSprite {
public:
	DrawSpriteSheetSprite();
	DrawSpriteSheetSprite(GLuint textureID, float u, float v, float width, float height);

	void Draw(float x, float y, float scale, float rotation);

	GLuint textureID;

	float u;
	float v;
	float width;
	float height;
};
