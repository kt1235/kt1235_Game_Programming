#include "Entity.h"

Entity::Entity(SheetSprite sprite)
	: sprite(sprite), x(0), y(0), width(0), height(0),
	velocity_x(0), velocity_y(0), acceleration_x(0), acceleration_y(0),
	collidedTop(false), collidedBottom(false), collidedLeft(false), collidedRight(false) {
}
void Entity::Draw(ShaderProgram *program) {
	Matrix modelMatrix = Matrix();
	modelMatrix.Translate(x, y, 0);

	program->setModelMatrix(modelMatrix);

	float vertices[] = {
		-width / 2, -height / 2,
		width / 2, height / 2,
		-width / 2, height / 2,
		width / 2, height / 2,
		-width / 2, -height / 2,
		width / 2, -height / 2
	};
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	sprite.Draw(program);

	glDisableVertexAttribArray(program->positionAttribute);
}

float lerp(float v0, float v1, float t) {
	return (1.0f - t) * v0 + t * v1;
}



void Entity::Update(float elapsed) {
	collidedTop = false;
	collidedBottom = false;
	collidedLeft = false;
	collidedRight = false;

	velocity_x = lerp(velocity_x, 0.0f, elapsed * 2.8f);	//friction_x
	velocity_x += acceleration_x * elapsed;
	velocity_y = lerp(velocity_y, 0.0f, elapsed * 0.4f);	//friction_y
	velocity_y += acceleration_y * elapsed;
	velocity_y += -3.0f * elapsed;
}


bool CollidesWith(Entity *entity) {
	return true;
}

bool Entity::checkCollision(Entity *entity) {
	float bot = y - (height * 0.5f);
	float top = y + (height * 0.5f);
	float left = x - (width + height * 0.5f);
	float right = x + (width + height * 0.5f);

	float otherBot = entity->y - entity->height * 0.5f;
	float otherTop = entity->y + entity->height * 0.5f;
	float otherLeft = entity->x - entity->width * 0.5f;
	float otherRight = entity->x + entity->width * 0.5f;

	if (bot > otherTop) return false;
	else if (top < otherBot) return false;
	else if (left > otherRight - .39f) return false;
	else if (right < otherLeft + .39f) return false;

	return true;

}

void Entity::handleCollisions(Entity* entity) {
	while (!invincibility) {
		if (fabs(x - entity->x) < (width + entity->width) / 2.5f)
		{
			velocity_y = 0.0f;
			if (y > entity->y) {
				float YPenetration = fabs((y - height / 2) - (entity->y + entity->height / 2));
				y += YPenetration + .001f;
				velocity_y += 0.25f;
			}
			if (y < entity->y) {
				float YPenetration = fabs((y + height / 2) - (entity->y - entity->height / 2));
				y -= (YPenetration + .001f);
				velocity_y -= 0.25f;
			}
		}


		if (fabs(y - entity->y) < (height + entity->height) / 2.5f) {
			velocity_y = 0.0f;
			if (x > entity->x) {
				float XPenetration = fabs((x - width / 2) - (entity->x + entity->width / 2));
				x += XPenetration + .001f;
				velocity_x += 0.25f;
			}
			if (x < entity->x) {
				float XPenetration = fabs((x + width / 2) - (entity->x - entity->width / 2));
				x -= (XPenetration + .001f);
				velocity_x -= 0.25f;
			}
		}
	}
}

void Entity::enableInvincibility() {
	bool collidedTop, collidedBottom, collidedLeft, collidedRight = false;
}