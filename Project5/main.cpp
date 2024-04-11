/**
* Author: Kerry Huang
* Assignment: Platformer
* Date due: 2024-04-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

/*
Menu Screen:
    a. Scene objects 
3 Levels:
    a. all levels must scroll 
    b. must have platforms 
3 Lives:
AI:
    a. moving AI 
    b. giant AI 
    c. jumping AI 
Audio:
    a. normal bgm 
    b. sound fx 
*/

#include <vector>

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>

#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"   // We'll talk about these later in the course


#include <fstream> //just to output???

#include <iomanip>
#include <sstream>
#include <SDL_mixer.h>

#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "Level1.h"


SDL_Window* displayWindow;
bool gameIsRunning = true;
int collisionStatus = 0;

//background colors 
float BG_RED = 0.53f,
BG_GREEN = 0.80f,
BG_BLUE = 0.92f,
BG_OPACITY = 1.0f;


//window sizes and dimensions 
const int WINDOW_WIDTH = 640,
    WINDOW_HEIGHT = 480;

//viewports for camera
const int VIEWPORT_X = 0,
    VIEWPORT_Y = 0,
    VIEWPORT_WIDTH = WINDOW_WIDTH,
    VIEWPORT_HEIGHT = WINDOW_HEIGHT;



//shaders 
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";     // make sure not to use std::string objects for these!
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const char PLAYER_SPRITE[] = "protagonist.png";
const char ENEMY_SPRITE[] = "wizard.png";
const char MAP_TILESET_FILEPATH[] = "grass.png";

const char FONT_SPRITE[] = "font1.png";

const char BGM_FILEPATH[] = "sweetdreams.mp3";

bool endgame = false;



std::ofstream logme;

//shader program and matricies 
glm::mat4 view_matrix, projection_matrix; //flip panes and field of view and projection of camera 

//shader 
ShaderProgram g_program;


const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0; // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0; // this value MUST be zero



Scene* g_current_scene;
Level1* g_level_1;

void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}


void initialize() {


    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO); //initializing both video and audio 


    displayWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);  //create camera    // Initialise our camera
    g_program.load(V_SHADER_PATH, F_SHADER_PATH); //create shaders 
    //g_program_1.load(V_SHADER_PATH, F_SHADER_PATH); //create shaders

    // Initialise our view, model, and projection matrices
    view_matrix = glm::mat4(1.0f);
    view_matrix = glm::translate(view_matrix, glm::vec3(-5.0, 3.75, 0.0f));


    //for entities. reset to the initial position and then translate into the desired position. 
    //modle matrix = translate into initial position 
    //
    float zoomout = 1.75;
    float xmove = 2.0f;
    float ymove = -1.0f;
    
    projection_matrix = glm::ortho(-5.0f * zoomout + xmove, 5.0f * zoomout + xmove, -3.75f * zoomout + ymove, 3.75f * zoomout + ymove, -1.0f, 1.0f);  
    //model_matrix = glm::mat4(1.0f);

    
    //set our matricies 
    g_program.set_view_matrix(view_matrix);
    g_program.set_projection_matrix(projection_matrix);

    glUseProgram(g_program.get_program_id());

    g_level_1 = new Level1();
    switch_to_scene(g_level_1);


    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
}


const float MILLISECONDS_IN_SECOND = 1000.0;
float prev_ticks = 0;
float time_accumulate = 0;
const float FIXED_TIMESTEP = 0.0001;


const int FONTBANK_SIZE = 16;


void update() {


    //movement 
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;  // get the current number of ticks
    float delta_time = ticks - prev_ticks;     // the delta time is the difference from the last frame
    prev_ticks = ticks;

    //logme << *(g_state.nightfall) << " lol\n";


    //create a fixed timestep 
    delta_time += time_accumulate; //time accumulated in total 

    if (delta_time < FIXED_TIMESTEP) {
        time_accumulate = delta_time;
        return;
    }
    while (delta_time >= FIXED_TIMESTEP) {
        //make updates to the thing 
        g_current_scene->update(FIXED_TIMESTEP);
       
        delta_time -= FIXED_TIMESTEP;
    }
    time_accumulate = delta_time;

    //logme << " enemies_defeated " << g_state.enemies_defeated << "\n";

    view_matrix = glm::mat4(1.0f);
    //bring it to the normal location 
    //view_matrix = glm::translate(view_matrix, glm::vec3(-5.0, 3.75, 0.0f));

    view_matrix = glm::translate(view_matrix, glm::vec3(0.0f, 3.75, 0.0f));
    view_matrix = glm::translate(view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, 0.0f, 0.0f));
    g_program.set_view_matrix(view_matrix);

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
}



void processinput() {
    SDL_Event event;


    while (SDL_PollEvent(&event)) {

        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
        }


        const Uint8* key_state = SDL_GetKeyboardState(NULL);

        g_current_scene->m_state.player->set_direction(g_current_scene->m_state.player->IDLE); //how to put this into the code instead of here

        if (key_state[SDL_SCANCODE_LEFT]) {
            g_current_scene->m_state.player->set_direction(g_current_scene->m_state.player->LEFT);
        }

        if (key_state[SDL_SCANCODE_RIGHT]){
            g_current_scene->m_state.player->set_direction(g_current_scene->m_state.player->RIGHT);
        }

        if (key_state[SDL_SCANCODE_UP]){
            g_current_scene->m_state.player->set_direction(g_current_scene->m_state.player->UP);
        }
    }
}

void render() {
    // Step 1
    glClear(GL_COLOR_BUFFER_BIT);

    g_current_scene->render(&g_program);

    // Step 4
    SDL_GL_SwapWindow(displayWindow);
}

void shutdown() {
    delete g_level_1;
    SDL_Quit();
}

int main(int argc, char* argv[]) {

    logme.open("log.txt");

    initialize();
    
    while (gameIsRunning) {
        processinput();

        if (!endgame) {
            update();
            render();
        }
    }

    logme.close();

    shutdown();
    return 0;
}
