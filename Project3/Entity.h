#pragma once
// Entity.h
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include <ostream>

class Entity {
public:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_accelerate;

    float radius; //collision radius 

    float gravity;

    float thrustacc;
    float thrust;
    float fuel;

    float turnrad; //turning radius each time 
    float turndir; //turning direction 
    float totalrad;

    glm::mat4 m_model_matrix;
    GLuint m_texture_id;

    //methods and functions 

    Entity();

    float getVelocity() const;
    bool upright() const;

    void Update(float delta_time);

    void Turn(float delta_time);
    void StartTurn(bool clockwise);
    void StopTurn();

    void Thrust(float delta_time);
    void StartThrust();
    void StopThrust();

};

