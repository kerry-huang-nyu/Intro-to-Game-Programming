#pragma once
// Entity.h
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"
#include <vector>

#

#include <ostream>

enum EntityType { F1, F2, F3, F4, F5, F6 };
enum Status {ALIVE, DEAD};

class Entity
{
public:

    glm::vec3 m_prev_position;
    glm::vec3 m_curr_position;
    glm::vec3 acceleration;

    std::vector<float> color;

    // ————— STATIC VARIABLES ————— //
    EntityType m_entity_type;
    Status m_status;
    float m_radius;


    // ————— TRANSFORMATIONS ————— //
    //glm::vec3 m_movement;
    //glm::vec3 m_velocity;

    glm::mat4 m_model_matrix;


    // ————— METHODS ————— //
    Entity();
    ~Entity();


    //void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    //void update(float delta_time, Entity* player, std::vector<Entity*>& others, Map* map);

    //void render(ShaderProgram* program, std::ofstream& log);

    //get and set m_position, m_movement 

    // ————— GETTERS ————— //
    const glm::vec3  get_position() const { return m_curr_position; };
    const glm::vec3  get_velocity(float dt) const { return (m_curr_position - m_prev_position) / dt; };

    // ————— SETTERS ————— //
    //void const set_position(glm::vec3 new_position) { m_prev_position = m_curr_position; m_curr_position = new_position; };

    // Getters
    EntityType const get_entity_type()    const { return m_entity_type; };
    
    double       const get_status()       const { return m_status; };

    void update(float dt, std::ofstream&);
    void accelerate(const glm::vec3& a) { acceleration += a; };
    
   
    // Setters
    void const set_entity_type(EntityType new_entity_type) { m_entity_type = new_entity_type; };
    void const set_status(Status new_status) { m_status = new_status;  };
    //void const set_direction(int new_direction);
};

