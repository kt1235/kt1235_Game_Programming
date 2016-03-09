// Kenny Tsang
// Intro to Game Programming
// Homework 2
// PONG 
#ifdef _WINDOWS
#include <GL/glew.h>
#include <SDL_mixer.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <cmath>
#include "Entity.h"
#include "ShaderProgram.h"
#include <vector>


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;
Entity ball, paddle_left, paddle_right, topWall, bottomWall, number1, number2;
float lastFrameTicks;
const Uint8 *keys = SDL_GetKeyboardState(NULL);

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
/*
class MixerSound{
public:
	Uint32 offset;
	Uint32 length;
	Uint8 *buffer;
	float volume;
	SDL_AudioFormat format;
	bool loaded;
	bool playing;
	bool loop;

};

class DemoApp {
public: 
	std::vector<MixerSound> mixerSounds;

	int loadSound(const char *soundFile) {
		Uint8 *buffer;
		SDL_AudioSpec spec;
		Uint32 bufferSize;

		if (SDL_LoadWAV(soundFile, &spec, &buffer, &bufferSize) == NULL) {
			return -1;
		}
		SDL_AudioCVT cvt;
		SDL_BuildAudioCVT(&cvt, spec.format, spec.channels, spec.freq, deviceSpec.format, deviceSpec.channels, deviceSpec.freq);
		cvt.len = bufferSize;
		cvt.buf = new Uint8[bufferSize*cvt.len_mult];
		memcpy(cvt.buf, buffer, bufferSize);

		SDL_ConvertAudio(&cvt);
		SDL_FreeWAV(buffer);

		MixerSound sound;
		sound.buffer = cvt.buf;
		sound.length = cvt.len_cvt;
		sound.loaded = true;
		sound.offset = 0;
		sound.format = deviceSpec.format;
		sound.volume = 1.0;
		sound.playing = false;
		mixerSounds.push_back(sound);

		return mixerSounds.size() - 1;
	}

	void playSound(int soundIndex, bool loop) {
		if (soundIndex >= 0 && soundIndex < mixerSounds.size()) {
			mixerSounds[soundIndex].playing = true;
			mixerSounds[soundIndex].offset = 0;
			mixerSounds[soundIndex].loop = loop;
		}
	}
};
*/
void Setup() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	//ShaderProgram program(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");
	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	
	GLuint ballTexture = LoadTexture("ufoGreen.png");
	GLuint paddleTexture = LoadTexture("laserBlue13.png");
	GLuint paddleTexture2 = LoadTexture("laserGreen03.png");
	GLuint topWallTexture = LoadTexture("laserRed13.png");
	GLuint bottomWallTexture = LoadTexture("laserRed13.png");
	GLuint num1 = LoadTexture("number1.png");
	GLuint num2 = LoadTexture("number2.png");

	//initial setup of textures
	ball.textureID = ballTexture;
	paddle_left.textureID = paddleTexture;
	paddle_right.textureID = paddleTexture2;
	topWall.textureID = topWallTexture;
	bottomWall.textureID = bottomWallTexture;
	number1.textureID = num1;
	number2.textureID = num2;
	//top walls location and size
	topWall.width = 0.1f;
	topWall.height = 4.7f;
	topWall.rotation = 90;
	topWall.y = 1.0f;
	//bottom wall location and size
	bottomWall.width = 0.f;
	bottomWall.height = 4.7f;
	bottomWall.rotation = 90;
	bottomWall.y = -1.0f;
	bottomWall.width = 0.1f;
	//left paddle and right paddles location
	paddle_left.height = 0.5f;
	paddle_left.width = 0.1f;
	paddle_right.height = 0.5f;
	paddle_right.width = 0.1f;
	paddle_left.x = (-0.65f);
	paddle_right.x = (0.65f);
	//ball size + speed
	ball.scale = 0.08f;
	ball.direction_x = 1.0f;
	ball.direction_y = 1.0f;
	ball.speed = 0.5f;
	//number1 (indicacting that left player wins) size + location
	//number 2 (indicating that right player wins) size + location
	number1.x = 0.0f;
	number1.y = 0.0f;
	number2.x = 0.0f;
	number2.y = 0.0f;

	number1.rotation = -90;
	number1.width = 1.0f;
	number1.height = 1.0f;
	number2.rotation = -90;
	number2.width = 1.0f;
	number2.height = 1.0f;
}

