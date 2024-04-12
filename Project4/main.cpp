/**
* Author: Kerry Huang
* Assignment: Rise of the AI
* Date due: 2024-03-23, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

/*
Revamp entity class:
    a. collision detection
    b. pitfall detection
    c. different types
    d. platform nudging
    e. update function in entity


AI with different behaviors:
    1. 1 only walks around 
    2. 1 flys diagonally 
    3. 1 follows/guards 
New sound + sprites 
    1. import new sound 
Enemy defeat:
    1. defeat individual enemies 
Display Message:
    1. display different messages 
Extra Credit: 
    1. Another way to die 


Extra Stuff:
    1. animation???
    2. attack sounds??? 
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
#include <SDL_mixer.h>

#define LEVEL1_WIDTH 16
#define LEVEL1_HEIGHT 12

#include "Map.h"


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

//compact disk quality frequency
const int   CD_QUAL_FREQ = 44100,  
            AUDIO_CHAN_AMT = 2,
            AUDIO_BUFF_SIZE = 4096;

//shaders 
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";     // make sure not to use std::string objects for these!
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const char PLAYER_SPRITE[] = "protagonist.png";
const char ENEMY_SPRITE[] = "wizard.png";
const char MAP_TILESET_FILEPATH[] = "grass.png";

const char FONT_SPRITE[] = "font1.png";

const char BGM_FILEPATH[] = "sweetdreams.mp3";
const int    LOOP_FOREVER = -1;  // -1 means loop forever in Mix_PlayMusic; 0 means play once and loop zero times

Mix_Music* g_music;


//game state struct 
struct GameState{
    int NUM_PLATFORMS = 3;
    int NUM_ENEMIES = 3;
    int enemies_defeated = 0;
    double gravity = -7;

    bool endgame = false;
    float* nightfall = new float(0);
    float* night_level = new float(0);

    Entity* player;

    Map* map;

    std::vector<Entity*> platforms;
    std::vector<Entity*> enemies;
};

unsigned int LEVEL_1_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    97, 98, 98, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 97, 98, 98, 99, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0,
    0, 0, 0, 0, 0, 0, 97, 98, 98, 99, 0, 0, 100, 0, 0, 0,
    0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 97, 98, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

GameState g_state; //game state object to access our players 


std::ofstream logme;
GLint font_texture_id;

//shader program and matricies 
glm::mat4 view_matrix, projection_matrix; //flip panes and field of view and projection of camera 

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


    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO); //initializing both video and audio  
    
    //Start Audio 
    Mix_OpenAudio(
        CD_QUAL_FREQ,   //frequency to playback audio at Hz
        MIX_DEFAULT_FORMAT,     //audio format 
        AUDIO_CHAN_AMT,         //channels. mono stero ...
        AUDIO_BUFF_SIZE         //audio buffer size in sample frames 
    );

    // Similar to our custom function load_texture
    g_music = Mix_LoadMUS(BGM_FILEPATH);

    // This will schedule the music object to begin mixing for playback.
    // The first parameter is the pointer to the mp3 we loaded 
    // and second parameter is the number of times to loop.
    Mix_PlayMusic(g_music, LOOP_FOREVER);

    // Set the music to half volume
    Mix_VolumeMusic(0); //MIX_MAX_VOLUME / 2



    displayWindow = SDL_CreateWindow("Rise of AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    // STEP 2: ...translate it to the NEGATIVE x-position of whatever you want the camera to follow
    view_matrix = glm::translate(view_matrix, glm::vec3(-5.0, 3.75, 0.0f));


    //for entities. reset to the initial position and then translate into the desired position. 
    //modle matrix = translate into initial position 
    //
    float zoomout = 1.75;
    float xmove = 2.0f;
    float ymove = -1.0f;
    
    projection_matrix = glm::ortho(-5.0f * zoomout + xmove, 5.0f * zoomout + xmove, -3.75f * zoomout + ymove, 3.75f * zoomout + ymove, -1.0f, 1.0f);  
    //model_matrix = glm::mat4(1.0f);

    //map initializations: 
    GLuint map_texture_id = load_texture(MAP_TILESET_FILEPATH);
    g_state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, LEVEL_1_DATA, map_texture_id, 1.0f, 24, 8);  // Look at this beautiful initialisation. That's literally it
    //g_state.map->normalize(10, 7.5); //normalize with the new coordinates of the view 
    //g_state.map->normalize(0, 0); //normalize with the new coordinates of the view 

    // player initializations 
    g_state.player = new Entity();
    g_state.player->set_position(glm::vec3(0.0f));
    g_state.player->set_movement(glm::vec3(0.0f));
    g_state.player->set_gravity(g_state.gravity);
    //g_state.player->set_gravity(0); //added this just to debug please delete 
    g_state.player->m_height = 1.0f;
    g_state.player->m_width = 0.5f;

    g_state.player->m_speed = 1.5f;
    g_state.player->m_texture_id = load_texture(PLAYER_SPRITE);
    g_state.player->m_entity_type = PLAYER;
    
    g_state.player->m_walking[g_state.player->LEFT] = new int[4] { 95, 94, 93, 92 };
    g_state.player->m_walking[g_state.player->RIGHT] = new int[4] { 16, 17, 18, 19 };
    g_state.player->m_walking[g_state.player->UP] = new int[4] { 16, 17, 16, 17 };
    g_state.player->m_walking[g_state.player->DOWN] = new int[4] { 0, 0, 0, 0 };
    g_state.player->m_walking[g_state.player->IDLE] = new int[4] { 0, 0, 0, 1 };

    g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->IDLE];  // start player looking down
    g_state.player->m_animation_frames = 4;
    g_state.player->m_animation_index = 0;
    g_state.player->m_animation_time = 0.0f;
    g_state.player->m_animation_cols = 8;
    g_state.player->m_animation_rows = 18;

    g_state.player->night_level = g_state.night_level;
    g_state.player->nightfall = g_state.nightfall;


    //create the enemies
    std::vector<glm::vec3> enemy_initial_pos = {glm::vec3(1.0f, -2.0f, 0), glm::vec3(8.0f, 0.0f, 0), glm::vec3(13.0f, 5.0f, 0)};
    std::vector<AIType> enemy_ai_type = {SORCERER, JUMPER, WALKER};
    

    for (int i = 0; i < g_state.NUM_ENEMIES; i++) {
        Entity* enemy = new Entity();

        g_state.enemies.push_back(enemy);
        enemy->set_position(enemy_initial_pos[i]);
        enemy->set_movement(glm::vec3(0.0f)); //may not even be needed check 

        enemy->set_gravity(g_state.gravity);
        enemy->m_height = 1.0f;
        enemy->m_width = 0.5f;
        enemy->m_entity_type = ENEMY;
        enemy->m_ai_type = enemy_ai_type[i];

        enemy->m_speed = 1.5f;
        enemy->m_texture_id = load_texture(ENEMY_SPRITE);

        enemy->m_walking[enemy->LEFT] = new int[4] { 99, 98, 97, 96 };
        enemy->m_walking[enemy->RIGHT] = new int[4] { 10, 11, 12, 13 };
        enemy->m_walking[enemy->UP] = new int[4] { 10, 11, 10, 11 };
        enemy->m_walking[enemy->DOWN] = new int[4] { 57, 56, 55, 54 };
        enemy->m_walking[enemy->IDLE] = new int[4] { 0, 0, 0, 1 };

        enemy->m_animation_indices = enemy->m_walking[enemy->IDLE];  // start enemy looking down
        enemy->m_animation_frames = 4;
        enemy->m_animation_index = 0;
        enemy->m_animation_time = 0.0f;
        enemy->m_animation_cols = 10;
        enemy->m_animation_rows = 16;

        enemy->night_level = g_state.night_level;
        enemy->nightfall = g_state.nightfall;
    }


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


bool collide(Entity* ent1, Entity* ent2) { 
    //location of the 2 values which are colliding

    /*
    float x_distance = fabs(ent1->m_position.x - ent2->m_position.x);
    float y_distance = fabs(ent1->m_position.y - ent2->m_position.y);

    float diagonal = sqrt(x_distance * x_distance + y_distance * y_distance);

    if (diagonal < (ent1->radius + ent2->radius)) {
        return true;
    }*/

    return false;
}



