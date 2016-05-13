#pragma once
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
#include "SheetSprite.h"
#include <time.h>
#include <SDL_mixer.h>

using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define LEVEL_HEIGHT 20
#define LEVEL_WIDTH 90
#define TILE_SIZE 0.3f
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEP 6

SDL_Window* displayWindow;
SDL_Event event;
ShaderProgram *program;
SDL_Joystick *joystick;

GLuint tileTexture;
GLuint titleScreenTexture;
float lastElapsedTime;
float timeLeftOver;

Matrix modelMatrix;
Matrix viewMatrix;
Matrix projectionMatrix;
unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH];
unsigned char level2Data[LEVEL_HEIGHT][LEVEL_WIDTH];
unsigned char level3Data[LEVEL_HEIGHT][LEVEL_WIDTH];

vector<Entity*> entities;
vector<Entity*> entities2;
vector<Entity*> lives;
vector<Entity*> fireballs;
Entity *player;
Entity *heart;
Entity *fireball;
int lifeCounter = 10;
int savePoint;
enum GameState { TITLE_STATE, GAME_ONE_STATE, GAME_TWO_STATE, GAME_WINNER_STATE, GAME_LOSE_STATE, GAME_THREE_STATE };
GameState currentState = TITLE_STATE;
float timer = 0;
bool done = false;
void CleanUp();
void Update(float elapsed);
const Uint8 *keys = SDL_GetKeyboardState(NULL);
bool win = false;
float playerStartingXLoc;
float playerStartingYLoc;
float fireBallStartingXLoc;
float fireBallStartingYLoc;
float screenShakeValue = 1.0f;
float screenShakeIntensity = 0.5f;
float invincibilityTimer = 0;
GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SDL_FreeSurface(surface);

	return textureID;
}

void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (UINT i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
}

void worldToTileCoordinates(float worldX, float worldY, int &currX, int &currY) {
	currX = (int)(worldX / TILE_SIZE);
	currY = (int)(worldY / TILE_SIZE);
}

void resetGame() {
	player->x = playerStartingXLoc;
	player->y = playerStartingYLoc;
	lifeCounter = 10;
	fireBallStartingXLoc = playerStartingXLoc + 10.0f + (rand() % 30);
	fireBallStartingYLoc = playerStartingYLoc + -(rand() % 30) / 2.0f;
}

void Setup() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifdef _WINDOWS
	glewInit();
