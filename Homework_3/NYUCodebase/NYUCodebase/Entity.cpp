#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Entity.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
Entity::Entity(){};

Entity::Entity(int texture, float posX, float posY, float rot, float w, float h) {
	textureID = texture;
	x = posX;
	y = posY;
	rotation = rot;
	width = w;
	height = h;
	direction_x = direction_x;
	direction_y = direction_y;
	bool visible = true;
}

Entity::~Entity() { }

void Entity::Draw(){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x, y, 0.0);
	glRotatef(rotation, 0.0, 0.0, 0.0);
	glScalef(scale, scale, scale);

	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
	glVertexPointer(2, GL_FLOAT, 0, texCoords);
	glEnableClientState(GL_VERTEX_ARRAY);
	GLfloat UVs[] = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
	glTexCoordPointer(2, GL_FLOAT, 0, UVs);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_TEXTURE_2D);
}