void update() {
    //movement 
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;  // get the current number of ticks
    float delta_time = ticks - prev_ticks;     // the delta time is the difference from the last frame
    prev_ticks = ticks;

    *(g_state.nightfall) += delta_time;

    logme << *(g_state.nightfall) << " lol\n";


    //create a fixed timestep 
    delta_time += time_accumulate; //time accumulated in total 

    if (delta_time < FIXED_TIMESTEP) {
        time_accumulate = delta_time;
        return;
    }
    while (delta_time >= FIXED_TIMESTEP) {
        //make updates to the thing 

        //ensure that collisions are detected 
        g_state.player->update(FIXED_TIMESTEP, g_state.player, g_state.enemies, g_state.map, logme);

        g_state.enemies_defeated = 0;
        for (int i = 0; i < g_state.NUM_ENEMIES; i++) {
            if (g_state.enemies[i]->get_activity() == DEAD)
                g_state.enemies_defeated++;
            if (g_state.enemies_defeated == g_state.NUM_ENEMIES) {
                g_state.endgame = true;
            }
            g_state.enemies[i]->update(FIXED_TIMESTEP, g_state.player, g_state.enemies, g_state.map, logme);
        }

        delta_time -= FIXED_TIMESTEP;
    }
    time_accumulate = delta_time;

    //logme << " enemies_defeated " << g_state.enemies_defeated << "\n";

    //view_matrix = glm::mat4(1.0f);
    //view_matrix = glm::translate(view_matrix, glm::vec3(-g_state.player->get_position().x, -g_state.player->get_position().y, 0.0f));

    if (*(g_state.night_level) > 0) {
        BG_RED -= *(g_state.night_level);
        BG_GREEN -= *(g_state.night_level);
        BG_BLUE -= *(g_state.night_level);
        *(g_state.night_level) = 0;
    }

    if (BG_BLUE <= 0) {
        g_state.player->set_activity(DEAD);
    }

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

        g_state.player->set_direction(g_state.player->IDLE); //how to put this into the code instead of here

        if (key_state[SDL_SCANCODE_LEFT]) {
            g_state.player->set_direction(g_state.player->LEFT);
        }

        if (key_state[SDL_SCANCODE_RIGHT]){
            g_state.player->set_direction(g_state.player->RIGHT);
        }

        if (key_state[SDL_SCANCODE_UP]){
            g_state.player->set_direction(g_state.player->UP);
        }
    }
}

