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

/**
    Notice that the game's state is now part of the Scene class, not the main file.
*/

struct GameState
{
    int NUM_ENEMIES;

    int enemies_defeated_past = 0;
    int enemies_defeated = 0;
    
    double gravity = -7;

    GLint font_texture_id;

    bool endgame = false; 

    Entity* player;

    Map* map;

    std::vector<Entity*> enemies;



    // ————— AUDIO ————— //
    Mix_Music* bgm;
    Mix_Chunk* eat_fx;
    
    //Mix_Chunk* jump_sfx;
};

class Scene {
public:
    // ————— ATTRIBUTES ————— //

    GameState m_state;

    // ————— METHODS ————— //
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram* program) = 0;
    virtual void reset() = 0;

    // ————— GETTERS ————— //
    GameState const get_state()             const { return m_state; }
};