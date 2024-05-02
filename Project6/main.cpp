/**
* 
*Heavily Referenced the Code found: https://www.youtube.com/watch?v=lS_qeBy3aQI&ab_channel=Pezzza%27sWork
*Used this code to make a physics engine to have the circle collision 

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
#include "Utility.h"
#include "Scene.h"
#include "Level1.h"
#include "Level2.h"

#include "LoadScreen.h"


SDL_Window* displayWindow;
bool gameIsRunning = true;

//background colors 
float BG_RED = 0.53f,
BG_GREEN = 0.80f,
BG_BLUE = 0.92f,
BG_OPACITY = 1.0f;


enum Coordinate { x_coordinate, y_coordinate };

const Coordinate X_COORDINATE = x_coordinate;
const Coordinate Y_COORDINATE = y_coordinate;


//tell professor that the ortho width and height are messed up and that could be causing a lot of pain
const float ORTHO_WIDTH = 10.0f,
ORTHO_HEIGHT = 7.5f;



//window sizes and dimensions 
const int WINDOW_WIDTH = 640,
    WINDOW_HEIGHT = 480;

//viewports for camera
const int VIEWPORT_X = 0,
    VIEWPORT_Y = 0,
    VIEWPORT_WIDTH = WINDOW_WIDTH,
    VIEWPORT_HEIGHT = WINDOW_HEIGHT;



//shaders 
const char V_SHADER_PATH[] = "shaders/vertex.glsl";     // make sure not to use std::string objects for these!
const char F_SHADER_PATH[] = "shaders/fragment.glsl";

const char V_TEXT_SHADER_PATH[] = "shaders/vertex_textured.glsl";     // make sure not to use std::string objects for these!
const char F_TEXT_SHADER_PATH[] = "shaders/fragment_textured.glsl";



const char FONT_SPRITE[] = "font1.png";

const char BGM_FILEPATH[] = "bread_song.mp3";



std::ofstream logme;

//shader program and matricies 
glm::mat4 view_matrix, projection_matrix; //flip panes and field of view and projection of camera 

//shader 
ShaderProgram g_program, text_program;


const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0; // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0; // this value MUST be zero



Scene* g_current_scene;
LoadScreen* loadscreen;
std::vector<Scene*> scenes = {new Level1(), new Level2()};
int curr_scene = 0;


void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}


void initialize() {


    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO); //initializing both video and audio 

    displayWindow = SDL_CreateWindow("Ballz", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);  //create camera    // Initialise our camera
    
    g_program.load(V_SHADER_PATH, F_SHADER_PATH); //create shaders 
    text_program.load(V_TEXT_SHADER_PATH, F_TEXT_SHADER_PATH); //create shaders for text 

    // Initialise our view, model, and projection matrices
    view_matrix = glm::mat4(1.0f);

    //for entities. reset to the initial position and then translate into the desired position. 
    //modle matrix = translate into initial position 
    //
    float zoomout = 1;
    float xmove = 0;
    float ymove = 0;
    
    projection_matrix = glm::ortho(-5.0f * zoomout + xmove, 5.0f * zoomout + xmove, -3.75f * zoomout + ymove, 3.75f * zoomout + ymove, -1.0f, 1.0f);  
    //model_matrix = glm::mat4(1.0f);

    
    //set our matricies 
    g_program.set_view_matrix(view_matrix);
    g_program.set_projection_matrix(projection_matrix);
    g_program.set_colour(1.0f, 0.0f, 0.0f, 1.0f);


    //set our matrices for text 
    text_program.set_view_matrix(view_matrix);
    text_program.set_projection_matrix(projection_matrix);

    glUseProgram(g_program.get_program_id());

    //loadscreen = new LoadScreen();
    switch_to_scene(scenes[0]);
    

    // enable blending
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
}


const float MILLISECONDS_IN_SECOND = 1000.0;
float prev_ticks = 0;
float time_accumulate = 0;
const float FIXED_TIMESTEP = 0.0005f; //0.0005f;


void update() {
    
    if (g_current_scene->m_state.endgame == true) {
        curr_scene++;
        if (curr_scene < scenes.size()) {
            switch_to_scene(scenes[curr_scene]);
        }
    }

    //movement 
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;  // get the current number of ticks
    float delta_time = ticks - prev_ticks;     // the delta time is the difference from the last frame
    prev_ticks = ticks;


    //create a fixed timestep 
    delta_time += time_accumulate; //time accumulated in total 

    if (delta_time < FIXED_TIMESTEP) {
        time_accumulate = delta_time;
    }
    while (delta_time >= FIXED_TIMESTEP) {
        //make updates to the thing 
        g_current_scene->update(FIXED_TIMESTEP, logme);
       
        delta_time -= FIXED_TIMESTEP;
    }
    time_accumulate = delta_time;

    view_matrix = glm::mat4(1.0f);


}



float get_screen_to_ortho(float coordinate, Coordinate axis)
{
    switch (axis)
    {
    case x_coordinate: return ((coordinate / WINDOW_WIDTH) * ORTHO_WIDTH) - (ORTHO_WIDTH / 2.0);
    case y_coordinate: return (((WINDOW_HEIGHT - coordinate) / WINDOW_HEIGHT) * ORTHO_HEIGHT) - (ORTHO_HEIGHT / 2.0);
    default: return 0.0f;
    }
}

void processinput() {
    SDL_Event event;


    while (SDL_PollEvent(&event)) {

        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;

            case SDL_MOUSEBUTTONDOWN:
                // event.motion.x
                // event.motion.y
                // event.button.button
                

                int x, y;
                SDL_GetMouseState(&x, &y);

                float x_val = get_screen_to_ortho(x, x_coordinate);
                float y_val = get_screen_to_ortho(y, y_coordinate);

                g_current_scene->spawn(x_val, y_val, -1);

        }

        if (SDL_KEYDOWN && (event.key.keysym.sym == SDLK_SPACE)) {
            if (g_current_scene == loadscreen) {
                switch_to_scene(scenes[curr_scene]);
            }
        }

        const Uint8* key_state = SDL_GetKeyboardState(NULL);
    }
}

void render() {
    // Step 1
    glClear(GL_COLOR_BUFFER_BIT);

    g_current_scene->render(&g_program, &text_program, logme);

   
    /*
    glm::mat4 modelmat = glm::mat4(1.0f);

    g_program.set_model_matrix(modelmat);

    float vertices[] = { 0.5f, -0.5f, 0.0f, 0.5f, -0.5f, -0.5f };


    glVertexAttribPointer(g_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_program.get_position_attribute());
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(g_program.get_position_attribute());
    */

   /* glm::vec3 position;
    position = { 0.0, 0.0, 0.0 };
    Utility::draw_text(&g_program, font_texture_id, "Have I been here before?", 0.5, 0, position);*/

    // Step 4
    SDL_GL_SwapWindow(displayWindow);
}

void shutdown() {
    for (Scene* ptr : scenes) {
        delete ptr;
    }
    SDL_Quit();
}

int main(int argc, char* argv[]) {

    logme.open("log.txt");

    initialize();
    
    while (gameIsRunning) {
        processinput();

        update();
        render();
    }

    logme.close();

    shutdown();
    return 0;
}