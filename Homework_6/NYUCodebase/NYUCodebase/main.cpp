#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "Entity.h"
#include <vector>
#define FIXED_TIMESTEP 0.0444444f
#define MAX_TIMESTEPS 1
using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#define FRICTION 1.250f
#define GRAVITY 1.0f
#define PENETRATION 0.0000000000001f


SDL_Window* displayWindow;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
SDL_Event event;
ShaderProgram* program;
Matrix projectionMatrix;
Matrix viewMatrix;
Matrix modelMatrix;
float lastFrameTicks = 0.0f;

bool done = false;
int mapWidth;
int mapHeight;
float TILE_SIZE = 0.5f;
unsigned char** levelData;
GLuint tileset;
Entity player;
Entity door;
Entity bg1;
vector<Entity> tiles;


GLuint LoadTexture(const char *image_path)
{
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);
	return textureID;
}



float lerp(float v0, float v1, float t) {
	return (1.0f - t)*v0 + t*v1;
}

void Setup() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	int screenWidth = 800;
	int screenHeight = 600;
	glViewport(0, 0, screenWidth, screenHeight);
	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glUseProgram(program->programID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float projectionWidth = 6.5f;
	float projectionHeight = 3.5f;
	projectionMatrix.setOrthoProjection(-projectionWidth / 2, projectionWidth / 2, -projectionHeight / 2, projectionHeight / 2, -1.0f, 1.0f);

	//GLuint spriteSheetTexture = LoadTexture("tiles_spritesheet.png");
	GLuint tileTexture = LoadTexture("iceBlock.png");
	//GLuint playerTexture = LoadTexture("alienBlue_square.png");
	GLuint playerTexture = LoadTexture("blockerMad.png");
	player.textureID = playerTexture;
	GLuint iceDoorTexture = LoadTexture("iglooDoor.png");
	door.textureID = iceDoorTexture;
	//GLuint bg1Text = LoadTexture("background1.jpg");
	//bg1.textureID = bg1Text;

	for (int i = 0; i < 91; i++) {
		Entity tile;
		tile.width = 0.2f;
		tile.height = 0.2f;
		if (i < 16){
			if (i == 0) {
				tile.x = 0.0f;
			}
			else tile.x = tiles[i - 1].x + 0.36f;
		}
		else if (i > 15 && i < 41){
			tile.x = tiles[i - 1].x;
			tile.y = tiles[i - 1].y + 0.2f;
		}
		
		else if (i > 41 && i < 76){
			tile.x = tiles[0].x;
			if (i == 82) {
				tile.y = tiles[0].y + 0.2f;
			}
			else {
				tile.y = tiles[i - 1].y + 0.2f;
			}
		}
		else if (i > 76 && i < 82) {
			tile.y = tiles[22].y;
			if (i == 77) {
				tile.x = tiles[22].x - 0.36f;
			}

			else {
				tile.x = tiles[i - 1].x - 0.36f;
			}
		}
		else if (i == 82) {
			//tile.width = 0.0001f;
			tile.x = tiles[5].x;
			tile.y = tiles[5].y + 0.50f;
		}
		else if (i == 83) {
			tile.x = tiles[8].x;
			tile.y = tiles[8].y + 1.0f;
		}
		else if (i > 83 && i < 89) {
			tile.y = tiles[53].y;
			if (i == 84) {
				tile.x = tiles[53].x + 0.36f;
			}
			else tile.x = tiles[i - 1].x + 0.36f;
		}
		else if (i == 89){
			tile.x = tiles[13].x - 0.25;
			tile.y = tiles[13].y + 2.0f;
		}
		else if (i == 90) {
			tile.x = tiles[8].x;
			tile.y = tiles[8].y + 2.5f;
		}
		
		tile.textureID = tileTexture;
		tile.matrix.setPosition(tile.x, tile.y, 0);
		tiles.push_back(tile);


	}
	player.x = 0.5f;
	player.y = 0.5f;
	player.width = 0.25f;
	player.height = 0.75f;
	player.friction_x = 5.0f;
	player.matrix.setPosition(player.x, player.y, 0.0f);
	door.x = tiles[85].x;
	door.y = tiles[85].y + 1.0f;
	door.matrix.setPosition(door.x, door.y, 0.0);
	//bg1.matrix.setPosition(1, 1, 0);
}

void RenderLevel() {
	float playerVertices[] = { -.125, -.25, .125, -.25, .125, .25, -.125, -.25, .125, .25, -.125, .25 };
	float staticEntityVertices[] = { -.25, -.25, .25, -.25, .25, .25, -.25, -.25, .25, .25, -.25, .25 };
	float testVertices[] = { -0.5f * 1, -0.5f * 1, 0.5f * 1, 0.5f * 1, -0.5f * 1, 0.5f * 1, 0.5f * 1, 0.5f * 1, -0.5f * 1, -0.5f * 1, 0.5f * 1, -0.5f * 1 };
	//program->setModelMatrix(bg1.matrix);
	//bg1.draw(program, 0, 1, 1, playerVertices);
	//Draw player
	program->setModelMatrix(player.matrix);
	player.draw(program, 0, 1, 1, playerVertices);
	program->setModelMatrix(door.matrix);
	door.draw(program, 0, 1, 1, staticEntityVertices);
	//Setup how gravity / acceleration affects the static entity (player)
	player.direction_x += player.acceleration_x * FIXED_TIMESTEP;

	player.x += player.direction_x * FIXED_TIMESTEP;
	player.y += player.direction_y * FIXED_TIMESTEP;
	player.matrix.Translate(player.direction_x * FIXED_TIMESTEP, player.direction_y * FIXED_TIMESTEP, 0.0f);
	viewMatrix.setPosition(-player.x, -player.y - 0.5f, 0.0f);



	//Draw the tiles (non static entities)
	for (int i = 0; i < tiles.size(); i++)
	{
		program->setModelMatrix(tiles[i].matrix);
		tiles[i].draw(program, 0,1,1, staticEntityVertices);
		if (player.checkCollision(tiles[i])){
			player.handleCollisions(tiles[i]);
		}
	}
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);

}
void Render() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnableVertexAttribArray(program->positionAttribute);
	glEnableVertexAttribArray(program->texCoordAttribute);
	program->setProjectionMatrix(projectionMatrix);
	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	RenderLevel();
	SDL_GL_SwapWindow(displayWindow);
	
}

void Update() {

	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	float gravity = 0.5f;

	player.acceleration_y -= GRAVITY * elapsed;
	player.direction_y -= GRAVITY * elapsed;
	if (keys[SDL_SCANCODE_RIGHT]) {
		if (player.direction_x > 0) player.direction_x = 0;
		player.direction_x += 0.5f;
	}
	else if (keys[SDL_SCANCODE_LEFT]) {
		if (player.direction_x < 0) player.direction_x = 0;
		player.direction_x -= 0.5f;
	}
	else if (keys == SDL_GetKeyboardState(NULL)) {
		player.acceleration_x = 0;
		//lerping to create friction.
		player.direction_x = lerp(player.direction_x, 0.0f, FIXED_TIMESTEP * FRICTION);
	}

	if (keys[SDL_SCANCODE_SPACE]) {
		player.direction_y = 0.5f;
	}
}
void ProcessEvents() {
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
		{
			done = true;
		}
	}
}
void CleanUp() {
	SDL_Quit();
}
int main(int argc, char *argv[])
{
	
	Setup();
	while (!done)
	{
		ProcessEvents();
		Update();
		Render();

	}
	CleanUp();
	return 0;
}