
#include <stdio.h>

// WINDOWS

// #include <SDL.h>
// #include <glad\glad.h>

// MACOS

// #include <SDL2/SDL.h>
// #include <OpenGL/gl3.h>

// LINUX

#include "Libraries/SDL2-devel-2.0.12-mingw/SDL2-devel-2.0/SDL2-2.0.12/x86_64-w64-mingw32/include/SDL2/SDL.h"
#include "Libraries/SDL2-devel-2.0.12-mingw/SDL2-devel-2.0/SDL2-2.0.12/x86_64-w64-mingw32/include/SDL2/SDL_timer.h"
// #include <GL/gl.h>
#include "Libraries/glad/glad.h"

#include <stdio.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "Libraries/stb/stb_image.h"
#include "headers/shader.h"

#define TRUE 1
#define FALSE 0
#define FULLSCREEN FALSE

static const int SCREEN_FULLSCREEN = 0;
static const int SCREEN_WIDTH  = 960;
static const int SCREEN_HEIGHT = 540;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer;
static SDL_GLContext maincontext;


GLuint screenWidth = 600, screenHeight = 600;
// const GLFWvidmode* mode;
// GLFWwindow* window;

void setupVAO();
GLuint getTextureHandle(char* path);
unsigned int VAO; // There is also a line_VAO, but declared in main

// GLFW
// user input functions
// void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
// int keys[1024];
// void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// SDL
// const Uint8 *key;
const Uint8 *keys;

GLfloat lastX = 0.0, lastY = 0.0;
GLfloat mouse_click_x = 0.0, mouse_click_y = 0.0;
GLfloat mouse_release_y = 0.0, mouse_release_x = 0.0;
int firstMouse = TRUE;
int is_drawing_selection_box_bool = FALSE;

float currentTime, deltaTime, lastFrame;

typedef struct Color {
    float r, g, b, a;
} Color;

typedef struct Rectangle {
    float x, y;
    float width;
    float height;
    Color color;
} Rectangle;

int bullet_switch_bool = FALSE;
typedef struct Bullet {
    float x, y, distance_shot;
} Bullet;

typedef struct Block {
    float x, y, width, height;
    Color color;
} Block;

int num_zombies = 0;
typedef struct Zombie {
    int direction; // 0, 1, 2, 3 for up, right, down, left
    // float width = 0.1;
    // float height = 0.1;
    Rectangle rect;
    // rect.x = 0.3; rect.y = 0.2;
    // rect.width = 0.1; rect.height = 0.1;
} Zombie;
float zombie_width = 0.1;
float zombie_height = 0.1;

int check_collision(Rectangle* r1, Rectangle* r2);
GLuint textured_quad_shader; // defined here bc used in the draw_rect function
void draw_rect(Rectangle r1, GLuint shader);

float player_upward_velocity = 0.0;
float player_upward_position = 0.0;
int player_can_jump_bool = TRUE;
float player_downward_velocity = 0.0;

float mouse_adjustment_x = 5.0;
float mouse_adjustment_y = -50.0;

float line_vertices[72]; // 3 coords per vertex, 24 vertexs; 3 * 24 = 72


int num_bullets = 0;
Bullet *bullets;

int num_blocks = 0;
Block *blocks;

static void sdl_die(const char * message) {
  fprintf(stderr, "%s: %s\n", message, SDL_GetError());
  exit(2);
}

void init_screen(const char * caption) {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
		sdl_die("Couldn't initialize SDL");
	atexit (SDL_Quit);
	SDL_GL_LoadLibrary(NULL); // Default OpenGL is fine.

	// Request an OpenGL 4.5 context (should be core)
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	// Also request a depth buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Create the window
	if (SCREEN_FULLSCREEN) {
	window = SDL_CreateWindow(
		caption,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL
	);
	} else {
	window = SDL_CreateWindow(
		caption,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL
	);
	}
	if (window == NULL) sdl_die("Couldn't set video mode");

	maincontext = SDL_GL_CreateContext(window);
	// renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // Create the renderer

	if(renderer == NULL) // management of errors
	{
	    printf("Error when creating renderer: %s",SDL_GetError());
	    exit(EXIT_FAILURE);
	}

	if (maincontext == NULL)
	sdl_die("Failed to create OpenGL context");

	// Check OpenGL properties
	printf("OpenGL loaded\n");
	gladLoadGLLoader(SDL_GL_GetProcAddress);
	printf("Vendor:   %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version:  %s\n", glGetString(GL_VERSION));

	// Use v-sync
	SDL_GL_SetSwapInterval(1);

	// Disable depth test and face culling.
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	int w,h;
	SDL_GetWindowSize(window, &w, &h);
	glViewport(0, 0, w, h);
	glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
}