void Render() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	paddle_left.Draw();
	ball.Draw();
	paddle_right.Draw();
	topWall.Draw();
	bottomWall.Draw();

	//Show Winner if ball goes past paddle
	if (ball.x >= 1.16f) {
		number1.Draw();
	}
	else if (ball.x <= -1.16f) {
		number2.Draw();
	}
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

	Mix_Music *ballHit = Mix_LoadMUS("ballHit.wav");

	ball.y += ball.speed*elapsed*ball.direction_y;
	ball.x += ball.speed*elapsed*ball.direction_x;

	if (keys[SDL_SCANCODE_W]) {
		paddle_left.y += 1.0f*elapsed;
	}
	else if (keys[SDL_SCANCODE_S]) {
		paddle_left.y -= 1.0f*elapsed;

	}
	if (keys[SDL_SCANCODE_UP]) {
		paddle_right.y += 1.0f*elapsed;
	}
	else if (keys[SDL_SCANCODE_DOWN]) {
		paddle_right.y -= 1.0f*elapsed;
	}

	//Setting up top and bottom wall boundaries so that the paddles cannot move past the walls.
	//Left paddle + top wall
	if (paddle_left.y + ((paddle_left.scale * paddle_left.height) / 4) > topWall.y / 2) {
		paddle_left.y -= (elapsed * paddle_left.speed);
		Mix_PlayChannel(-1, Mix_LoadWAV("paddleSound.wav"), 0);
	}
	//Left paddle + bottom wall
	if (paddle_left.y - ((paddle_left.scale * paddle_left.height) / 4) < bottomWall.y / 2) {
		paddle_left.y += (elapsed * paddle_left.speed);
		Mix_PlayChannel(-1, Mix_LoadWAV("paddleSound.wav"), 0);
	}
	//Right paddle + top wall
	if (paddle_right.y + ((paddle_right.scale * paddle_right.height) / 4) > topWall.y / 2) {
		paddle_right.y -= (elapsed * paddle_left.speed);
		Mix_PlayChannel(-1, Mix_LoadWAV("paddleSound.wav"), 0);
	//Right paddle + bottom wall
	}
	if (paddle_right.y - ((paddle_right.scale * paddle_right.height) / 4) < bottomWall.y / 2) {
		paddle_right.y += (elapsed * paddle_left.speed);
		Mix_PlayChannel(-1, Mix_LoadWAV("paddleSound.wav"), 0);
	}

	//Setting up bounds such that ball will rebound when it hits paddle
	// (These boundaries are not very well done...)
	if (ball.x >= 1.15f && ball.y >= paddle_right.y - 0.40f && ball.y <= paddle_right.y + 0.40){
		ball.direction_x *= -1;
		Mix_PlayChannel(-1, Mix_LoadWAV("paddleSound.wav"), 0);
	}
	if (ball.x <= -1.15f && ball.y >= paddle_left.y - 0.40f && ball.y <= paddle_left.y + 0.40f){
		ball.direction_x *= -1;
		Mix_PlayChannel(-1, Mix_LoadWAV("paddleSound.wav"), 0);
	}
	

	//Setting the wall boundaries for the ball so that it doesn't fly off the screen
	if (ball.y >= 0.85f) {
		//ball.direction_x = ball.direction_y;
		ball.direction_y *= -1;
		Mix_PlayChannel(-1, Mix_LoadWAV("ballHit.wav"), 0);
	}
	if (ball.y <= -0.85f){
		ball.direction_y *= -1;
		Mix_PlayChannel(-1, Mix_LoadWAV("ballHit.wav"), 0);
	}
}


int main(int argc, char *argv[])
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Music *music = Mix_LoadMUS("ChillingMusic.wav");
	Mix_PlayMusic(music, -1);
	Setup();
	while (ProcessEvents()) {
		Update();
		Render();
	}
	SDL_Quit();
	return 0;
}
