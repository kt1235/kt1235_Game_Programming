#include "Matrix.h"
#include "ShaderProgram.h"
class Entity
{
public:
	Entity();
	Entity(float x, float y, float scale, float velocity_x, float velocity_y);
	float x = 0;
	float y = 0;
	float width = 0;
	float height = 0;
	float direction_x = 0;
	float direction_y = 0;
	float acceleration_x = 0;
	float acceleration_y = 0;
	float friction_x = 0;
	float friction_y = 0;
	int textureID;
	Matrix matrix;

	void draw(ShaderProgram *program,int index, int spriteCountX, int spriteCountY);
	void resetCollisionFlags();
	bool checkCollision(Entity entity);
	void handleCollisions(Entity entity);
};