#include "Matrix.h"
#include "ShaderProgram.h"
class Entity
{
public:
	Entity();
	Entity(float x, float y, float scale, float velocity_x, float velocity_y);
	float x = 0;
	float y = 0;
	float width = 1.0f;
	float height = 1.0f;
	float direction_x = 0;
	float direction_y = 0;
	float acceleration_x = 0;
	float acceleration_y = 0;
	float friction_x = 0;
	float friction_y = 0;
	float scale = 1.0f;
	int textureID;
	float u = 0.0f;
	float v = 0.0f;
	Matrix matrix;

	void draw(ShaderProgram *program,int index, int spriteCountX, int spriteCountY, float vertices[]);
	void resetCollisionFlags();
	bool checkCollision(Entity entity);
	void handleCollisions(Entity entity);
};