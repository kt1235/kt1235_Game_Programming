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

SDL_Window* displayWindow;
const Uint8 *keys = SDL_GetKeyboardState(NULL);

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

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	SDL_Event event;
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	Matrix projectionMatrix;
	Matrix viewMatrix;

	int screenWidth = 640;
	int screenHeight = 360;
	glViewport(0, 0, screenWidth, screenHeight);
	glUseProgram(program.programID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float projectionWidth = 6.5f;
	float projectionHeight = 3.5f;
	projectionMatrix.setOrthoProjection(-projectionWidth / 2, projectionWidth / 2, -projectionHeight / 2, projectionHeight / 2, -1.0f, 1.0f);

	Entity player;
	player.x = -1.5f;
	player.y = 1.2f;
	player.width = .25f;
	player.height = .5f;
	player.friction_x = 5.0f;
	player.matrix.setPosition(player.x, player.y, 0.0f);

	//GLuint spriteSheetTexture = LoadTexture("tiles_spritesheet.png");
	GLuint tileTexture = LoadTexture("fart00.png");
	GLuint playerTexture = LoadTexture("alienBlue_square.png");
	player.textureID = playerTexture;

	//vector of Entities which will store all the tile entities.
	// will be used later when need to check the static entities against non static
	vector<Entity> tiles;
	// create all the platforms
	//every 8 indices is 1 platform for a total
	//of 5 platforms. then stairlike map is created.
	for (int i = 0; i < 60; i++)
	{
		Entity tile;
		tile.width = 0.2f;
		tile.height = 0.2f;

		if (i == 0) {
			tile.x = -2.5f;
			tile.y = 1.0f;
		}
		else if (i == 1 || i == 2 || i == 3 || i == 4 || i == 5 || i == 6 || i == 7) {
			tile.x = (tiles[i - 1].x + 0.2f);
			tile.y = tiles[i - 1].y;
		}

		else if (i == 8) {
			tile.x = 1.0f;
			tile.y = 1.0f;
		}
		else if (i == 9 || i == 10 || i == 11 || i == 12 || i == 13 || i == 14 || i == 15) {
			tile.x = (tiles[i - 1].x + 0.2f);
			tile.y = tiles[i - 1].y;
		}
		else if (i == 16) {
			tile.x = -0.75;
			tile.y = 0.0f;
		}
		else if (i == 17 || i == 18 || i == 19 || i == 20 || i == 21 || i == 22 || i == 23){
			tile.x = (tiles[i - 1].x + 0.2f);
			tile.y = tiles[i - 1].y;
		}
		else if (i == 24) {
			tile.x = -2.5f;
			tile.y = -1.0f;
		}
		else if (i == 25 || i == 26 || i == 27 || i == 28 || i == 29 || i == 30 || i == 31) {
			tile.x = (tiles[i - 1].x + 0.2f);
			tile.y = tiles[i - 1].y;
		}
		else if (i == 32) {
			tile.x = 1.0f;
			tile.y = -1.0f;
		}
		else if (i == 33 || i == 34 || i == 35 || i == 36 || i == 37 || i == 38 || i == 39) {
			tile.x = (tiles[i - 1].x + 0.2f);
			tile.y = tiles[i - 1].y;
		}
		else if (i > 39) {
			tile.x = tiles[i - 1].x + 0.3;
			tile.y = tiles[i - 1].y + 0.3;
		}
		tile.textureID = tileTexture;
		tile.matrix.setPosition(tile.x, tile.y, 0);
		tiles.push_back(tile);

	}

	float gravity = 0.5f;
	float ticks = 0.0f;
	float lastFrameTicks = 0.0f;
	float elapsed = 0.0f;

	bool done = false;
	while (!done)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnableVertexAttribArray(program.positionAttribute);
		glEnableVertexAttribArray(program.texCoordAttribute);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		ticks = (float)SDL_GetTicks() / 1000.0f;
		elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		//Draw player
		program.setModelMatrix(player.matrix);
		player.draw(&program, 0, 1, 1);
		//Setup how gravity / acceleration affects the static entity (player)
		player.direction_x += player.acceleration_x * FIXED_TIMESTEP;
		player.acceleration_y -= gravity * elapsed;
		player.x += player.direction_x * FIXED_TIMESTEP;
		player.y += player.direction_y * FIXED_TIMESTEP;
		player.matrix.Translate(player.direction_x * FIXED_TIMESTEP, player.direction_y * FIXED_TIMESTEP, 0.0f);
		player.direction_y -= gravity * elapsed;
		viewMatrix.setPosition(-player.x, -player.y + 0.5f, 0.0f);

		//Draw the tiles (non static entities)
		for (int i = 0; i < tiles.size(); i++)
		{
			program.setModelMatrix(tiles[i].matrix);
			tiles[i].draw(&program, 0, 1, 1);
			if (player.checkCollision(tiles[i])){
				player.handleCollisions(tiles[i]);
			}
		}

		if (keys[SDL_SCANCODE_RIGHT]) {
			if (player.direction_x > 0) player.direction_x = 0;
			player.direction_x += 0.3f;
		}
		else if (keys[SDL_SCANCODE_LEFT]) {
			if (player.direction_x < 0) player.direction_x = 0;
			player.direction_x -= 0.3f;
		}
		else if (keys == SDL_GetKeyboardState(NULL)) {
			player.acceleration_x = 0;
			//lerping to create friction.
			player.direction_x = lerp(player.direction_x, 0.0f, FIXED_TIMESTEP * player.friction_x);
		}

		if (keys[SDL_SCANCODE_SPACE]) {
			player.direction_y = 0.5f;
		}
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
			{
				done = true;
			}
		}

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}