/**
* Author: [Kerry Huang]
* Assignment: Simple 2D Scene
* Date due: 2024-02-17, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

/*
1. 2 objects with different textures   50%                              DONE 
2. movement in objects                 25%  1 must move in relation     DONE
3. rotation                            25%  1 object must rotate        DONE

extra:
1. background change color every second                                 DONE 
2. revolving object change scale in heart beat                          DONE
3. both objects translate not up and down                               DONE

*/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>

#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"   // We'll talk about these later in the course

#include "stb_image.h" //load a texture 



SDL_Window* displayWindow;
bool gameIsRunning = true;


//background colors 
const float BG_RED = 1.0f,
    BG_GREEN = 0.987f,
    BG_BLUE = 0.078f,
    BG_OPACITY = 1.0f;

const float BG_RED_1 = 0.333f,
    BG_GREEN_1 = 0.1059f,
    BG_BLUE_1 = 0.222f,  
    BG_OPACITY_1 = 0.3f;

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

const char PLAYER_SPRITE[] = "smile.png";
const char PLAYER_1_SPRITE[] = "frown.png";

GLuint g_player_texture_id;
GLuint g_player_texture_id_1;

//shader program and matricies 
ShaderProgram g_program;
glm::mat4 g_view_matrix; //position + orientation of camera 
glm::mat4 g_model_matrix; // Defines every translation, rotation, and/or scaling applied to an object; we'll look at these next week
glm::mat4 g_projection_matrix; //flip panes and field of view and projection of camera 

//2nd triangle's shader 
ShaderProgram g_program_1;
glm::mat4 g_view_matrix_1; //position + orientation of camera 
glm::mat4 g_model_matrix_1; // Defines every translation, rotation, and/or scaling applied to an object; we'll look at these next week
glm::mat4 g_projection_matrix_1; //flip panes and field of view and projection of camera


const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0; // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0; // this value MUST be zero

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        std::cout << "Unable to load image. Make sure the path is correct.";
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters. pixel art and stuff 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void initialize() {


    SDL_Init(SDL_INIT_VIDEO); //initializing 
    displayWindow = SDL_CreateWindow("Two roads diverged in a yellow wood, filled with happy little trees.", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_HEIGHT, VIEWPORT_HEIGHT);  //create camera    // Initialise our camera
    g_program.load(V_SHADER_PATH, F_SHADER_PATH); //create shaders 
    //g_program_1.load(V_SHADER_PATH, F_SHADER_PATH); //create shaders

    // Initialise our view, model, and projection matrices
    g_view_matrix = glm::mat4(1.0f);
    g_model_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  

    //object 2's matricies 
    //g_view_matrix_1 = glm::mat4(1.0f);
    g_model_matrix_1 = glm::mat4(1.0f);
    //g_projection_matrix_1 = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    // Orthographic means perpendicular—meaning that our camera 
    // will be looking perpendicularly down to our triangle

    //set our matricies 
    g_program.set_view_matrix(g_view_matrix);
    g_program.set_projection_matrix(g_projection_matrix);

    //g_program_1.set_view_matrix(g_view_matrix_1);
    //g_program_1.set_projection_matrix(g_projection_matrix_1);


    glUseProgram(g_program.get_program_id());
    //glUseProgram(g_program_1.get_program_id());
    //cout << g_program.get_program_id

    g_player_texture_id = load_texture(PLAYER_SPRITE);
    g_player_texture_id_1 = load_texture(PLAYER_1_SPRITE);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
}

double velocity = 0.0005; 
double acceleration = -0.00001; //this is my gravity? looks like a staircase though

const float GROWTH_FACTOR = 1.0002f;  // growth rate of 1.0% per frame
const float SHRINK_FACTOR = 0.9998f;  // growth rate of -1.0% per frame
const int MAX_FRAME = 40;           // this value is, of course, up to you
bool grow = true;


const float MILLISECONDS_IN_SECOND = 1000.0;
float prev_ticks = 0;
float checkpt = 0;

float bg_checkpt = 0;
bool original_bg = true;

double velocity_1 = 0.01;
double angle = 90.0f; 
float g_triangle_rotate;


void update() {
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;  // get the current number of ticks
    float delta_time = ticks - prev_ticks;     // the delta time is the difference from the last frame
    prev_ticks = ticks;


    glm::vec3 trans_vector;
    glm::vec3 scale_vector;
    glm::vec3 trans_vector_1;
    glm::vec3 rotate_vector_1;

    velocity += acceleration * delta_time;

    
    if (ticks - checkpt >= 2.00) {
        acceleration *= -1.00; // Toggle acceleration direction
        checkpt = ticks; // Reset frame count

        grow = !grow;
    }

    trans_vector = glm::vec3(0.0005f, velocity, 0.0f);
    scale_vector = glm::vec3(grow ? GROWTH_FACTOR : SHRINK_FACTOR,
                               grow ? GROWTH_FACTOR : SHRINK_FACTOR,
                            1.0f);

    velocity_1 += velocity_1 * delta_time;
    trans_vector_1 = glm::vec3(-velocity_1, -velocity_1, -1.0f);
    rotate_vector_1 = glm::vec3(0.0f, 0.0f, 1.0f);

   
    g_model_matrix = glm::translate(g_model_matrix, trans_vector);
    g_model_matrix = glm::scale(g_model_matrix, scale_vector);

    g_triangle_rotate += angle * delta_time;


    g_model_matrix_1 = glm::translate(g_model_matrix, trans_vector_1);
    g_model_matrix_1 = glm::rotate(g_model_matrix_1, glm::radians(g_triangle_rotate), rotate_vector_1);

    if (ticks - bg_checkpt >= 1.50) {
        original_bg = !original_bg;
        if (original_bg) {
            glClearColor(BG_RED_1, BG_GREEN_1, BG_BLUE_1, BG_OPACITY_1);
        }
        else {
            glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
        }
        bg_checkpt = ticks;
    }

}

void processinput() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

void render() {
    // Step 1
    glClear(GL_COLOR_BUFFER_BIT);

    // Step 2
    g_program.set_model_matrix(g_model_matrix);
    
    // 
    //set model matrix here since it is basically everything done to the object
    //need this to be updated every frame 

    // Step 3
    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,   // triangle 2
    };

    glVertexAttribPointer(g_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_program.get_position_attribute());


    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f     // triangle 2
    };

    glVertexAttribPointer(g_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_program.get_tex_coordinate_attribute());

    //bind texture 
    glBindTexture(GL_TEXTURE_2D, g_player_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(g_program.get_position_attribute());
    glDisableVertexAttribArray(g_program.get_tex_coordinate_attribute());





    g_program.set_model_matrix(g_model_matrix_1);
    
    float vertices_1[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,   // triangle 2
    };

    glVertexAttribPointer(g_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices_1);
    glEnableVertexAttribArray(g_program.get_position_attribute());


    // Textures
    float texture_coordinates_1[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f     // triangle 2
    };

    glVertexAttribPointer(g_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates_1);
    glEnableVertexAttribArray(g_program.get_tex_coordinate_attribute());


    
    glBindTexture(GL_TEXTURE_2D, g_player_texture_id_1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(g_program.get_position_attribute());
    glDisableVertexAttribArray(g_program.get_tex_coordinate_attribute());

    // Step 4
    SDL_GL_SwapWindow(displayWindow);
}

void shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {

    initialize();

    
    while (gameIsRunning) {
        processinput();

        update();

        render();
    }

    shutdown();
    return 0;
}