int main(int argc, char *argv[]) {
  init_screen("OpenGL 4.5");
  SDL_Event event;
  int quit = FALSE;



    // set up Vertex Array Object that contains our vertices and bind it
    setupVAO();
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to unbind in the setupVertexArray function and then bind here, but we'll do so for clarity, organization, and avoiding possible bugs in future

    textured_quad_shader = glCreateProgram();
    buildShaders(textured_quad_shader, "shaders/textured_quad.vs", "shaders/textured_quad.fs");

    GLuint line_shader = glCreateProgram();
    buildShaders(line_shader, "shaders/line.vs", "shaders/line.fs");

    GLuint test_shader = glCreateProgram();
    buildShaders(test_shader, "shaders/test.vs", "shaders/test.fs");

    GLuint sprite_shader = glCreateProgram();
    buildShaders(sprite_shader, "shaders/sprite.vs", "shaders/sprite.fs");

    GLuint resting_sprite_shader = glCreateProgram();
    buildShaders(resting_sprite_shader, "shaders/resting_sprite.vs", "shaders/resting_sprite.fs");

    GLuint walking_zombie_shader = glCreateProgram();
    buildShaders(walking_zombie_shader, "shaders/walking_zombie.vs", "shaders/walking_zombie.fs");

    GLuint colored_quad_shader = glCreateProgram();
    buildShaders(colored_quad_shader, "shaders/colored_quad.vs", "shaders/colored_quad.fs");

    GLuint zombie_texture = getTextureHandle("assets/zombie.png");
    GLuint tower_texture = getTextureHandle("assets/tower.png");
    GLuint monster_texture = getTextureHandle("assets/monster.png");
    GLuint hero_texture = getTextureHandle("assets/hero.png");
    GLuint shadow_texture = getTextureHandle("assets/shadow.png");
    GLuint background_texture = getTextureHandle("assets/background.png");
    GLuint cursor_texture = getTextureHandle("assets/dwarven_gauntlet.png");

    // for alpha (opacity)
    glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int window_width, window_height;



    Rectangle r1, r2, r3, r5; // r3 is cursor, r1 is player
    r1.x = -1.0; r1.y = 0.0; r1.width = 0.1; r1.height = 0.1; r1.color.r = 0.7; r1.color.g = 0.2; r1.color.b = 1.0; r1.color.a = 0.9; //player
    r2.x = 0.0; r2.y = 0.0; r2.width = 0.1; r2.height = 0.2; r2.color.r = 0.3; r2.color.g = 0.2; r2.color.b = 1.0; r2.color.a = 0.9; //monster
    r3.x = -0.4; r3.y = -0.4; r3.width = 0.1; r3.height = 0.1; r3.color.r = 0.3; r3.color.g = 0.2; r3.color.b = 0.8; r3.color.a = 0.9; //cursor
    // r4.x = -0.1; r4.y = 0.6; r4.width = 0.1; r4.height = 0.15; r4.color.r = 0.3; r4.color.g = 0.2; r4.color.b = 0.2; r4.color.a = 0.9;
    r5.x = -1.0; r5.y = -68.0 / 120.0; r5.width = 2.0; r5.height = 0.1; r5.color.r = 0.3; r5.color.g = 0.2; r5.color.b = 0.2; r5.color.a = 0.0;



    num_blocks++;
    blocks = realloc(blocks, sizeof(Block) * num_blocks);

    Zombie zombie1;
    zombie1.rect.x = 0.3; zombie1.rect.y = 0.2;
    zombie1.rect.width = 0.1; zombie1.rect.height = 0.1;

    int numBlocks = 2;
    Rectangle* blocks[2] = {&r2, &r5};
    // printf("%s\n", "hi");
    // printf("%lu", sizeof(blocks));
    // printf("%s\n", "hi");
    // return 0;
    // return 0;


    // line vao

    // 0,0 to 0,1
    line_vertices[0] = 0.0;
    line_vertices[1] = 0.0;
    line_vertices[2] = 0.0;

    line_vertices[3] = 0.0;
    line_vertices[4] = 1.0;
    line_vertices[5] = 0.0;

    // 0,0 to 1,0
    line_vertices[6] = 0.0;
    line_vertices[7] = 0.0;
    line_vertices[8] = 0.0;

    line_vertices[9] = 1.0;
    line_vertices[10] = 0.0;
    line_vertices[11] = 0.0;

    // 0,0 to 0,-1
    line_vertices[12] = 0.0;
    line_vertices[13] = 0.0;
    line_vertices[14] = 0.0;

    line_vertices[15] = 0.0;
    line_vertices[16] = -1.0;
    line_vertices[17] = 0.0;

    // 0,0 to -1,0
    line_vertices[18] = 0.0;
    line_vertices[19] = 0.0;
    line_vertices[20] = 0.0;

    line_vertices[21] = -1.0;
    line_vertices[22] = 0.0;
    line_vertices[23] = 0.0;

    unsigned int line_VAO, line_VBO;
    glGenVertexArrays(1, &line_VAO);
    glGenBuffers(1, &line_VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(line_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, line_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertices), line_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // end line vao

    GLfloat lineWidthRange[2] = {0.0f, 0.0f};
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
    printf("%f\n", lineWidthRange[0]);
    printf("%f\n", lineWidthRange[1]);
    // Maximum supported line width is in lineWidthRange[1].

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(7.0);

	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	double deltaTime = 0;

    int alien_x=0, alien_y=0;
    int alien_xvel=0, alien_yvel=0;

	while (!quit) {

			while (SDL_PollEvent(&event)) {


			keys = SDL_GetKeyboardState(NULL);
			if (keys[SDL_SCANCODE_RETURN]) {
			    printf("<RETURN> is pressed.\n");
			}
			if (keys[SDL_SCANCODE_RIGHT] && keys[SDL_SCANCODE_UP]) {
			    printf("Right and Up Keys Pressed.\n");
			}

			LAST = NOW;
			NOW = SDL_GetPerformanceCounter();

			currentTime = NOW;
			deltaTime = (double)((NOW - LAST)*1000 / (double)SDL_GetPerformanceFrequency() );

			// // need how to get time in sdl
			currentTime = SDL_GetTicks() / 1000.0;
			// printf("currentTime: %f\n", currentTime);
			deltaTime = currentTime - lastFrame;
			lastFrame = currentTime;

			// missing get window size
			// glfwGetWindowSize(window, &window_width, &window_height);
			SDL_GetWindowSize(window, &window_width, &window_height);
			glViewport(0, 0, window_width, window_height);

			if(keys[SDLK_ESCAPE]) quit = TRUE;

			if(keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) r1.x = r1.x + 0.01;
			for(int i = 0; i < numBlocks; i++){
			    if(check_collision(&r1, blocks[i])){
			        r1.x = blocks[i]->x - r1.width;
			    }
			}

			if(keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) r1.x = r1.x - 0.01;

			for(int i = 0; i < numBlocks; i++){
			    if(check_collision(&r1, blocks[i])){
			        r1.x = blocks[i]->x + blocks[i]->width;
			    }
			}

			if(keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) r1.y = r1.y + 0.01;

			for(int i = 0; i < numBlocks; i++){
			    if(check_collision(&r1, blocks[i])){
			        r1.y = blocks[i]->y - r1.height;
			    }
			}

			if(keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) r1.y = r1.y - 0.01;

			for(int i = 0; i < numBlocks; i++){
			    if(check_collision(&r1, blocks[i])){
			        r1.y = blocks[i]->y + blocks[i]->height;
			    }
			}


			if(player_can_jump_bool && keys[SDL_SCANCODE_SPACE]){
			    player_upward_velocity += 0.01;
			    player_can_jump_bool = FALSE;
			}

			// GRAVITY
			player_downward_velocity += deltaTime * 0.03;

			// SET PLAYER_Y_VELOCITY
			player_upward_position += player_upward_velocity - player_downward_velocity;

			if(player_upward_position <= 0.0){
			    player_upward_position = 0.0;
			    player_downward_velocity = 0.0;
			    player_upward_velocity = 0.0;
			    player_can_jump_bool = TRUE;
			}

			glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			float SCR_WIDTH = (float) window_width;
			float SCR_HEIGHT = (float) window_height;

			// float mouse_x = lastX - window_width;
			float mouse_x = lastX;
			float mouse_y = window_height - lastY;
			r3.x = (mouse_x / (float) window_width - 0.5) * 2.0;
			r3.y = (mouse_y / (float) window_height - 0.5) * 2.0;
			// printf("mouse_x: %f\n", mouse_x);
			// printf("mouse_y: %f\n", mouse_y);
			// printf("SCR_WIDTH: %f\n", SCR_WIDTH);
			// if(lastX > 1000.0){




			glBindVertexArray(VAO);

			glUseProgram(textured_quad_shader);
			glBindTexture(GL_TEXTURE_2D, background_texture);
			glUniform2f(glGetUniformLocation(textured_quad_shader, "trans"), -1.0, -1.0);
			glUniform2f(glGetUniformLocation(textured_quad_shader, "scale"), 2.0, 2.0);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


			glBindTexture(GL_TEXTURE_2D, tower_texture);
			glUseProgram(textured_quad_shader);
			// glUniform1f(glGetUniformLocation(textured_quad_shader, "time"), currentTime);
			glUniform2f(glGetUniformLocation(textured_quad_shader, "trans"), r2.x, r2.y);
			glUniform2f(glGetUniformLocation(textured_quad_shader, "scale"), r2.width, r2.height);
			glUniform4f(glGetUniformLocation(textured_quad_shader, "color"), r2.color.r, r2.color.g, r2.color.b, r2.color.a);
			// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			draw_rect(r2, textured_quad_shader);

			glBindTexture(GL_TEXTURE_2D, zombie_texture);
			glUseProgram(walking_zombie_shader);
			glUniform1f(glGetUniformLocation(walking_zombie_shader, "time"), currentTime);
			glUniform2f(glGetUniformLocation(walking_zombie_shader, "trans"), zombie1.rect.x, zombie1.rect.y);
			glUniform2f(glGetUniformLocation(walking_zombie_shader, "scale"), zombie_width, zombie_height);
			// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			draw_rect(zombie1.rect, walking_zombie_shader);
			zombie1.rect.x -= 0.001;


			glBindTexture(GL_TEXTURE_2D, shadow_texture);
			glUseProgram(textured_quad_shader);
			float shadow_scale_modifier = 0.3;
			float shadow_adjustment = player_upward_position * shadow_scale_modifier;
			glUniform2f(glGetUniformLocation(textured_quad_shader, "trans"), r1.x + shadow_adjustment / 2.0, r1.y);
			glUniform2f(glGetUniformLocation(textured_quad_shader, "scale"), r1.width - shadow_adjustment, r1.height / 4.0);
			glUniform4f(glGetUniformLocation(textured_quad_shader, "color"), r1.color.r, r1.color.g, r1.color.b, r1.color.a);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			glUseProgram(sprite_shader);
			glBindTexture(GL_TEXTURE_2D, hero_texture);
			glUniform2f(glGetUniformLocation(sprite_shader, "trans"), r1.x, r1.y + player_upward_position);
			glUniform2f(glGetUniformLocation(sprite_shader, "scale"), r1.width, r1.height);
			glUniform4f(glGetUniformLocation(sprite_shader, "color"), r1.color.r, r1.color.g, r1.color.b, r1.color.a);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


			glUseProgram(textured_quad_shader);
			glBindTexture(GL_TEXTURE_2D, cursor_texture);
			draw_rect(r3, textured_quad_shader);

			glUseProgram(colored_quad_shader);

			int accepted_x[2] = {3,4};
			int accepted_y[2] = {5,6};

			for(int i = 0; i < 31000; i++){ // 230 x 135 = 31050
			    int x = i % 230;
			    int y = floor(i / 230.0);
			    float width = 2.0 / 230.0;
			    float height = 2.0 / 135.0;
			    glUniform2f(glGetUniformLocation(colored_quad_shader, "trans"), (float)x * width - 1.0, (float)y * height - 1.0);
			    glUniform2f(glGetUniformLocation(colored_quad_shader, "scale"), width, height);
			    glUniform4f(glGetUniformLocation(colored_quad_shader, "color"), (float)x * width / 2.0, (float)y * height / 2.0, r1.color.b, r1.color.a);
			    if(x == accepted_x[0] && (int) y == accepted_y[1]) glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}


			// for making bullets
			if(bullet_switch_bool){

			    num_bullets++;
			    bullets = realloc(bullets, sizeof(Bullet) * num_bullets);

			    // num_bullets - 1 bc if num_bullets = 1 then it means you only allocated memory for bullets[0]
			    bullets[num_bullets - 1].x = r1.x;
			    bullets[num_bullets - 1].y = r1.y + player_upward_position;
			    bullets[num_bullets - 1].distance_shot = 0.07;

			    // printf("num_bullets: %d\n", num_bullets);


			    // for(int i = 0; i < num_bullets; i++){
			        // bullets[num_bullets - 1].x++;
			    // }

			    bullet_switch_bool = FALSE;

			}
			for(int i = 0; i < num_bullets; i++){
			    bullets[i].distance_shot += 0.015;
			    int x =13; int y = 12;
			    float width = 2.0 / 230.0;
			    float height = 2.0 / 135.0;
			    glUniform2f(glGetUniformLocation(colored_quad_shader, "trans"), bullets[i].x + bullets[i].distance_shot, bullets[i].y + 0.05);
			    glUniform2f(glGetUniformLocation(colored_quad_shader, "scale"), width, height);
			    glUniform4f(glGetUniformLocation(colored_quad_shader, "color"), 1.0, (-cos(20.0 * bullets[i].distance_shot) + 1.0) / 2.0, (-cos(20.0 * bullets[i].distance_shot) + 1.0) / 2.0, 1.0);
			    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}

			int x =13; int y = 12;
			float width = 2.0 / 230.0;
			float height = 2.0 / 135.0;
			glUniform2f(glGetUniformLocation(colored_quad_shader, "trans"), (float)x * width - 1.0, (float)y * height - 1.0);
			glUniform2f(glGetUniformLocation(colored_quad_shader, "scale"), width, height);
			glUniform4f(glGetUniformLocation(colored_quad_shader, "color"), (float)x * width / 2.0, (float)y * height / 2.0, r1.color.b, r1.color.a);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


			// for drawing selection box using lines
			if (is_drawing_selection_box_bool){

			    float adjusted_mouse_click_x = mouse_click_x + mouse_adjustment_x;
			    float adjusted_mouse_click_y = mouse_click_y + mouse_adjustment_y;
			    float adjusted_mouse_x = lastX + mouse_adjustment_x;
			    float adjusted_mouse_y = lastY + mouse_adjustment_y;

			    glBindVertexArray(line_VAO);
			    glUseProgram(line_shader);
			    // glUniform1i(glGetUniformLocation(line_shader, "is_drawing_selection_box_bool"), is_drawing_selection_box_bool);;
			    glUniform4f(glGetUniformLocation(line_shader, "color"), 1.0, 0.0, 0.0, 1.0);
			    glUniform2f(glGetUniformLocation(line_shader, "line_start_point"), adjusted_mouse_click_x, adjusted_mouse_click_y);
			    glUniform2f(glGetUniformLocation(line_shader, "scale"), (adjusted_mouse_x - adjusted_mouse_click_x) / SCR_WIDTH, (adjusted_mouse_y - adjusted_mouse_click_y) / SCR_HEIGHT);
			    printf("%f\n", (lastX - adjusted_mouse_click_x) / SCR_WIDTH);
			    glUniform2f(glGetUniformLocation(line_shader, "resolution"), SCR_WIDTH, SCR_HEIGHT);
			    glDrawArrays(GL_LINES, 0, 2);
			    glDrawArrays(GL_LINES, 2, 2);
			    glUniform2f(glGetUniformLocation(line_shader, "line_start_point"), adjusted_mouse_x, adjusted_mouse_y);
			    glUniform2f(glGetUniformLocation(line_shader, "scale"), -(adjusted_mouse_x - adjusted_mouse_click_x) / SCR_WIDTH, -(adjusted_mouse_y - adjusted_mouse_click_y) / SCR_HEIGHT);
			    glDrawArrays(GL_LINES, 0, 2);
			    glDrawArrays(GL_LINES, 2, 2);
			}



			// float SCR_WIDTH = (float) screenWidth;
			// float SCR_HEIGHT = (float) screenHeight;

			// glUseProgram(test_shader);
			// glUniform2f(glGetUniformLocation(test_shader, "mouse"), lastX/ SCR_WIDTH, lastY / SCR_HEIGHT);
			// glUniform1f(glGetUniformLocation(test_shader, "time"), currentTime * 1.);
			// // glUniform1f(glGetUniformLocation(shader_glsl_sandbox_copy_paste6, "my_var"), my_var1);
			// // glUniform1f(glGetUniformLocation(shader_glsl_sandbox_copy_paste6, "my_var2"), my_var2);
			// // glUniform2f(glGetUniformLocation(shader_glsl_sandbox_copy_paste6, "surfacePosition"), SCR_WIDTH, SCR_HEIGHT);
			// //glUniform1f(glGetUniformLocation(shader_glsl_sandbox_copy_paste, "to_text"), currentTime);
			// glUniform2f(glGetUniformLocation(test_shader, "resolution"), SCR_WIDTH, SCR_HEIGHT);
			// // glUniform2f(glGetUniformLocation(test_shader, "trans"), 0.0, 0.0);
			// // glUniform2f(glGetUniformLocation(test_shader, "scale"), 1.0, 1.0);
			// // glUniform4f(glGetUniformLocation(test_shader, "color"), r1.color.r, r1.color.g, r1.color.b, r1.color.a);

			// glUniform2f(glGetUniformLocation(test_shader, "trans"), -1.0, -1.0);
			// glUniform2f(glGetUniformLocation(test_shader, "scale"), r2.width, r2.height);
			// glUniform4f(glGetUniformLocation(test_shader, "color"), r1.color.r, r1.color.g, r1.color.b, r1.color.a);
			// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			// glBindVertexArray(0); // no need to unbind it every time

			// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
			// -------------------------------------------------------------------------------
			// glfwSwapBuffers(window);
			// glfwPollEvents();


			if (event.type == SDL_QUIT) quit = TRUE;
        	SDL_GL_SwapWindow(window);
    	}
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void draw_rect(Rectangle r1, GLuint shader){
    glUseProgram(shader);
    glUniform2f(glGetUniformLocation(shader, "trans"), r1.x, r1.y);
    glUniform2f(glGetUniformLocation(shader, "scale"), r1.width, r1.height);
    // glUniform4f(glGetUniformLocation(shader, "color"), r1.color.r, r1.color.g, r1.color.b, r1.color.a);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


int check_collision(Rectangle* r1, Rectangle* r2){
    if (r1->x < r2->x + r2->width &&
        r1->x + r1->width > r2->x &&
        r1->y < r2->y + r2->height &&
        r1->y + r1->height > r2->y) {
            return 1; // collision
    } else {
        return 0;
    }
}

// void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
// {
//     if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, GL_TRUE);
//     if (key >= 0 && key < 1024)
//     {
//         if(action == GLFW_PRESS)
//             keys[key] = TRUE;
//         else if(action == GLFW_RELEASE)
//             keys[key] = FALSE;
//     }
// }

// void mouse_callback(GLFWwindow* window, double xpos, double ypos)
// {
//     if(firstMouse)
//     {
//         lastX = xpos;
//         lastY = ypos;
//         firstMouse = FALSE;
//     }

//     GLfloat xoffset = xpos - lastX;
//     GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

//     lastX = xpos;
//     lastY = ypos;

//     // camera.ProcessMouseMovement(xoffset, yoffset);
// }

// void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
// {
//     if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
//         mouse_click_y = lastY;
//         mouse_click_x = lastX;
//         printf("user has clicked mouse button left\n");
//         bullet_switch_bool = TRUE;
//         is_drawing_selection_box_bool = TRUE;
//     }
//     if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
//         mouse_release_y = lastX;
//         mouse_release_x = lastY;
//         printf("user has released mouse button left\n");
//         is_drawing_selection_box_bool = FALSE;
//     }
// }


// void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
// {
//     // camera.ProcessMouseScroll(yoffset);
// }

void setupVAO(){
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions            // textures
         1.0f,  1.0f, 0.0f,     1.0f, 1.0f, // top right
         1.0f, 0.0f, 0.0f,     1.0f, 0.0f, // bottom right
        0.0f, 0.0f, 0.0f,     0.0f, 0.0f, // bottom left
        0.0f,  1.0f, 0.0f,     0.0f, 1.0f // top left
    };

    unsigned int indices[] = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    unsigned int VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}

GLuint getTextureHandle(char* path)
{
    GLuint textureHandle;
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

    // Set our texture parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // Set texture wrapping to GL_REPEAT
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);   // Set texture wrapping to GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // Set texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Load, create texture and generate mipmaps;
    //
    // Note: image loaders usually think of top left as being (0,0) while in OpenGL I would rather think of bottom left as being (0,0) as OpenGL does that already, so that is why I set the stb library to flip image vertically. There are other workarounds like flipping our texCoords upside down or flipping things in the vs or fs, but that would mean that we are choosing in OpenGL to work with two different coordinate systems, one upside-down from the other. I would rather choose not to do that and simply flip images when loading in. It is a matter of personal choice.
    //

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *image = stbi_load(path, &width, &height, &nrChannels, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // free memory
    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0); // unbind so that we can deal with other textures

    return textureHandle;
}
