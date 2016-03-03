// Kenny Tsang
// Intro to Game Programming
// Homework 3
// Space Invaders
// Couldnt finish the collision checking / laser hitting enemies
// Couldn't get sprites to work so everything is mainly hard coded.
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <cmath>
#include "Entity.h"
#include "ShaderProgram.h"
#include "DrawSpriteSheetSprite.h"
#include <vector>


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;
Entity pressEnter, redShip, spriteSheet, enemy1, enemy2, enemy3, enemy4, enemy5, enemy6, enemy7, enemy8, laser, laser2, laser3, laser4;
float lastFrameTicks;
int scene = 0;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
std::vector <Entity> enemies;

GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);

	return textureID;
}

bool CollisionCheck(Entity entity, Entity laser) {
	float entity_top = entity.y + (entity.height / 2.0f);
	float entity_bot = entity.y - (entity.height / 2.0f);
	float entity_right = entity.x + (entity.width / 2.0f);
	float entity_left = entity.x - (entity.width / 2.0f);

	float laser_top = laser.y + (laser.height / 2.0f);
	float laser_bot = laser.y - (laser.height / 2.0f);
	float laser_right = laser.x + (laser.width / 2.0f);
	float laser_left = laser.x - (laser.width / 2.0f);

	//laser.scale *= 5;
	if (entity_bot > laser_top) {
		return false;
	}
	else if (entity_top < laser_bot) {
		return false;
	}
	else if (entity_left > laser_right){
		return false;
	}
	else if (entity_right < laser_left) {
		return false;
	}
}

void Setup() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	//ShaderProgram program(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");

	
	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	
	GLuint pressEnterTexture = LoadTexture("pressEnter.png");
	pressEnter.textureID = pressEnterTexture;
	pressEnter.scale = 0.50f;
	pressEnter.x = -0.25;

	GLuint redShipTexture = LoadTexture("playerShip2_red.png");
	redShip.textureID = redShipTexture;
	redShip.scale = 0.25f;
	redShip.y = -0.95f;

	GLuint enemyText1 = LoadTexture("enemyRed1.png");
	enemy1.textureID = enemyText1;
	//enemy1.scale = 0.25f;
	enemy1.x = -1.0f;
	enemy2.textureID = enemyText1;
	//enemy2.scale = 0.25f;
	enemy2.x = -0.50f;
	enemy3.textureID = enemyText1;
	//enemy3.scale = 0.25f;
	enemy3.x = 0.0f;
	enemy4.textureID = enemyText1;
	//enemy4.scale = 0.25f;
	enemy4.x = 0.50f;
	enemy5.textureID = enemyText1;
	//enemy5.scale = 0.25f;
	enemy5.x = 1.0f;

	enemy6.textureID = enemyText1;
	//enemy6.scale = 0.25f;
	enemy6.y = 0.25;
	enemy6.x = -0.50f;

	enemy7.textureID = enemyText1;
	//enemy7.scale = 0.25f;
	enemy7.y = 0.25f;
	enemy7.x = -.025f;

	enemy8.textureID = enemyText1;
	//enemy8.scale = 0.25f;
	enemy8.y = 0.25f;
	enemy8.x = 0.50f;

	GLuint laserText = LoadTexture("laserRed03.png");
	laser.textureID = laserText;
	laser2.textureID = laserText;
	laser3.textureID = laserText;
	laser4.textureID = laserText;
	laser.scale = 0.05f;
	laser.direction_y = 1.5;
	laser.y = 10.0f;
	enemy1.direction_x = 0.10f;
	enemy2.direction_x = 0.1f;
	enemy3.direction_x = 0.1f;
	enemy4.direction_x = 0.1f;
	enemy5.direction_x = 0.1f;
	enemy6.direction_x = 0.1f;
	enemy7.direction_x = 0.1f;
	enemy8.direction_x = 0.1f;

}

