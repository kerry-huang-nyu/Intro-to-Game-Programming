// Level2.cpp
#include "Level2.h"
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


Level2::~Level2()
{
    Mix_FreeMusic(m_state.bgm);

    for (Entity* ptr : m_state.fruits) {
        delete ptr;
    }
}

void Level2::initialise()
{

    m_state.font_texture_id = Utility::load_texture("font1.png"); //then load font as well 

    m_state.engine = Physics();
    m_state.engine.set_gravity(-50.0f);
    m_state.engine.set_constraint(2.5f); //set my constraint to be a 4 radius circle 


    //We need the Goals2 
    Goals2.have = { 6, 0, 2, 0, 1, 0 };
    Goals2.forbid = { false, false, false, false, false, false };


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
}

void Level2::reset()
{
    m_state.endgame = false;
    /* m_state.player->set_status(ALIVE);
     m_state.player->set_position(glm::vec3(0));
     m_state.player->m_movement = glm::vec3(0);*/

}

void Level2::spawn(float x, float y, int fruitindex)
{
    int randomNumber = fruitindex;

    if (fruitindex == -1) {
        randomNumber = fruit_stats.nextfruit;
        fruit_stats.nextfruit = gen() % 3;
    }

    Entity* fruit = new Entity();

    m_state.fruits.push_back(fruit);

    fruit->m_index = randomNumber;
    fruit->m_radius = fruit_stats.RADIUS[randomNumber];

    fruit->m_status = ALIVE;
    fruit->color = fruit_stats.COLOR[randomNumber];


    fruit->m_curr_position = glm::vec3(x, y, 0.0f);
    fruit->m_prev_position = glm::vec3(x, y, 0.0f);
}

void Level2::clean_death() {
    int start = -1; //start is the first hole 

    for (int i = 0; i < m_state.fruits.size(); i++) {
        if (m_state.fruits[i]->m_status == DEAD) {
            delete m_state.fruits[i];
            if (start == -1) {
                start = i;
            }
        }
        else if (start >= 0) {
            m_state.fruits[start] = m_state.fruits[i]; //copy over no need to swap 
            start++;
        }
    }

    if (start >= 0) {
        m_state.fruits.resize(start); //in the end you have start number of fruits left 
    }

}

void Level2::spawn_new_fruits(std::vector<Info>& info) {
    for (Info& fruit : info) {
        spawn(fruit.x, fruit.y, fruit.index);
    }
}

bool Level2::check_win()
{
    if (m_state.endgame == true) {
        return true; //stop checking at this pt 
    }

    m_state.endgame = true;
    std::vector<int> count = { 0,0,0,0,0,0 };

    for (int i = 0; i < m_state.fruits.size(); i++) {
        int ind = m_state.fruits[i]->get_index();
        count[ind]++;
    }

    for (int i = 0; i < 6; i++) {
        if (Goals2.have[i] > count[i]) {
            m_state.endgame = false;
            break;
        }
        if ((Goals2.forbid[i] == true) && (count[i] > 0)) {
            m_state.endgame = false;
            break;
        }
    }
    return m_state.endgame;
}

void Level2::update(float delta_time, std::ofstream& log)
{
    if (check_win() == true) {
        return;
    }

    m_state.engine.apply_gravity(m_state.fruits);
    std::vector<Info> info = m_state.engine.check_collisions(m_state.fruits);

    //clean the exploded fruits 
    //then create new ones from their ashes 
    clean_death();
    spawn_new_fruits(info);

    m_state.engine.apply_constraint(m_state.fruits, delta_time);

    for (int i = 0; i < m_state.fruits.size(); i++) { //update objects 
        m_state.fruits[i]->update(delta_time, log);
    }
}

void Level2::render_next_fruit(ShaderProgram* program, ShaderProgram* text_program, std::ofstream& log)
{
    //render the next fruit 
    glUseProgram(text_program->get_program_id());
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec3 position = { 2.25, 3.0, 0.0 };
    Utility::draw_text(text_program, m_state.font_texture_id, "Next Ball", 0.25, 0, position);
    glUseProgram(program->get_program_id());

    //create the new fruit 
    glm::mat4 loc = glm::mat4(1.0f);
    loc = glm::translate(loc, glm::vec3(3.5, 2.0, 0.0));
    int index = fruit_stats.nextfruit;

    program->set_colour(fruit_stats.COLOR[index][0], fruit_stats.COLOR[index][1], fruit_stats.COLOR[index][2], 1.0f);
    Utility::render(program, loc, fruit_stats.RADIUS[index], 64, log);

}

void Level2::render_Goals2(ShaderProgram* program, ShaderProgram* text_program, std::ofstream& log) {

    glm::vec3 position = { -4.8, 3.0, 0.0 };
    glm::vec3 increment = { 0.0, -1.0, 0.0 };

    //render the criterias 

    glUseProgram(text_program->get_program_id());
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::vector<std::string> names = { "Red", "Orange", "Yellow", "Green", "Blue", "Purple" };

    std::string message = "";

    for (int i = 0; i < 6; i++) {

        if (Goals2.forbid[i] == true) { //if they have no criterias 
            message = names[i] + " prohibited";
        }
        else if (Goals2.have[i] != 0) {
            message = names[i] + " >= " + std::to_string(Goals2.have[i]);
        }
        else {
            message = names[i] + " no limit";
        }

        Utility::draw_text(text_program, m_state.font_texture_id, message, 0.2, 0, position);
        position += increment;

        message = "";
    }

    glUseProgram(program->get_program_id());
}

void Level2::render(ShaderProgram* program, ShaderProgram* text_program, std::ofstream& log)
{
    program->set_colour(1.0f / 3, 1.0f / 3, 1.0f / 3, 1.0f);
    Utility::render(program, m_state.engine.constraint.m_model_matrix, m_state.engine.constraint.m_radius, 64, log);
    program->set_colour(1.0f, 0.0f, 0.0f, 1.0f);

    for (int i = 0; i < m_state.fruits.size(); i++) {
        //Utility::draw_text(program, m_state.font_texture_id, "Have I been here before?", 0.5, 0, position);
        //log << "radius : " << m_state.fruits[i]->m_radius << "\n";
        std::vector<float> temp = m_state.fruits[i]->color;

        program->set_colour(temp[0], temp[1], temp[2], 1.0f);
        Utility::render(program, m_state.fruits[i]->m_model_matrix, m_state.fruits[i]->m_radius, 64, log);
    }

    render_next_fruit(program, text_program, log);
    render_Goals2(program, text_program, log);

}