// LoadScreen.cpp
#include "LoadScreen.h"
#include "Utility.h"

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 12


//compact disk quality frequency
const int   CD_QUAL_FREQ = 44100,
AUDIO_CHAN_AMT = 2,
AUDIO_BUFF_SIZE = 4096;

const int    LOOP_FOREVER = -1;  // -1 means loop forever in Mix_PlayMusic; 0 means play once and loop zero times


LoadScreen::~LoadScreen()
{
    //delete    m_state.player;
    Mix_FreeMusic(m_state.bgm);
}

void LoadScreen::initialise()
{
    //GLuint map_texture_id = Utility::load_texture("grass.png");

    m_state.font_texture_id = Utility::load_texture("font1.png"); //then load font as well 

    //// player initializations 
    //m_state.player = new Entity();
    //m_state.player->set_position(glm::vec3(14.0f, -3.5f, 0.0f));


    //m_state.player->set_movement(glm::vec3(0.0f));
    //m_state.player->set_gravity(0);
    ////m_state.player->set_gravity(0); //added this just to debug please delete 
    //m_state.player->radius = 3.0f;

    //m_state.player->m_speed = 0.0f;
    //m_state.player->m_texture_id = Utility::load_texture("bread.png");

    ////m_state.player->m_direction = IDLE;

    //m_state.player->m_walking[m_state.player->LEFT] = new int[4] { 23, 22, 21, 20 };
    //m_state.player->m_walking[m_state.player->RIGHT] = new int[4] { 4, 5, 6, 7 };
    //m_state.player->m_walking[m_state.player->UP] = new int[4] { 8, 8, 8, 8 };
    //m_state.player->m_walking[m_state.player->DOWN] = new int[4] { 7, 7, 12, 12 };
    //m_state.player->m_walking[m_state.player->IDLE] = new int[4] { 0, 1, 2, 2 };

    //m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->IDLE];  // start player looking down
    //m_state.player->m_animation_frames = 4;
    //m_state.player->m_animation_index = 0;
    //m_state.player->m_animation_time = 0.0f;
    //m_state.player->m_animation_cols = 4;
    //m_state.player->m_animation_rows = 8;

    //m_state.NUM_ENEMIES = 0;


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
    m_state.bgm = Mix_LoadMUS("bread_song.mp3");

    // This will schedule the music object to begin mixing for playback.
    // The first parameter is the pointer to the mp3 we loaded 
    // and second parameter is the number of times to loop.
    Mix_PlayMusic(m_state.bgm, LOOP_FOREVER);

    // Set the music to half volume
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2); //MIX_MAX_VOLUME / 2

    //m_state.jump_sfx = Mix_LoadWAV("assets/bounce.wav");
}

void LoadScreen::update(float delta_time, std::ofstream&)
{
    //m_state.player->update(delta_time, m_state.player, m_state.enemies, m_state.map);
}

void LoadScreen::render(ShaderProgram* program, std::ofstream& )
{
    //m_state.player->render(program);

    glm::vec3 position;

    position = { 17.0,-4.5, 0.0 };
    Utility::draw_text(program, m_state.font_texture_id, "Press Space to Start", 0.5, 0, position);

}

void LoadScreen::reset() {

}