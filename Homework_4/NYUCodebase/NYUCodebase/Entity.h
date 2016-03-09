#ifndef NYUCodebase_Entity_h
#define NYUCodebase_Entity_h

class Entity {
public:
	Entity();
	Entity(int texture, float posX, float posY, float rot, float w, float h);
	~Entity();

	void Draw();
	float x = 0.0;
	float y = 0.0;
	float rotation = 0.0f;
	float scale = 1.0f;

	GLuint textureID;

	float width = 1.0f;
	float height = 1.0f;
	float speed = 0.9f;
	float direction_x;
	float direction_y;
};

#endif;