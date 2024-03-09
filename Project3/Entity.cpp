
#include "Entity.h"
#include <fstream>
#include <cmath>

// Entity.cpp
Entity::Entity()
{
    m_position = glm::vec3(0);
    m_velocity = glm::vec3(0);
    m_accelerate = glm::vec3(0);

    radius = 0;

    thrustacc = 0.5; //the acceleration value of the thruster 
    thrust = 0;

    gravity = 0;
    fuel = 1000;//initial fuel 


    turnrad = glm::radians(1.0f);
    turndir = 0;
    totalrad = 0;

    m_model_matrix = glm::mat4(1.0f);
}


float Entity::getVelocity() const {
    float speed = sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);
    return speed;
}

bool Entity::upright() const {
    float margin = M_PI / 16;
    if (totalrad < margin || 2 * M_PI - margin < totalrad) { //within 1/16 of the situation in
        return true;
    }
    return false;
}


void Entity::Turn(float delta_time) { //to be used mostly by the entity itself 
    float total = turnrad * delta_time * 100;
    total *= turndir;
    totalrad += total;

    //regularize totalrad 
    if (totalrad < 0) {
        totalrad += 2 * M_PI;
    }
    totalrad = fmod(totalrad, 2*M_PI); 

    m_model_matrix = glm::rotate(m_model_matrix, totalrad, glm::vec3(0.0f, 0.0f, 1.0f));
}

//used by the buttons to start and stop turn 
void Entity::StartTurn(bool clockwise) { 
    if (clockwise) turndir = 1;
    else turndir = -1;
}

void Entity::StopTurn() {
    turndir = 0;
}

void Entity::Thrust(float delta_time) {
    //take into account the direction to get the x and y components 
    if (fuel > 0){ //only thrust when you still have fuel
        double y_component = abs(thrust * cos(totalrad));
        double x_component = abs(thrust * sin(totalrad));

        fuel -= thrust * delta_time * 50;
        if (fuel < 0) {
            fuel = 0;
        }

        if (totalrad < M_PI) { //negate under these circumstances 
            x_component *= -1;
        }
        if (M_PI / 2 < totalrad && totalrad < 3 * M_PI / 2) {
            y_component *= -1;
        }

        //add the movements 
        m_velocity.y += y_component * delta_time;
        m_velocity.x += x_component * delta_time;
    }

}

void Entity::StartThrust() {
    thrust = thrustacc;
}

void Entity::StopThrust() {
    thrust = 0;
}

void Entity::Update(float delta_time) {
    //add the movements 

    m_velocity.y += gravity * delta_time; //gravity 
    m_velocity += m_accelerate * delta_time;
    m_position += m_velocity * delta_time;

    //apply transformation 
    m_model_matrix = glm::mat4(1.0f);

    
    Thrust(delta_time);

    m_model_matrix = glm::translate(m_model_matrix, m_position);
    Turn(delta_time);//turn 

    //why does turn have unexpected results!!!
}