#endif
	done = false;

	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl",RESOURCE_FOLDER"fragment_textured.glsl");
	//program = new ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	tileTexture = LoadTexture(RESOURCE_FOLDER"sheet_4.png");
	//tileTexture = LoadTexture(RESOURCE_FOLDER"tile_spritesheet.png");
	// Setup Title Textures
	titleScreenTexture = LoadTexture(RESOURCE_FOLDER"font.png");

	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
	// ======================================================
	// LEVEL ONE SETUP
	// Works kinda like a tile map, instead of a .txt file, it just takes the png and cuts
	// it up into triangles and store specific parts of the .png to levelData, then renders those specific parts only.
	// FILL levelData with a 20 x 90 grid with either the static tile or empty tile.
	// Sets up border (the border tiles on left, above and right
	srand((unsigned int)time(NULL)); // rand() sometimes returns the same exact number.
	for (int i = 0; i < LEVEL_HEIGHT; i++) {
		for (int j = 0; j < LEVEL_WIDTH; j++) {
			if (i == 0 || j == 0 || j == LEVEL_WIDTH - 1)
				levelData[i][j] = 190; // Static tile #190 (blue tile)
			else
				levelData[i][j] = 254; //Emptiness
		}
	}
	// Sets up tiles on the map using random function
	for (int k = 2; k < LEVEL_WIDTH - 5;) {
		for (int r = rand() % 3; r < 3; r++) {
			int j = rand() % (LEVEL_HEIGHT - 4) + 3;
			for (int i = rand() % 4; i < 6; i++) {
				levelData[j][k + i] = 190;
				if (i == 4 && (k + i) > LEVEL_WIDTH - 11) {
					levelData[j - 1][k + i] = 175; // Goal
					r = 3;
				}
			}
		}
		k += 7 + rand() % 3;
	}
	// =======================================================
	//LEVEL TWO SETUP
	
	for (int i = 0; i < LEVEL_HEIGHT; i++) {
		for (int j = 0; j < LEVEL_WIDTH; j++) {
			if (i == 0 || j == 0 || j == LEVEL_WIDTH - 1)
				level2Data[i][j] = 40;
			else
				level2Data[i][j] = 254;
		}
	}

	for (int k = 2; k < LEVEL_WIDTH - 5;) {
		for (int r = rand() % 3; r < 3; r++) {
			int j = rand() % (LEVEL_HEIGHT - 5) + 5;
			for (int i = rand() % 4; i < 6; i++) {
				level2Data[j][k + i] = 40;
				if (i == 4 && (k + i) > LEVEL_WIDTH - 11) {
					level2Data[j - 1][k + i] = 175;
					r = 3;
				}
			}
		}
		k += 7 + rand() % 3;
	}
	
	// ==========================================================
	// LEVEL THREE SETUP

	for (int i = 0; i < LEVEL_HEIGHT; i++) {
		for (int j = 0; j < LEVEL_WIDTH; j++) {
			if (i == 0 || j == 0 || j == LEVEL_WIDTH - 1)
				level3Data[i][j] = 10; 
			else
				level3Data[i][j] = 254; //Emptiness
		}
	}
	// Sets up tiles on the map using random function
	for (int k = 2; k < LEVEL_WIDTH - 5;) {
		for (int r = rand() % 3; r < 3; r++) {
			int j = rand() % (LEVEL_HEIGHT - 4) + 3;
			for (int i = rand() % 4; i < 6; i++) {
				level3Data[j][k + i] = 10;
				if (i == 4 && (k + i) > LEVEL_WIDTH - 11) {
					level3Data[j - 1][k + i] = 175; // Goal
					r = 3;
				}
			}
		}
		k += 7 + rand() % 3;
	}

	// ==========================================================
	SheetSprite playerSprite = SheetSprite(LoadTexture("p2_spritesheet.png"), 1, 1);
	player = new Entity(playerSprite);
	entities.push_back(player);
	player->width = 0.225f;
	player->height = 0.3f;
	player->x = TILE_SIZE * 6 + player->width / 2;
	player->y = -TILE_SIZE * 2;
	player->isStatic = false;
	savePoint = 6;
	playerStartingXLoc = player->x;
	playerStartingYLoc = player->y;
	// ==========Load Heart Texture================================================
	SheetSprite heartSprite = SheetSprite(LoadTexture("hud_heartFull.png"), 1, 1);
	for (int i = 0; i < 10; i++) {
		heart = new Entity(heartSprite);
		heart->width = 0.225f;
		heart->height = 0.3f;
		heart->x = player->x + (i * 0.5f);
		heart->y = player->y -3.0f;
		lives.push_back(heart);
	}
	// ===========Load Fireball texture===================================================
	SheetSprite fireBallSprite = SheetSprite(LoadTexture("fire2.png"), 1, 1);
	for (int i = 0; i < 30; i++) {
		fireball = new Entity(fireBallSprite);
		fireball->width = 0.35f;
		fireball->height =0.4f;
		fireball->x = player->x + 15.0f + (rand() % 30);
		fireball->y = player->y + -(rand() % 7) / 2.0f;
		fireballs.push_back(fireball);
	}
	// ===============================BG TEXTURE ========================================
}

