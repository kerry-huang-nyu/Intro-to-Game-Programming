/**
* Author: [Kerry Huang]
* Assignment: Lunar Lander
* Date due: 2024-03-9, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

/*
Requirements
1. Player falls with gravity:           DONE         
    a. create entity 
    b. gravity matrix 
    c. falls under gravity 
2. Movement with acceleration:          DONE 
    a. < > moves orientation 
    b. ^ moves thrusters 
    c. get movement 
3. Mission sucess/fail                  
    a. create/draw platform entities 
    b. detect collisions 
    c. display words 
4. Extra 
    a. fuel track 
    b. fuel display 

*/

#include <vector>

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

#include "Entity.h"

#include <iomanip>
#include <sstream>

SDL_Window* displayWindow;
bool gameIsRunning = true;
int collisionStatus = 0;

//background colors 
const float BG_RED = 0.03f,
    BG_GREEN = 0.05f,
    BG_BLUE = 0.13f,
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

const char ROCKET_SPRITE[] = "rocket.png";

const char BLACK_HOLE_SPRITE[] = "black hole.png";
const char EARTH_SPRITE[] = "earth.png";
const char SUN_SPRITE[] = "sun.png";
const char SATURN_SPRITE[] = "saturn.png";

const char FONT_SPRITE[] = "font1.png";


//game state struct 
struct GameState{
    float gravity = -0.25;

    int NUM_OBSTACLES = 3;

    Entity* rocket;
    std::vector<Entity*> obstacles;
    Entity* target;
};

GameState g_state; //game state object to access our players 

std::ofstream logme;
GLint font_texture_id;

//shader program and matricies 
glm::mat4 view_matrix; //position + orientation of camera 
glm::mat4 projection_matrix; //flip panes and field of view and projection of camera 

//shader 
ShaderProgram g_program;



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
    displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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

    // player initializations 
    g_state.rocket = new Entity();
    g_state.rocket->m_texture_id = load_texture(ROCKET_SPRITE);

    std::vector<const char*> sprites = {BLACK_HOLE_SPRITE, SUN_SPRITE, SATURN_SPRITE};

    for (int i = 0; i < g_state.NUM_OBSTACLES; i++) {
        Entity* temp = new Entity(); //create a new Entity 
        g_state.obstacles.push_back(temp);
        g_state.obstacles[i]->m_texture_id = load_texture(sprites[i]);
    }

    //earth initializations 
    g_state.target = new Entity();
    g_state.target->m_texture_id = load_texture(EARTH_SPRITE);

    font_texture_id = load_texture(FONT_SPRITE); //then load font as well 


    //set our matricies 
    g_program.set_view_matrix(view_matrix);
    g_program.set_projection_matrix(projection_matrix);
    

    glUseProgram(g_program.get_program_id());


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
    //initialize the locations, gravity, and radius 

    //rocket 
    g_state.rocket->gravity = g_state.gravity;
    g_state.rocket->radius = 0.3;
    glm::vec3 m_position = glm::vec3(0);
    //my location should be the bottom 
    m_position.x = 3;
    m_position.y = -1;
    g_state.rocket->m_position = m_position;
    g_state.rocket->m_model_matrix = glm::translate(g_state.rocket->m_model_matrix, m_position);


    //obstacles 
    std::vector<float> radii = {0.5, 0.5, 0.4}; //for blackhole, sun , saturn
    std::vector<double> x_loc = {3, 2.5, -4};
    std::vector<double> y_loc = {-3, 2.76, -3};

    for (int i = 0; i < g_state.NUM_OBSTACLES; i++) {
        g_state.obstacles[i]->gravity = 0;
        g_state.obstacles[i]->radius = radii[i];
        m_position.x = x_loc[i];
        m_position.y = y_loc[i];
        
        g_state.obstacles[i]->m_position = m_position;
        g_state.obstacles[i]->m_model_matrix = glm::translate(g_state.obstacles[i]->m_model_matrix, m_position);
    }

    //my location should be the bottom 
    g_state.target->gravity = 0;
    g_state.target->radius = 0.7;
    m_position.x = 0;
    m_position.y = -3.75;
    g_state.target->m_position = m_position;
    g_state.target->m_model_matrix = glm::translate(g_state.target->m_model_matrix, m_position);
}



