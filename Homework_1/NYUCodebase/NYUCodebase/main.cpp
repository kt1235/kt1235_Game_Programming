// Kenny Tsang
// Intro to Game Programming
// Homework 1
// Atleast 1 animated texture, and use atleast 3 textures.
#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

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

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	SDL_Event event;
	
	glViewport(0, 0, 640, 360);
	ShaderProgram program(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");

	Matrix projectionMatrix;
	Matrix modelMatrixShip;
	Matrix viewMatrix;
	Matrix modelMatrixPlayerShip;
	Matrix modelMatrixMeteor;
	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	GLuint ship = LoadTexture("enemyShip.png");
	GLuint playerShip = LoadTexture("playerShip1_blue.png");
	GLuint meteor = LoadTexture("meteorBrown_tiny1.png");
	//modelMatrix.identity();
	float ship_vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	float playerShip_vertices[] = { -2.5, -0.5, -1.5, -0.5, -1.5, 0.5, -2.5, -0.5, -1.5, 0.5, -2.5, 0.5 };
	float meteor_vertices[] = { 1.5, -0.5, 2.5, -0.5, 2.5, 0.5, 1.5, -0.5, 2.5, 0.5, 1.5, 0.5 };
	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(program.programID);
	modelMatrixShip.identity();
	float lastFrameTicks = 0.0f;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		
		program.setModelMatrix(modelMatrixShip);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(program.programID);

		
		//create the ship
		glBindTexture(GL_TEXTURE_2D, ship);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ship_vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glEnableVertexAttribArray(program.texCoordAttribute);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		//create the second ship
		glBindTexture(GL_TEXTURE_2D, playerShip);
		program.setModelMatrix(modelMatrixPlayerShip);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, playerShip_vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		
		glEnableVertexAttribArray(program.texCoordAttribute);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		//create meteor
		glBindTexture(GL_TEXTURE_2D, meteor);
		program.setModelMatrix(modelMatrixMeteor);
		modelMatrixMeteor.Translate((-1)*elapsed, 0.0, 0.0);
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, meteor_vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		
		glEnableVertexAttribArray(program.texCoordAttribute);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
	
		modelMatrixShip.Translate(0.0, (-1)*elapsed , 0.0);
		modelMatrixShip.Rotate(elapsed);
		SDL_GL_SwapWindow(displayWindow);
		if (ticks >= 1){
			modelMatrixPlayerShip.Translate(elapsed, elapsed, 0.0);
		}
		
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
	}

	//glClearColor(1.0f, 0.0f, 0.0f, 0.0f);

	SDL_Quit();
	return 0;
}