void RenderTitle() {
	viewMatrix.identity();
	program->setProjectionMatrix(projectionMatrix);
	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	glEnable(GL_BLEND);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(5 / 255.0f, 100 / 255.0f, 190 / 255.0f, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	modelMatrix.identity();
	modelMatrix.Translate(-1.5f, 0.5f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(program, titleScreenTexture, "Fun Jump Race", 0.3f, 0.0f);
	modelMatrix.identity();
	modelMatrix.Translate(-3.5f, 0.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(program, titleScreenTexture, "P1:Up=Jump,<=left,>=right", 0.22f, 0.0f);
	modelMatrix.identity();
	modelMatrix.Translate(-3.3f, -1.00f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(program, titleScreenTexture, "1=Easy, 2=Hard, 3=Extreme,M=Menu", 0.2f, 0.0f);
	modelMatrix.identity();
	modelMatrix.Translate(-1.0f, -1.50f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(program, titleScreenTexture, "ESC to EXIT", 0.2f, 0.0f);

	modelMatrix.identity();
	glEnableVertexAttribArray(program->positionAttribute);
	glEnableVertexAttribArray(program->texCoordAttribute);
	SDL_GL_SwapWindow(displayWindow);
}

void RenderLevelOne() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
	viewMatrix.identity();
	viewMatrix.Translate(-player->x, -player->y, 0);

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);

	glUseProgram(program->programID);

	vector<float> vertexData;
	vector<float> texCoordData;

	int emptySpace = 0;
	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {
			if (levelData[y][x] != 254) {
				float u = (float)(((int)levelData[y][x]) % 14) / (float)14;
				float v = (float)(((int)levelData[y][x]) / 14) / (float)8;

				float spriteWidth = 1.0f / (float)14;
				float spriteHeight = 1.0f / (float)8;

				vertexData.insert(vertexData.end(), {
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, -TILE_SIZE * (y + 1),
					TILE_SIZE * (x + 1), -TILE_SIZE * (y + 1),

					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * (x + 1), -TILE_SIZE * (y + 1),
					TILE_SIZE * (x + 1), -TILE_SIZE * y
				});

				texCoordData.insert(texCoordData.end(), {
					u, v,
					u, v + spriteHeight,
					u + spriteWidth, v + spriteHeight,

					u, v,
					u + spriteWidth, v + spriteHeight,
					u + spriteWidth, v
				});
			}
			else
				emptySpace++;
		}
	}

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, &vertexData[0]);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, &texCoordData[0]);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, tileTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6 * (LEVEL_HEIGHT * LEVEL_WIDTH - emptySpace));

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);

	for (size_t i = 0; i < entities.size(); i++) {
		entities[i]->Draw(program);
	}
	if (lifeCounter > 0) {
		for (size_t i = 0; i < lifeCounter; i++) {
			lives[i]->Draw(program);
			lives[i]->x = player->x - (0.25f * i);
			lives[i]->y = player->y - 1.75f;

		}
	}
	SDL_GL_SwapWindow(displayWindow);
}
void RenderLevelTwo() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
	viewMatrix.identity();
	viewMatrix.Translate(-player->x, -player->y, 0);

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);

	glUseProgram(program->programID);

	vector<float> vertexData;
	vector<float> texCoordData;

	int emptySpace = 0;
	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {
			if (level2Data[y][x] != 254) {
				float u = (float)(((int)level2Data[y][x]) % 14) / (float)14;
				float v = (float)(((int)level2Data[y][x]) / 14) / (float)8;

				float spriteWidth = 1.0f / (float)14;
				float spriteHeight = 1.0f / (float)8;

				vertexData.insert(vertexData.end(), {
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, -TILE_SIZE * (y + 1),
					TILE_SIZE * (x + 1), -TILE_SIZE * (y + 1),

					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * (x + 1), -TILE_SIZE * (y + 1),
					TILE_SIZE * (x + 1), -TILE_SIZE * y
				});

				texCoordData.insert(texCoordData.end(), {
					u, v,
					u, v + spriteHeight,
					u + spriteWidth, v + spriteHeight,

					u, v,
					u + spriteWidth, v + spriteHeight,
					u + spriteWidth, v
				});
			}
			else
				emptySpace++;
		}
	}

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, &vertexData[0]);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, &texCoordData[0]);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, tileTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6 * (LEVEL_HEIGHT * LEVEL_WIDTH - emptySpace));

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);

	// draw entities (only the player)
	// intented to be 2 player game
	for (size_t i = 0; i < entities.size(); i++) {
		entities[i]->Draw(program);
	}
	if (lifeCounter > 0) {
		for (size_t i = 0; i < lifeCounter; i++) {
			lives[i]->Draw(program);
			lives[i]->x = player->x - (0.25f * i);
			lives[i]->y = player->y - 1.75f;

		}
	}

	SDL_GL_SwapWindow(displayWindow);
}
void RenderLevelThree() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
	viewMatrix.identity();
	viewMatrix.Translate(-player->x, -player->y, 0);

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);

	glUseProgram(program->programID);

	vector<float> vertexData;
	vector<float> texCoordData;

	int emptySpace = 0;
	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {
			if (level3Data[y][x] != 254) {
				float u = (float)(((int)level3Data[y][x]) % 14) / (float)14;
				float v = (float)(((int)level3Data[y][x]) / 14) / (float)8;

				float spriteWidth = 1.0f / (float)14;
				float spriteHeight = 1.0f / (float)8;

				vertexData.insert(vertexData.end(), {
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, -TILE_SIZE * (y + 1),
					TILE_SIZE * (x + 1), -TILE_SIZE * (y + 1),

					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * (x + 1), -TILE_SIZE * (y + 1),
					TILE_SIZE * (x + 1), -TILE_SIZE * y
				});

				texCoordData.insert(texCoordData.end(), {
					u, v,
					u, v + spriteHeight,
					u + spriteWidth, v + spriteHeight,

					u, v,
					u + spriteWidth, v + spriteHeight,
					u + spriteWidth, v
				});
			}
			else
				emptySpace++;
		}
	}

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, &vertexData[0]);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, &texCoordData[0]);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, tileTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6 * (LEVEL_HEIGHT * LEVEL_WIDTH - emptySpace));

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);

	for (size_t i = 0; i < entities.size(); i++) {
		entities[i]->Draw(program);
	}
	// if life > 0, render hearts. set their position relative to the player
	if (lifeCounter > 0) {
		for (size_t i = 0; i < lifeCounter; i++) {
			lives[i]->Draw(program);
			lives[i]->x = player->x - (0.25f * i);
			lives[i]->y = player->y - 1.75f;

		}
	}
	// render the fire balls
	for (size_t i = 0; i < fireballs.size(); i++) {
		fireballs[i]->Draw(program);
	}
	SDL_GL_SwapWindow(displayWindow);
}