bool collide(Entity* ent1, Entity* ent2) { 
    //location of the 2 values which are colliding
    float x_distance = fabs(ent1->m_position.x - ent2->m_position.x);
    float y_distance = fabs(ent1->m_position.y - ent2->m_position.y);

    float diagonal = sqrt(x_distance * x_distance + y_distance * y_distance);

    if (diagonal < (ent1->radius + ent2->radius)) {
        return true;
    }

    return false;
}



void update() {
    


    //collision detection 
    for (int i = 0; i < g_state.NUM_OBSTACLES; i++) {
        if (collide(g_state.rocket, g_state.obstacles[i])) {
            collisionStatus = 1; //1 meaning error 
        }
    }

    if (collide(g_state.rocket, g_state.target)) {
        collisionStatus = 1;
        //logme << " this is my velocity " << g_state.rocket->getVelocity() << "this is my upright" << g_state.rocket->upright() << std::endl;

        if (g_state.rocket->getVelocity() < 0.40 && g_state.rocket->upright()) { //if you collide with the right orientation and the right speed 
            collisionStatus = 2;
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
        return;
    }
    while (delta_time >= FIXED_TIMESTEP) {
        //make updates to the thing 

        //ensure that colisions are detected 
        g_state.rocket->Update(FIXED_TIMESTEP);
        

        delta_time -= FIXED_TIMESTEP;
    }
    time_accumulate = delta_time;

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

        g_state.rocket->StopTurn(); //stops turning otherwise 
        g_state.rocket->StopThrust();

        const Uint8* key_state = SDL_GetKeyboardState(NULL);

        if (key_state[SDL_SCANCODE_LEFT]){
            g_state.rocket->StartTurn(true);
        }

        if (key_state[SDL_SCANCODE_RIGHT]){
            g_state.rocket->StartTurn(false);
        }

        if (key_state[SDL_SCANCODE_UP]){
            g_state.rocket->StartThrust();
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

    renderobj(vertices, texture_coordinates, g_state.rocket->m_model_matrix, g_state.rocket->m_texture_id);
    for (int i = 0; i < g_state.NUM_OBSTACLES; i++) {
        renderobj(vertices, texture_coordinates, 
        g_state.obstacles[i]->m_model_matrix, g_state.obstacles[i]->m_texture_id);
    }
    renderobj(vertices, texture_coordinates, g_state.target->m_model_matrix, g_state.target->m_texture_id);

    //display the amount of fuel left 
    glm::vec3 position = { -4.7,3.3,0.0 };
    std::string fuel_status = "Fuel Left: ";
    fuel_status += std::to_string(int(g_state.rocket->fuel));
    draw_text(&g_program, font_texture_id, fuel_status, 0.33, 0, position);



    fuel_status = "X Velocity: ";
    position = { -4.5,2.9,0.0 }; 
    fuel_status += std::to_string(g_state.rocket->m_velocity.x * 10);
    fuel_status = fuel_status.substr(0, fuel_status.find('.') + 3);
    draw_text(&g_program, font_texture_id, fuel_status, 0.33, 0, position);


    //this is the dumbest concatenanation issue i encountered. still dont' know how to properly display but whatever
    fuel_status = "Y Velocity: ";
    position = { -4.5,2.5,0.0 };
    fuel_status += std::to_string(g_state.rocket->m_velocity.y * 10);
    fuel_status = fuel_status.substr(0, fuel_status.find('.') + 3);
    draw_text(&g_program, font_texture_id, fuel_status, 0.33, 0, position);

    if (collisionStatus == 2) {
        position = { -2.5,0.0,0.0 };
        draw_text(&g_program, font_texture_id, "ET goes home :D", 0.33, 0, position);
    }
    if (collisionStatus == 1) {
        position = { -4,0.0,0.0 };
        draw_text(&g_program, font_texture_id, "You have died of Dysentery", 0.33, 0, position);
    }

    // Step 4
    SDL_GL_SwapWindow(displayWindow);
}

void shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {

    logme.open("log.txt");

    initialize();

    sprite_init();
    
    while (gameIsRunning) {
        processinput();

        if (collisionStatus == 0) {
            update();

            render();
        }
    }

    logme.close();

    shutdown();
    return 0;
}
