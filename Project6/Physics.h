#pragma once
#pragma once
// Entity.h
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"
#include "Entity.h"


#include <vector>
#include <cmath>

#include <ostream>

struct Constraint
{
public:

    glm::vec3 m_position = { 0.0, 0.0, 0.0 };
    glm::mat4 m_model_matrix = glm::mat4(1.0f);

    float m_radius;
};



class Physics
{
public:

    glm::vec3  m_gravity;

    Constraint constraint;


    void set_gravity(float x) { m_gravity = glm::vec3(0.0f, x, 0.0f); };

    glm::vec3 get_gravity() const { return m_gravity; };

    void set_constraint(float radius) { constraint.m_radius = radius; };

    Constraint get_constraint() const { return constraint; };


         
    void apply_gravity(std::vector<Entity*>& entities);

    void apply_constraint(std::vector<Entity*>& entities);

    void check_collisions(std::vector<Entity*>& entities);



    Physics() { m_gravity = glm::vec3(0.0f, 0.0f, 0.0f); };

    

};

