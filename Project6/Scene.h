#pragma once
// Scene.h
#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"

#include "Physics.h"

/**
    Notice that the game's state is now part of the Scene class, not the main file.
*/

struct GameState
{

    GLint font_texture_id;

    bool endgame = false; 

    //Entity* player;

    std::vector<Entity*> fruits;


    // ————— AUDIO ————— //
    Mix_Music* bgm;

    Physics engine;
    
    //Mix_Chunk* jump_sfx;
};

class Scene {
public:
    // ————— ATTRIBUTES ————— //

    GameState m_state;

    // ————— METHODS ————— //
    virtual void initialise() = 0;
    virtual void update(float delta_time, std::ofstream& ) = 0;
    virtual void render(ShaderProgram* program, ShaderProgram* text_program, std::ofstream&) = 0;
    virtual void reset() = 0;

    virtual void spawn(float x, float y, int fruitindex) { };

    // ————— GETTERS ————— //
    GameState const get_state()             const { return m_state; }
};