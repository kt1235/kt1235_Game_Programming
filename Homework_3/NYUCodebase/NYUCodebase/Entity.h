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
	float rotation = 0;
	float scale = 0.25f;
	//float cool_down = 0;
	GLuint textureID;

	float width = 1.0f;
	float height = 1.0f;
	float speed = 2.0f;
	float direction_x;
	float direction_y;
};

#endif;