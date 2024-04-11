// Level1.cpp
#include "Level1.h"
#include "Utility.h"

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 12


//compact disk quality frequency
const int   CD_QUAL_FREQ = 44100,
AUDIO_CHAN_AMT = 2,
AUDIO_BUFF_SIZE = 4096;

const int    LOOP_FOREVER = -1;  // -1 means loop forever in Mix_PlayMusic; 0 means play once and loop zero times

unsigned int LEVEL_DATA[] =
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

Level1::~Level1()
{
    delete    m_state.player;
    delete    m_state.map;

    //Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void Level1::initialise()
{

    //map initializations: 
    GLuint map_texture_id = Utility::load_texture("grass.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 24, 8);  // Look at this beautiful initialisation. That's literally it
    //m_state.map->normalize(10, 7.5); //normalize with the new coordinates of the view 
    //m_state.map->normalize(0, 0); //normalize with the new coordinates of the view 

    m_state.font_texture_id = Utility::load_texture("font1.png"); //then load font as well 


    // player initializations 
    m_state.player = new Entity();
    m_state.player->set_position(glm::vec3(0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_gravity(m_state.gravity);
    //m_state.player->set_gravity(0); //added this just to debug please delete 
    m_state.player->m_height = 1.0f;
    m_state.player->m_width = 0.5f;

    m_state.player->m_speed = 1.5f;
    m_state.player->m_texture_id = Utility::load_texture("protagonist.png");
    m_state.player->m_entity_type = PLAYER;

    m_state.player->m_walking[m_state.player->LEFT] = new int[4] { 95, 94, 93, 92 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4] { 16, 17, 18, 19 };
    m_state.player->m_walking[m_state.player->UP] = new int[4] { 16, 17, 16, 17 };
    m_state.player->m_walking[m_state.player->DOWN] = new int[4] { 0, 0, 0, 0 };
    m_state.player->m_walking[m_state.player->IDLE] = new int[4] { 0, 0, 0, 1 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->IDLE];  // start player looking down
    m_state.player->m_animation_frames = 4;
    m_state.player->m_animation_index = 0;
    m_state.player->m_animation_time = 0.0f;
    m_state.player->m_animation_cols = 8;
    m_state.player->m_animation_rows = 18;


    //create the enemies
    std::vector<glm::vec3> enemy_initial_pos = { glm::vec3(1.0f, -2.0f, 0), glm::vec3(8.0f, 0.0f, 0), glm::vec3(13.0f, 5.0f, 0) };
    std::vector<AIType> enemy_ai_type = { SORCERER, JUMPER, WALKER };

    m_state.NUM_ENEMIES = 3;

    for (int i = 0; i < m_state.NUM_ENEMIES; i++) {
        Entity* enemy = new Entity();

        m_state.enemies.push_back(enemy);
        enemy->set_position(enemy_initial_pos[i]);
        enemy->set_movement(glm::vec3(0.0f)); //may not even be needed check 

        enemy->set_gravity(m_state.gravity);
        enemy->m_height = 1.0f;
        enemy->m_width = 0.5f;
        enemy->m_entity_type = ENEMY;
        enemy->m_ai_type = enemy_ai_type[i];

        enemy->m_speed = 1.5f;
        enemy->m_texture_id = Utility::load_texture("wizard.png");

        enemy->m_walking[enemy->LEFT] = new int[4] { 99, 98, 97, 96 };
        enemy->m_walking[enemy->RIGHT] = new int[4] { 10, 11, 12, 13 };
        enemy->m_walking[enemy->UP] = new int[4] { 10, 11, 10, 11 };
        enemy->m_walking[enemy->DOWN] = new int[4] { 57, 56, 55, 54 };
        enemy->m_walking[enemy->IDLE] = new int[4] { 0, 0, 0, 1 };

        enemy->m_activity = ALIVE;
        enemy->m_animation_indices = enemy->m_walking[enemy->IDLE];  // start enemy looking down
        enemy->m_animation_frames = 4;
        enemy->m_animation_index = 0;
        enemy->m_animation_time = 0.0f;
        enemy->m_animation_cols = 10;
        enemy->m_animation_rows = 16;

    }


    



    /**
     BGM and SFX
     */
     //Start Audio 
    Mix_OpenAudio(
        CD_QUAL_FREQ,   //frequency to playback audio at Hz
        MIX_DEFAULT_FORMAT,     //audio format 
        AUDIO_CHAN_AMT,         //channels. mono stero ...
        AUDIO_BUFF_SIZE         //audio buffer size in sample frames 
    );

    // Similar to our custom function load_texture
    m_state.bgm = Mix_LoadMUS("sweetdreams.mp3");

    // This will schedule the music object to begin mixing for playback.
    // The first parameter is the pointer to the mp3 we loaded 
    // and second parameter is the number of times to loop.
    Mix_PlayMusic(m_state.bgm, LOOP_FOREVER);

    // Set the music to half volume
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2); //MIX_MAX_VOLUME / 2

    //m_state.jump_sfx = Mix_LoadWAV("assets/bounce.wav");
}

void Level1::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, m_state.map);

    m_state.enemies_defeated = 0;
    for (int i = 0; i < m_state.NUM_ENEMIES; i++) {
        if (m_state.enemies[i]->get_activity() == DEAD)
            m_state.enemies_defeated++;
        if (m_state.enemies_defeated == m_state.NUM_ENEMIES) {
            m_state.endgame = true;
        }
        m_state.enemies[i]->update(delta_time, m_state.player, m_state.enemies, m_state.map);
    }
}

void Level1::render(ShaderProgram* program)
{
    m_state.map->render(program);
    m_state.player->render(program);

    for (int i = 0; i < m_state.NUM_ENEMIES; i++) {
        m_state.enemies[i]->render(program);
    }

    glm::vec3 position;


    if (m_state.player->get_activity() == DEAD) {
        position = { 2.0,-4, 0.0 };
        Utility::draw_text(program, m_state.font_texture_id, "Failed the Quest", 0.75, 0, position);
        //glm::translate(m_state.rocket->m_model_matrix, m_position);
    }
    else if (m_state.enemies_defeated == m_state.NUM_ENEMIES) {
        position = { 3.5,-4.0, 0.0 };
        Utility::draw_text(program, m_state.font_texture_id, "Sucess :)", 0.75, 0, position);
    }
}