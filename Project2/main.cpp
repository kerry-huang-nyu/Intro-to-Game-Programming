/**
* Author: [Kerry Huang]
* Assignment: Pong Clone!
* Date due: 2024-03-2, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

/*
Requirements
1. Paddles / Players (50%)                  DONE
    a. independent paddle movement 
    b. paddle constrained by walls 
    c. ball bounces off paddles 
    d. t key switches behavior 
    e. ball movement normalized 
2. Bounces Off Walls (25%)                  DONE
    a. ball bounces off walls 
3. Game Over (25%)                          DONE
    a. winning/losing conditions 

extra:
1. Give a theme                             DONE 
    a. import coquette theme and textures 
2. Endgame UI Message                       DONE 
    a. show the ending text 
3. Choose the number of balls               
    a. generate balls but no render 

*/

#include <vector>
#include <random>

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

#include <fstream> //just to output???

SDL_Window* displayWindow;
bool gameIsRunning = true;

std::random_device rd;
std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()

// Define a distribution (for example, uniform distribution between 1 and 100)
std::uniform_real_distribution<> dis(-1.0, 1.0);


//background colors 
const float BG_RED = 1.0f,
    BG_GREEN = 0.8f,
    BG_BLUE = 0.8f,
    BG_OPACITY =1.0f;


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

const char PLAYER_SPRITE[] = "donut.png";
const char PADDLE_SPRITE[] = "coquette wallpaper.jpg";
const char FONT_SPRITE[] = "font1.png";

//number of balls we have currently vs total we can have 
const int NUMBALLS = 3;
int currBalls = 1; 

std::ofstream logme;

GLuint ball_texture_id[NUMBALLS]; //asdf

GLuint lpaddle_texture_id, rpaddle_texture_id, font_texture_id;
//GLuint leftpaddle_texture_id;

//shader program and matricies 
glm::mat4 view_matrix; //position + orientation of camera 
glm::mat4 projection_matrix; //flip panes and field of view and projection of camera 

//ball's shader 
ShaderProgram g_program;
//glm::mat4 ball_model_matrix; // Defines every translation, rotation, and/or scaling applied to an object; we'll look at these next week

glm::mat4 ball_model_matrix[NUMBALLS]; //

//lpaddle's shader 
glm::mat4 lpaddle_model_matrix; // Defines every translation, rotation, and/or scaling applied to an object; we'll look at these next week

//rpaddle's shader
glm::mat4 rpaddle_model_matrix; 

const double PADDLE_WIDTH = 0.25;
const double PADDLE_LENGTH = 1;

int winner = 0;

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
    displayWindow = SDL_CreateWindow("Pong :3", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_HEIGHT, VIEWPORT_HEIGHT);  //create camera    // Initialise our camera
    g_program.load(V_SHADER_PATH, F_SHADER_PATH); //create shaders 
    //g_program_1.load(V_SHADER_PATH, F_SHADER_PATH); //create shaders

    // Initialise our view, model, and projection matrices
    view_matrix = glm::mat4(1.0f);
    projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  

    //object's matricies 
    //ball_model_matrix = glm::mat4(1.0f);
    for (int i = 0; i < NUMBALLS; ++i) { //asdk 
        ball_model_matrix[i] = glm::mat4(1.0f);
    }

    lpaddle_model_matrix = glm::mat4(1.0f);
    rpaddle_model_matrix = glm::mat4(1.0f);



    //set our matricies 
    g_program.set_view_matrix(view_matrix);
    g_program.set_projection_matrix(projection_matrix);
    

    glUseProgram(g_program.get_program_id());
    //cout << g_program.get_program_id


    //ball_texture_id = load_texture(PLAYER_SPRITE);
    for (int i = 0; i < NUMBALLS; ++i) {
        ball_texture_id[i] = load_texture(PLAYER_SPRITE);
    }


    lpaddle_texture_id = load_texture(PADDLE_SPRITE);
    rpaddle_texture_id = load_texture(PADDLE_SPRITE);
    font_texture_id = load_texture(FONT_SPRITE);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);
}


const float MILLISECONDS_IN_SECOND = 1000.0;
float prev_ticks = 0;

float left_paddle_speed = 1;
float right_paddle_speed = -1;
const float PADDLE_SPEED_LMT = 3;


glm::vec3 left_trans_vector = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 right_trans_vector = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 ball_trans_vector = glm::vec3(0.0f, 0.0f, 0.0f); 

glm::vec3 paddle_scale = glm::vec3(0.25f, 1.0f, 1.0f);


bool single_mode = false; 

//for top and bottom wall thinking of them as boxes 

double wallwidth = 5 * 2;
double wallheight = 3.75 * 2;


double padwidth = 0.25;
double padlength = 1;

double ballwidth = 1;

//object's location and width information 
double lwall[4] = { -wallwidth/2, 0, 0, wallheight}; 
double rwall[4] = { wallwidth/2, 0, 0, wallheight };

