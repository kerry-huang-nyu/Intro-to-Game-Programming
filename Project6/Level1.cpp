// Level1.cpp
#include "Level1.h"
#include "Utility.h"

#include "Physics.h"
#include <random>

#define LEVEL_WIDTH 16
#define LEVEL_HEIGHT 12


//compact disk quality frequency
const int   CD_QUAL_FREQ = 44100,
AUDIO_CHAN_AMT = 2,
AUDIO_BUFF_SIZE = 4096;

const int PLAY_ONCE = 0,
NEXT_CHNL = -1,  // next available channel
MUTE_VOL = 0,
MILS_IN_SEC = 1000,
ALL_SFX_CHN = -1;

const int    LOOP_FOREVER = -1;  // -1 means loop forever in Mix_PlayMusic; 0 means play once and loop zero times


Level1::~Level1()
{

    Mix_FreeChunk(m_state.eat_fx);
    Mix_FreeMusic(m_state.bgm);
}

void Level1::initialise()
{
   
    m_state.font_texture_id = Utility::load_texture("font1.png"); //then load font as well 

    //1 fruit initialization 

    std::vector<glm::vec3> positions = { glm::vec3(1.4, 0.0, 0.0), glm::vec3(-2.4, 0.0, 0.0) };

    for (int i = 0; i < 2; i++) {

        Entity* fruit = new Entity();

        m_state.fruits.push_back(fruit);


        //fruit->set_position(glm::vec3(1.0f, 1.0f, 0));


        fruit->m_radius = 0.8;

        fruit->m_status = ALIVE;
        fruit->m_curr_position = positions[i];
        fruit->m_prev_position = positions[i];

    }
    
    

    m_state.engine = Physics();
    m_state.engine.set_gravity(-7.0f);

    m_state.engine.set_constraint(6.0f); //set my constraint to be a 4 radius circle 

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
    m_state.eat_fx = Mix_LoadWAV("crunch.wav");

    // This will schedule the music object to begin mixing for playback.
    // The first parameter is the pointer to the mp3 we loaded 
    // and second parameter is the number of times to loop.
    Mix_PlayMusic(m_state.bgm, LOOP_FOREVER);

    // Set the music to half volume
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2); //MIX_MAX_VOLUME / 2

    Mix_VolumeChunk(
        m_state.eat_fx,     // Set the volume of the bounce sound...
        MIX_MAX_VOLUME / 4  // ... to 1/4th.
    );
}

void Level1::reset() 
{
    m_state.endgame = false;
   /* m_state.player->set_status(ALIVE);
    m_state.player->set_position(glm::vec3(0));
    m_state.player->m_movement = glm::vec3(0);*/
    
}

void Level1::spawn(int x, int y) 
{
    int randomNumber = gen() % 6;

    Entity* fruit = new Entity();

    m_state.fruits.push_back(fruit);

    fruit->m_radius = fruit_stats.radius[randomNumber];

    fruit->m_status = ALIVE;
    fruit->m_entity_type = fruit_stats.type[randomNumber];
    fruit->color = fruit_stats.color[randomNumber];

    fruit->m_curr_position = glm::vec3(0);
    fruit->m_prev_position = glm::vec3(0);


}

void Level1::update(float delta_time, std::ofstream& log)
{
    m_state.engine.apply_gravity(m_state.fruits);

    m_state.engine.apply_constraint(m_state.fruits);

    m_state.engine.check_collisions(m_state.fruits);

    for (int i = 0; i < m_state.fruits.size(); i++) {
        m_state.fruits[i]->update(delta_time, log);
    }

}

void Level1::render(ShaderProgram* program, std::ofstream& log)
{


    glm::vec3 position = { 0.0, 0.0, 0.0 };


    program->set_colour(1.0f, 1.0f, 1.0f, 1.0f);

    Utility::render(program, m_state.engine.constraint.m_model_matrix, m_state.engine.constraint.m_radius, 64, log);

    //Utility::draw_text(program, m_state.font_texture_id, "Have I been here before?", 0.5, 0, position);

    program->set_colour(1.0f, 0.0f, 0.0f, 1.0f);

    for (int i = 0; i < m_state.fruits.size(); i++) {

        //Utility::draw_text(program, m_state.font_texture_id, "Have I been here before?", 0.5, 0, position);
        //log << "radius : " << m_state.fruits[i]->m_radius << "\n";

        Utility::render(program, m_state.fruits[i]->m_model_matrix, m_state.fruits[i]->m_radius, 64, log);

        
    }
}