void render() {
    // Step 1
    glClear(GL_COLOR_BUFFER_BIT);

    g_state.map->render(&g_program, 0, 0); //10 and 7.5 is where its at 

    g_state.player->render(&g_program);

    for (int i = 0; i < g_state.NUM_ENEMIES; i++) {
        g_state.enemies[i]->render(&g_program);
    }

    
    glm::vec3 position;

    if (*(g_state.nightfall) < 5) {
        position = { 2.5,-4, 0.0 };
        draw_text(&g_program, font_texture_id, "Defeat the Wizards", 0.5, 0, position);
        position = { 3,-5, 0.0 };
        draw_text(&g_program, font_texture_id, "before Nightfall", 0.5, 0, position);
    }

    if (g_state.player->get_activity() == DEAD) {
        position = { 2.0,-4, 0.0 };
        draw_text(&g_program, font_texture_id, "Failed the Quest", 0.75, 0, position);
        //glm::translate(g_state.rocket->m_model_matrix, m_position);
    }
    else if (g_state.enemies_defeated == g_state.NUM_ENEMIES) {
        position = { 3.5,-4.0, 0.0 };
        draw_text(&g_program, font_texture_id, "Sucess :)", 0.75, 0, position);
    }

    else if (BG_BLUE < 0.5 && BG_BLUE > 0.4) {
        position = { 2,-4.0, 0.0 };
        draw_text(&g_program, font_texture_id, "Crippled by Nightfall", 0.5, 0, position);
    }


    // Step 4
    SDL_GL_SwapWindow(displayWindow);
}

void shutdown() {
    delete g_state.map;
    delete g_state.nightfall;
    Mix_FreeMusic(g_music);
    SDL_Quit();
}

int main(int argc, char* argv[]) {

    logme.open("log.txt");

    initialize();
    
    while (gameIsRunning) {
        processinput();

        if (!g_state.endgame) {
            update();
            render();
        }
    }

    logme.close();

    shutdown();
    return 0;
}