double twall[4] = { 0, wallheight/2, wallwidth, 0 };
double bwall[4] = { 0, -wallheight/2, wallwidth, 0 };

double lpad[4] = { 0, 0, padwidth, padlength};
double rpad[4] = { 0, 0, padwidth, padlength};

double ball[3][4];

double ball_speed_x[NUMBALLS];
double ball_speed_y[NUMBALLS];

const int FONTBANK_SIZE = 16;

void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairs—one for each character
    // Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their position
        //    relative to the whole sentence)
        int spritesheet_index = (int)text[i];  // ascii value of character
        float offset = (screen_size + spacing) * i;

        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
            });
    }

    // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    program->set_model_matrix(model_matrix);
    glUseProgram(program->get_program_id());

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(g_program.get_position_attribute());
    glVertexAttribPointer(g_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(g_program.get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(g_program.get_position_attribute());
    glDisableVertexAttribArray(g_program.get_tex_coordinate_attribute());
}


void sprite_init() { 
    //first call and translate paddles to the old position and then continue to translate 
    lpaddle_model_matrix = glm::scale(lpaddle_model_matrix, paddle_scale);
    rpaddle_model_matrix = glm::scale(rpaddle_model_matrix, paddle_scale);

    lpaddle_model_matrix[3][0] = -5 + padwidth/2; // this is how to update the x position of the paddels at the start
    rpaddle_model_matrix[3][0] = 5 - padwidth/2;

    //lpaddle_model_matrix = glm::translate(lpaddle_model_matrix, glm::vec3(leftedge, 0, 0));
    //rpaddle_model_matrix = glm::translate(rpaddle_model_matrix, glm::vec3(rightedge, 0, 0));


    for (int i = 0; i < NUMBALLS; i++) { 
        //every ball spawns at the center with possibly a different direction 
        ball[i][0] = 0;
        ball[i][1] = 0;
        ball[i][2] = ballwidth;
        ball[i][3] = ballwidth;


        ball_speed_x[i] = dis(gen);
        ball_speed_y[i] = dis(gen);

    }
}



void getLoc() {
    for (int i = 0; i < NUMBALLS; i++) {
        ball[i][0] = ball_model_matrix[i][3][0];
        ball[i][1] = ball_model_matrix[i][3][1];
    }
    
    lpad[0] = lpaddle_model_matrix[3][0];
    lpad[1] = lpaddle_model_matrix[3][1];

    rpad[0] = rpaddle_model_matrix[3][0];
    rpad[1] = rpaddle_model_matrix[3][1];

}

bool collide(double obj1[], double obj2[]) { //location of the 2 values which are colliding
    float x_distance = fabs(obj1[0] - obj2[0]) - ((obj1[2] + obj2[2]) / 2.0f);
    float y_distance = fabs(obj1[1] - obj2[1]) - ((obj1[3] + obj2[3]) / 2.0f);

    if (x_distance < 0 && y_distance < 0)
    {
        return true;
    }

    //logme << x_distance << "    " << y_distance << " abs component " << fabs(obj1[1] - obj2[1]) << " other component    " << ((obj1[3] + obj2[3]) / 2.0f) << "\n";

    return false;
}



void update() {
    getLoc(); //get location of my paddles and also the ball 


    //collision detection 
    for (int i = 0; i < currBalls; i++) {

        if (collide(ball[i], lwall) == true) { //game over conditions 
            winner = 2; //2 indicates the right
        }

        if (collide(ball[i], rwall) == true) { //game over conditions 
            winner = 1; //1 indicates the left 
        }

        //reverse the velocity of x when encounter obstacle while traveling left 
        if (collide(ball[i], lpad) == true) { //bounce off left 
            ball_speed_x[i] *= -1;

            ball_speed_x[i] *= 1.001;
            ball_speed_y[i] *= 1.001;
        }
        if (collide(ball[i], rpad) == true) { //bounce off right 
            ball_speed_x[i] *= -1;

            ball_speed_x[i] *= 1.001;
            ball_speed_y[i] *= 1.001;
        }

        //reverse the velocity of y when encounter obstacle while traveling up 
        if (collide(ball[i], twall) == true) { //bounce off left 
            ball_speed_y[i] *= -1;
        }
        if (collide(ball[i], bwall) == true) { //bounce off right 
            ball_speed_y[i] *= -1;
        }

    }

    //jamming with walls 
    if (collide(lpad, twall) == true) {
        left_paddle_speed = -1;
    }
    if (collide(rpad, twall) == true) {
        right_paddle_speed = -1;
    }

    if (collide(lpad, bwall) == true) {
        left_paddle_speed = 1;
    }

    if (collide(rpad, bwall) == true) {
        right_paddle_speed = 1;
    }

    //movement 
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;  // get the current number of ticks
    float delta_time = ticks - prev_ticks;     // the delta time is the difference from the last frame
    prev_ticks = ticks;

    //movement * speed * deltatime 

    //update the ball first before updating the paddle 
    for (int i = 0; i < currBalls; ++i) { //use the same space but then it is not used anywhere else so its fine 
        ball_trans_vector.x = ball_speed_x[i] * delta_time;
        ball_trans_vector.y = ball_speed_y[i] * delta_time;

        if (glm::length(ball_trans_vector) > 1.0f) {
            ball_trans_vector = glm::normalize(ball_trans_vector);
        }
        ball_model_matrix[i] = glm::translate(ball_model_matrix[i], ball_trans_vector);

    }


    //update the paddle under certain conditions 
    if (single_mode) { //under single mode left paddle is tethered to the ball 

        //always follow the ball that is going left and closest to the left 
        int followme = 0;
        for (int i = 0; i < currBalls; ++i) { //for the balls currently on screen 
            if (ball_model_matrix[i][3][0] < ball_model_matrix[followme][3][0] && ball_speed_x[i] < 0) {
                followme = i;
            }
        }

        if (ball[followme][1] > lpad[1]) {
            left_paddle_speed = 1;
        }
        else if (ball[followme][1] < lpad[1]) {
            left_paddle_speed = -1;
        }
    }

    left_trans_vector.y = left_paddle_speed * delta_time;
    right_trans_vector.y = right_paddle_speed * delta_time;

    lpaddle_model_matrix = glm::translate(lpaddle_model_matrix, left_trans_vector);
    rpaddle_model_matrix = glm::translate(rpaddle_model_matrix, right_trans_vector);

}



void processinput() {
    SDL_Event event;


    while (SDL_PollEvent(&event)) {

        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_t:
                    single_mode = !single_mode;
                }
        }

        left_paddle_speed = 0;
        right_paddle_speed = 0;

        const Uint8* key_state = SDL_GetKeyboardState(NULL);

        if (!single_mode) { //only consider if we are playing two player 
            if (key_state[SDL_SCANCODE_W]) {
                left_paddle_speed = PADDLE_SPEED_LMT;
            }

            if (key_state[SDL_SCANCODE_S])
            {
                left_paddle_speed = -PADDLE_SPEED_LMT;
            }
        }

        if (key_state[SDL_SCANCODE_UP]) {
            right_paddle_speed = PADDLE_SPEED_LMT;
        }

        if (key_state[SDL_SCANCODE_DOWN])
        {
            right_paddle_speed = -PADDLE_SPEED_LMT;
        }

        //checks if you want 1 2 or 3 balls on screen 
        if (key_state[SDL_SCANCODE_1]) {
            currBalls = 1;
        }

        if (key_state[SDL_SCANCODE_2]) {
            currBalls = 2;
        }

        if (key_state[SDL_SCANCODE_3]) {
            currBalls = 3;
        }
    }
}