void RenderWinner() {
	program->setProjectionMatrix(projectionMatrix);
	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	viewMatrix.identity();
	glEnable(GL_BLEND);
	//glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	modelMatrix.identity();
	program->setModelMatrix(modelMatrix);
	modelMatrix.Translate(-1.5f, 0.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(program, titleScreenTexture, "YOU WIN !!!!", 0.3f, 0.0f);
	modelMatrix.identity();
	program->setModelMatrix(modelMatrix);
	modelMatrix.Translate(-3.0f, -1.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(program, titleScreenTexture, "Enter to go to Main Menu", 0.25f, 0.0f);
	modelMatrix.identity();
	program->setModelMatrix(modelMatrix);
	modelMatrix.Translate(-3.0f, -1.5f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(program, titleScreenTexture, "ESC to quit", 0.25f, 0.0f);
	modelMatrix.identity();
	program->setModelMatrix(modelMatrix);

	glEnableVertexAttribArray(program->positionAttribute);
	glEnableVertexAttribArray(program->texCoordAttribute);
	SDL_GL_SwapWindow(displayWindow);
}
void RenderLoser() {
	program->setProjectionMatrix(projectionMatrix);
	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	viewMatrix.identity();
	glEnable(GL_BLEND);
	//glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	modelMatrix.identity();
	program->setModelMatrix(modelMatrix);
	modelMatrix.Translate(-1.5f, 0.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(program, titleScreenTexture, "YOU Lose :(", 0.3f, 0.0f);
	modelMatrix.identity();
	program->setModelMatrix(modelMatrix);
	modelMatrix.Translate(-3.0f, -1.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(program, titleScreenTexture, "Enter to go to Main Menu", 0.25f, 0.0f);
	modelMatrix.identity();
	program->setModelMatrix(modelMatrix);
	modelMatrix.Translate(-3.0f, -1.5f, 0.0f);
	program->setModelMatrix(modelMatrix);
	DrawText(program, titleScreenTexture, "ESC to quit", 0.25f, 0.0f);
	modelMatrix.identity();
	program->setModelMatrix(modelMatrix);

	glEnableVertexAttribArray(program->positionAttribute);
	glEnableVertexAttribArray(program->texCoordAttribute);
	SDL_GL_SwapWindow(displayWindow);
}

void Update(float elapsed) {

	int currX = 0, currY = 0;


	for (size_t i = 0; i < entities.size(); i++){

		if (!entities[i]->isStatic) {
			entities[i]->Update(elapsed);
			// If player falls (goes past the height of the map, send him back to the top of the map above a tile.
			if (entities[i]->y < TILE_SIZE * -LEVEL_HEIGHT) {
				entities[i]->y = TILE_SIZE * -2;
				entities[i]->x = TILE_SIZE * savePoint + entities[i]->width / 2;
				entities[i]->velocity_y = 0;
				lifeCounter--;
			}

			if (currentState == GAME_ONE_STATE) {
				// Top and bottom collisions with static entities
				entities[i]->y += entities[i]->velocity_y * elapsed;
				worldToTileCoordinates(entities[i]->x, entities[i]->y - entities[i]->height / 2, currX, currY);
				if (currX >= 0 && currY <= 0 && levelData[-currY][currX] == 190) {
					entities[i]->y = currY * TILE_SIZE + entities[i]->height / 2 + 0.000001f;
					entities[i]->velocity_y = 0;
					entities[i]->collidedBottom = true;
					savePoint = currX;
				}
				worldToTileCoordinates(entities[i]->x, entities[i]->y + entities[i]->height / 2, currX, currY);
				if (currX >= 0 && currY <= 0 && levelData[-currY][currX] == 190) {
					entities[i]->y = (currY - 1) * TILE_SIZE - entities[i]->height / 2 - 0.000001f;
					entities[i]->velocity_y = 0;
					entities[i]->collidedTop = true;
				}

				// Left and right collisions with static entities
				entities[i]->x += entities[i]->velocity_x * elapsed;
				worldToTileCoordinates(entities[i]->x - entities[i]->width / 2, entities[i]->y, currX, currY);
				if (currX >= 0 && currY <= 0 && levelData[-currY][currX] == 190) {
					entities[i]->x = (currX + 1) * TILE_SIZE + entities[i]->width / 2 + 0.000001f;
					entities[i]->velocity_x = 0;
					entities[i]->collidedLeft = true;
				}
				worldToTileCoordinates(entities[i]->x + entities[i]->width / 2, entities[i]->y, currX, currY);
				if (currX >= 0 && currY <= 0 && levelData[-currY][currX] == 190) {
					entities[i]->x = currX * TILE_SIZE - entities[i]->width / 2 - 0.000001f;
					entities[i]->velocity_x = 0;
					entities[i]->collidedRight = true;
				}
				// ==========================================================================================
				//Collision handling with the goal.
				entities[i]->x += entities[i]->velocity_x * elapsed;
				worldToTileCoordinates(entities[i]->x - entities[i]->width / 2, entities[i]->y, currX, currY);
				if (currX >= 0 && currY <= 0 && levelData[-currY][currX] == 175) {
					win = true;
				}
				worldToTileCoordinates(entities[i]->x + entities[i]->width / 2, entities[i]->y, currX, currY);
				if (currX >= 0 && currY <= 0 && levelData[-currY][currX] == 175) {
					win = true;
				}
				entities[i]->y += entities[i]->velocity_y * elapsed;
				worldToTileCoordinates(entities[i]->x, entities[i]->y - entities[i]->height / 2, currX, currY);
				if (currX >= 0 && currY <= 0 && levelData[-currY][currX] == 175) {
					win = true;
				}
				worldToTileCoordinates(entities[i]->x, entities[i]->y + entities[i]->height / 2, currX, currY);
				if (currX >= 0 && currY <= 0 && levelData[-currY][currX] == 175) {
					win = true;
				}
				// ==================================================================================

			}

			else if (currentState == GAME_TWO_STATE) {


				//Knocks back the player by a random amount every 4 seconds
				timer += elapsed;
				screenShakeValue += elapsed;
				if (timer >= 4.0f) {
					//SHAKE SCREEN EVERY 4 SECONDS AFTER GETTING HIT BY THE FORCE
					//viewMatrix.identity();
					//viewMatrix.Translate(0.0f, sin(screenShakeValue * 0.2f) * 100.0f, 0.0f);
					
					Mix_PlayChannel(-1, Mix_LoadWAV("wind.wav"), 0);
					int seed = static_cast<int>(time(0));
					srand(seed);
					float randNum = ((rand() % (12000 - 0)) / 1000.0) - 0.06;
					entities[i]->velocity_x -= randNum;
					timer = 0;

				}


				// Top and bottom collisions with static entities
				entities[i]->y += entities[i]->velocity_y * elapsed;
				worldToTileCoordinates(entities[i]->x, entities[i]->y - entities[i]->height / 2, currX, currY);
				if (currX >= 0 && currY <= 0 && level2Data[-currY][currX] == 40) {
					entities[i]->y = currY * TILE_SIZE + entities[i]->height / 2 + 0.000001f;
					entities[i]->velocity_y = 0;
					entities[i]->collidedBottom = true;
					savePoint = currX;
				}
				worldToTileCoordinates(entities[i]->x, entities[i]->y + entities[i]->height / 2, currX, currY);
				if (currX >= 0 && currY <= 0 && level2Data[-currY][currX] == 40) {
					entities[i]->y = (currY - 1) * TILE_SIZE - entities[i]->height / 2 - 0.000001f;
					entities[i]->velocity_y = 0;
					entities[i]->collidedTop = true;
				}

				// Left and right collisions with static entities
				entities[i]->x += entities[i]->velocity_x * elapsed;
				worldToTileCoordinates(entities[i]->x - entities[i]->width / 2, entities[i]->y, currX, currY);
				if (currX >= 0 && currY <= 0 && level2Data[-currY][currX] == 40) {
					entities[i]->x = (currX + 1) * TILE_SIZE + entities[i]->width / 2 + 0.000001f;
					entities[i]->velocity_x = 0;
					entities[i]->collidedLeft = true;
				}
				worldToTileCoordinates(entities[i]->x + entities[i]->width / 2, entities[i]->y, currX, currY);
				if (currX >= 0 && currY <= 0 && level2Data[-currY][currX] == 40) {
					entities[i]->x = currX * TILE_SIZE - entities[i]->width / 2 - 0.000001f;
					entities[i]->velocity_x = 0;
					entities[i]->collidedRight = true;
					entities[i]->x -= 0.01f;
				}

				// ==========================================================================================
				//Collision handling with the goal.
				entities[i]->x += entities[i]->velocity_x * elapsed;
				worldToTileCoordinates(entities[i]->x - entities[i]->width / 2, entities[i]->y, currX, currY);
				if (currX >= 0 && currY <= 0 && level2Data[-currY][currX] == 175) {
					win = true;
				}
				worldToTileCoordinates(entities[i]->x + entities[i]->width / 2, entities[i]->y, currX, currY);
				if (currX >= 0 && currY <= 0 && level2Data[-currY][currX] == 175) {
					win = true;
				}
				entities[i]->y += entities[i]->velocity_y * elapsed;
				worldToTileCoordinates(entities[i]->x, entities[i]->y - entities[i]->height / 2, currX, currY);
				if (currX >= 0 && currY <= 0 && level2Data[-currY][currX] == 175) {
					win = true;
				}
				worldToTileCoordinates(entities[i]->x, entities[i]->y + entities[i]->height / 2, currX, currY);
				if (currX >= 0 && currY <= 0 && level2Data[-currY][currX] == 175) {
					win = true;
				}
				// ==================================================================================
			}
			else if (currentState == GAME_THREE_STATE) {
				invincibilityTimer += elapsed;

				// Randomizes fireball to a certain extent.
				// Randomizes the X and Y starting points of the fire ball based on player location.
				for (size_t i = 0; i < fireballs.size(); i++) {
					fireballs[i]->x -= 0.01f;
					if (fireballs[i]->x < player->x - 3.0f) {
						fireballs[i]->x = player->x + 10.0f + (rand() % 30);
						fireballs[i]->y = player->y + -(rand() % 7) / 2.0f;
					}
					if (!player->invincibility) {
						if (player->checkCollision(fireballs[i])) {
							lifeCounter--;
							//player->handleCollisions(fireballs[i]);
							player->invincibility = true;
							player->enableInvincibility();
							if (invincibilityTimer >= 3) {
								player->invincibility = false;
							}

						}

					}
				}

				entities[i]->y += entities[i]->velocity_y * elapsed;
				worldToTileCoordinates(entities[i]->x, entities[i]->y - entities[i]->height / 2, currX, currY);
				if (currX >= 0 && currY <= 0 && level3Data[-currY][currX] == 10) {
					entities[i]->y = currY * TILE_SIZE + entities[i]->height / 2 + 0.000001f;
					entities[i]->velocity_y = 0;
					entities[i]->collidedBottom = true;
					savePoint = currX;
				}
				worldToTileCoordinates(entities[i]->x, entities[i]->y + entities[i]->height / 2, currX, currY);
				if (currX >= 0 && currY <= 0 && level3Data[-currY][currX] == 10) {
					entities[i]->y = (currY - 1) * TILE_SIZE - entities[i]->height / 2 - 0.000001f;
					entities[i]->velocity_y = 0;
					entities[i]->collidedTop = true;
				}

				// Left and right collisions with static entities
				entities[i]->x += entities[i]->velocity_x * elapsed;
				worldToTileCoordinates(entities[i]->x - entities[i]->width / 2, entities[i]->y, currX, currY);
				if (currX >= 0 && currY <= 0 && level3Data[-currY][currX] == 10) {
					entities[i]->x = (currX + 1) * TILE_SIZE + entities[i]->width / 2 + 0.000001f;
					entities[i]->velocity_x = 0;
					entities[i]->collidedLeft = true;
				}
				worldToTileCoordinates(entities[i]->x + entities[i]->width / 2, entities[i]->y, currX, currY);
				if (currX >= 0 && currY <= 0 && level3Data[-currY][currX] == 10) {
					entities[i]->x = currX * TILE_SIZE - entities[i]->width / 2 - 0.000001f;
					entities[i]->velocity_x = 0;
					entities[i]->collidedRight = true;
					entities[i]->x -= 0.01f;
				}

				// ==========================================================================================
				//Collision handling with the goal.
				entities[i]->x += entities[i]->velocity_x * elapsed;
				worldToTileCoordinates(entities[i]->x - entities[i]->width / 2, entities[i]->y, currX, currY);
				if (currX >= 0 && currY <= 0 && level3Data[-currY][currX] == 175) {
					win = true;
				}
				worldToTileCoordinates(entities[i]->x + entities[i]->width / 2, entities[i]->y, currX, currY);
				if (currX >= 0 && currY <= 0 && level3Data[-currY][currX] == 175) {
					win = true;
				}
				entities[i]->y += entities[i]->velocity_y * elapsed;
				worldToTileCoordinates(entities[i]->x, entities[i]->y - entities[i]->height / 2, currX, currY);
				if (currX >= 0 && currY <= 0 && level3Data[-currY][currX] == 175) {
					win = true;
				}
				worldToTileCoordinates(entities[i]->x, entities[i]->y + entities[i]->height / 2, currX, currY);
				if (currX >= 0 && currY <= 0 && level3Data[-currY][currX] == 175) {
					win = true;
				}
			}
		}
	}
}

void Update() {

	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastElapsedTime;
	lastElapsedTime = ticks;

	float fixedElapsed = elapsed;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEP)
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEP;
	while (fixedElapsed > FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;
		Update(FIXED_TIMESTEP);
	}

	//User input (movement)
	// Space to jump, left arrow to move left, right arrow to move right
	if (keys[SDL_SCANCODE_UP] && player->collidedBottom) {
		player->velocity_y = 5.8f;
		player->collidedBottom = false;
		Mix_PlayChannel(-1, Mix_LoadWAV("jump_11.wav"), 0);
	}
	if (keys[SDL_SCANCODE_LEFT]) {
		player->acceleration_x = -5.8f;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		player->acceleration_x = 5.8f;
	}
	else {
		player->acceleration_x = 0;
	}

	Update(fixedElapsed);

	if (currentState == TITLE_STATE) {
		if (keys[SDL_SCANCODE_1]) currentState = GAME_ONE_STATE;
		else if (keys[SDL_SCANCODE_2]) currentState = GAME_TWO_STATE;
		else if (keys[SDL_SCANCODE_3]) currentState = GAME_THREE_STATE;
	}
	else if (currentState == GAME_WINNER_STATE) {
		if (keys[SDL_SCANCODE_RETURN]) {
			currentState = TITLE_STATE;
		}
	}
	else if (win) {
		currentState = GAME_WINNER_STATE;
		Mix_PlayChannel(-1, Mix_LoadWAV("1_person_cheering-Jett_Rifkin-1851518140.wav"), 0);
		win = false;
	}
	else if (lifeCounter <= 0) {
		currentState = GAME_LOSE_STATE;
		Mix_PlayChannel(-1, Mix_LoadWAV("GBOMG_f.mp3"), 0);
	}
	else if (currentState == GAME_LOSE_STATE) {
		if (keys[SDL_SCANCODE_RETURN]) {
			currentState = TITLE_STATE;
		}
	}
	else if (keys[SDL_SCANCODE_M]) {
		if (currentState != TITLE_STATE) {
			currentState = TITLE_STATE;
			resetGame();
		}
		
		else CleanUp();
	}

}
	


bool ProcessEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			return false;
		}
		if (keys[SDL_SCANCODE_ESCAPE]) { return false; }

		return true;

	}
	return true;


}
void CleanUp() {
	SDL_Quit();
}


int main(int argc, char *argv[])
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Music *music = Mix_LoadMUS("Dungeon Theme.mp3");
	Mix_PlayMusic(music, -1);
	Setup();
	while (ProcessEvents())
	{
		Update();
		if (currentState == TITLE_STATE) RenderTitle();
		else if (currentState == GAME_ONE_STATE) RenderLevelOne();
		else if (currentState == GAME_TWO_STATE) RenderLevelTwo();
		else if (currentState == GAME_THREE_STATE) RenderLevelThree();
		else if (currentState == GAME_WINNER_STATE) {
			RenderWinner();
			resetGame();
		}
		else if (currentState == GAME_LOSE_STATE) {
			RenderLoser();
			resetGame();
		}
		
	}
	CleanUp();
	return 0;
}
