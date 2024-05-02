
#include "Entity.h"
#include <fstream>
#include <cmath>


Entity::Entity()
{
    m_prev_position = glm::vec3(0.0f);
    m_curr_position = glm::vec3(0.0f);
    m_model_matrix = glm::mat4(1.0f);
    acceleration = glm::vec3(0.0f);
    m_radius = 1.0f;
    m_status = ALIVE;

}

Entity::~Entity()
{

}


void Entity::update(float dt, std::ofstream& log)
{
    // Compute how much we moved
    glm::vec3 displacement = m_curr_position - m_prev_position;


    // Update position
    m_prev_position = m_curr_position;

    glm::vec3 a_factor = acceleration * (dt * dt);
    m_curr_position = m_curr_position + displacement + a_factor;
    // Reset acceleration

    acceleration = glm::vec3(0.0f);


    m_model_matrix = glm::mat4(1.0f); //reset 
    m_model_matrix = glm::translate(m_model_matrix, m_curr_position);
}