void RenderTitle() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	pressEnter.Draw();
	//GLuint spriteSheetTexture;
	//spriteSheetTexture = LoadTexture("sheet.png");
	//DrawSpriteSheetSprite enemySprite = DrawSpriteSheetSprite(spriteSheetTexture, 425, 552, 93, 84);
	//enemySprite.Draw(0, 0, 0.5, 0);
	SDL_GL_SwapWindow(displayWindow);
}

void RenderGamePlay() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	redShip.Draw();
	enemy1.Draw();
	
	enemy2.Draw();
	enemy3.Draw();
	enemy4.Draw();
	enemy5.Draw();
	enemy6.Draw();
	enemy7.Draw();
	enemy8.Draw();
	
	laser.Draw();
	

	SDL_GL_SwapWindow(displayWindow);
}

bool ProcessEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			return false;
		}
		return true;
	}
	return true;
}
void Update(){
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	int timer = elapsed;
	laser.y += laser.speed * elapsed *laser.direction_y;
	enemy1.x += enemy1.speed * elapsed * enemy1.direction_x;
	enemy2.x += enemy2.speed * elapsed * enemy2.direction_x;
	enemy3.x += enemy3.speed * elapsed * enemy3.direction_x;
	enemy4.x += enemy4.speed * elapsed * enemy4.direction_x;
	enemy5.x += enemy5.speed * elapsed * enemy5.direction_x;
	enemy6.x += enemy6.speed * elapsed * enemy6.direction_x;
	enemy7.x += enemy7.speed * elapsed * enemy7.direction_x;
	enemy8.x += enemy8.speed * elapsed * enemy8.direction_x;

	if (keys[SDL_SCANCODE_RETURN]) {
		scene = 1;
	}
	if (keys[SDL_SCANCODE_LEFT]) {
		redShip.x -= 0.05;

	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		redShip.x += 0.05f;
	}
	else if (keys[SDL_SCANCODE_UP]) {
		if (laser.y > 1.0f) {
			laser.x = redShip.x + 0.095f;
			laser.y = redShip.y + 0.25f;
		}
	}

	//Hard coded all the movement because I couldn't figure out how to get the sprite images to work.
	
	if (enemy8.x >= 1.0f) {
		enemy8.direction_x *= -1.0f;
	}
	if (enemy8.x <= -0.0f) {
		enemy8.direction_x *= -1.0f;
	}
	if (enemy7.x >= 0.50f) {
		enemy7.direction_x *= -1.0f;
	}
	if (enemy7.x <= -0.50f) {
		enemy7.direction_x *= -1.0f;
	}
	if (enemy6.x >= 0.0f) {
		enemy6.direction_x *= -1.0f;
	}
	if (enemy6.x <= -1.0f) {
		enemy6.direction_x *= -1.0f;
	}
	if (enemy5.x > 1.15f) {
		enemy5.direction_x *= -1.0f;
	}
	if (enemy5.x <= 0.65f) {
		enemy5.direction_x *= -1.0f;
	}
	if (enemy4.x >= 0.65f) {
		enemy4.direction_x *= -1.0f;
	}
	if (enemy4.x <= 0.15f) {
		enemy4.direction_x *= -1.0f;
	}
	if (enemy3.x >= 0.15f) {
		enemy3.direction_x *= -1.0f;
	}
	if (enemy3.x <= -0.35f) {
		enemy3.direction_x *= -1.0f;
	}

	if (enemy2.x >= -0.35f) {
		enemy2.direction_x *= -1;
	}
	if (enemy2.x <= -0.85f) {
		enemy2.direction_x *= -1;
	}

	if (enemy1.x >= -0.85f) {
		enemy1.direction_x *= -1;
	}
	if (enemy1.x <= -1.35f) {
		enemy1.direction_x *= -1;
	}
	// Couldn't get collision to work, most likely because I didnt use sprites. 
	if (CollisionCheck(enemy1, laser) == true) {
		enemy1.y = 10.0f;
	}


}


int main(int argc, char *argv[])
{
	Setup();
	while (ProcessEvents()) {
		Update();
		if (scene == 0) {
			RenderTitle();
		}
		else if (scene == 1) {
			RenderGamePlay();
		}
		
		
	}
	SDL_Quit();
	return 0;
}
