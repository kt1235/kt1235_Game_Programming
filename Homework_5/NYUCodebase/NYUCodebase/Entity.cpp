#include "Entity.h"

Entity::Entity(){}
Entity::Entity(float x, float y, float scale, float direction_x, float direction_y){}

void Entity::draw(ShaderProgram *program, int index, int spriteCountX, int spriteCountY)
{
	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0f / (float)spriteCountX;
	float spriteHeight = 1.0f / (float)spriteCountY;

	GLfloat texCoords[] = {  u, v + spriteHeight, u + spriteWidth, v + spriteHeight, u + spriteWidth, v,
		u, v + spriteHeight, u + spriteWidth, v, u, v  };
	float aspect = width / height;
	float vertices[] = {
		-0.5f * height * aspect, -0.5f * height,
		0.5f * height * aspect, 0.5f * height,
		-0.5f * height * aspect, 0.5f * height,
		0.5f * height * aspect, 0.5f * height,
		-0.5f * height * aspect, -0.5f * height,
		0.5f * height * aspect, -0.5f * height
	};

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

// check for collision. returns false if collision detected
bool Entity::checkCollision(Entity entity) {
	float bot = y - (height * 0.5f);
	float top = y + (height * 0.5f);
	float left = x - (width + height * 0.5f);
	float right = x + (width + height * 0.5f);

	float otherBot = entity.y - entity.height * 0.5f;
	float otherTop = entity.y + entity.height * 0.5f;
	float otherLeft = entity.x - entity.width * 0.5f;
	float otherRight = entity.x + entity.width * 0.5f;

	if (bot > otherTop) return false;
	else if (top < otherBot) return false;
	else if (left > otherRight - 0.39f) return false;
	else if (right < otherLeft + 0.39f) return false;

	return true;

}

void Entity::handleCollisions(Entity entity) {
	if (fabs(x - entity.x) < (width + entity.width) / 2.5f)
	{
		direction_y = 0.0f;
		if (y > entity.y) {
			float YPenetration = fabs((y - height / 2) - (entity.y + entity.height / 2));
			y += YPenetration + .001f;
			matrix.Translate(0, YPenetration + .001f, 0);
		}
		if (y < entity.y) {
			float YPenetration = fabs((y + height / 2) - (entity.y - entity.height / 2));
			y -= (YPenetration + .001f);
			matrix.Translate(0, -YPenetration - .001f, 0);
		}
	}

	if (fabs(y - entity.y) < (height + entity.height) / 2.5f) {
		direction_y = 0.0f;
		if (x > entity.x) {
			float XPenetration = fabs((x - width / 2) - (entity.x + entity.width / 2));
			x += XPenetration + .001f;
			matrix.Translate(XPenetration + .0001f, 0, 0);
		}
		if (x < entity.x) {
			float XPenetration = fabs((x + width / 2) - (entity.x - entity.width / 2));
			x -= (XPenetration + .001f);
			matrix.Translate(-XPenetration - .0001f, 0, 0);
		}
	}
}