void renderobj(float vertices[], float texture[], glm::mat4& model_matrix, GLuint id) {
    g_program.set_model_matrix(model_matrix);
    glVertexAttribPointer(g_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_program.get_position_attribute());

    glVertexAttribPointer(g_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture);
    glEnableVertexAttribArray(g_program.get_tex_coordinate_attribute());

    //bind texture 
    glBindTexture(GL_TEXTURE_2D, id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(g_program.get_position_attribute());
    glDisableVertexAttribArray(g_program.get_tex_coordinate_attribute());

}

void render() {
    // Step 1
    glClear(GL_COLOR_BUFFER_BIT);

    // Step 2
  

    // Step 3
    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,   // triangle 2
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f     // triangle 2
    };

    for (int i = 0; i < currBalls; ++i) {
        //asldkfsa; ldfkjas;ldfjsldf
        renderobj(vertices, texture_coordinates, ball_model_matrix[i], ball_texture_id[i]);
    }

    renderobj(vertices, texture_coordinates, lpaddle_model_matrix, lpaddle_texture_id);
    renderobj(vertices, texture_coordinates, rpaddle_model_matrix, rpaddle_texture_id);

    if (winner == 2) {
        glm::vec3 position = { -3,0.0,0.0 };
        draw_text(&g_program, font_texture_id, "Right Player Won :3", 0.33, 0, position);
    }
    if (winner == 1) {
        glm::vec3 position = { -3,0.0,0.0 };
        draw_text(&g_program, font_texture_id, "Left Player Won :3", 0.33, 0, position);
    }

    // Step 4
    SDL_GL_SwapWindow(displayWindow);
}

void shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {

    //std::ofstream log;
    logme.open("log.txt");

    initialize();

    sprite_init();
    
    while (gameIsRunning) {
        processinput();

        if (winner == 0) { //when the winner hasn't been decided on yet 
            update();

            render();
        }
        
    }

    logme.close();

    shutdown();
    return 0;
}
