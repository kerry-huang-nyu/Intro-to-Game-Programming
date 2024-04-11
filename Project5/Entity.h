#pragma once
// Entity.h
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"
#include <ostream>
#include "Map.h"

enum EntityType { PLATFORM, PLAYER, ENEMY };
enum AIType { WALKER, FOLLOWER, JUMPER, SORCERER};
enum AIState { WALKING, IDLE, ATTACKING };
enum Activity { DEAD, ALIVE};


class Entity
{
private:
    //look into how to animate in this case + connect the different stuff 
    int* m_animation_right = NULL, // move to the right
        * m_animation_left = NULL, // move to the left
        * m_animation_up = NULL, // move upwards
        * m_animation_down = NULL, // move downwards
        * m_animation_idle = NULL; //idle animation 

    glm::vec3 m_position;
    double gravity = 0;


public:
    // ————— STATIC VARIABLES ————— //
    EntityType m_entity_type;
    AIType m_ai_type;
    AIState m_ai_state;
    Activity m_activity;

    static const int SECONDS_PER_FRAME = 4;
    static const int LEFT = 0,
                    RIGHT = 1,
                    UP = 2,
                    DOWN = 3,
                    IDLE = 4;

    // ————— ANIMATION ————— //
    int** m_walking = new int* [5]
        {
            m_animation_left,
            m_animation_right,
            m_animation_up,
            m_animation_down,
            m_animation_idle
        };

    int m_animation_frames = 0,
        m_animation_index = 0,
        m_animation_cols = 0,
        m_animation_rows = 0;

    int* m_animation_indices = NULL;
    float m_animation_time = 0.0f;

    float* nightfall;
    float* night_level;


    // ————— TRANSFORMATIONS ————— //
    float     m_speed;
    glm::vec3 m_movement;
    glm::vec3 m_velocity;

    glm::mat4 m_model_matrix;

    GLuint    m_texture_id;

    // ----- COLLISION ----- //

    int m_direction; //IDLE at the start 
    double m_height; //height and width of modle ma
    double m_width;

    bool m_collided_top,
        m_collided_bottom,
        m_collided_left,
        m_collided_right;

    bool m_pit_left,
        m_pit_right;

    int ai_action[4];
    int action_frame = 0;

    // ————— METHODS ————— //
    Entity();
    ~Entity();


    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    void update(float delta_time, Entity* player, std::vector<Entity*>& others, Map* map);

    void animate(float delta_time);
    void move(float delta_time, std::ofstream& log);
    void simulate_gravity(float delta_time);
    void render(ShaderProgram* program);

    void clear_collision();

    //get and set m_position, m_movement 

    // ————— GETTERS ————— //
    const glm::vec3  get_position() const { return m_position; };
    const glm::vec3  get_movement() const { return m_movement; };

    // ————— SETTERS ————— //
    void const set_position(glm::vec3 new_position) { m_position = new_position; };
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; };


    //------- AI methods ----------------
    void ai_activate(float delta_time, Entity* player, std::ofstream&);
    void ai_walk(std::ofstream& log);
    void ai_jump(std::ofstream& log);

    //void ai_follow(Entity* player, std::ofstream& log);
    void ai_guard(Entity* player);
    void ai_sorcery();

    // Getters
    EntityType const get_entity_type()    const { return m_entity_type; };
    AIType     const get_ai_type()        const { return m_ai_type; };
    AIState    const get_ai_state()       const { return m_ai_state; };
    Activity   const get_activity()       const { return m_activity; };
    double     const get_gravity()        const { return gravity; };

    int        const get_direction()      const { return m_direction; }; 
    
    void       const check_collision_entity(Entity*, std::ostream& log);

    void       const check_collision_y(Map* map, std::ofstream&);
    void       const check_collision_x(Map* map, std::ofstream&);

    bool       const check_collision(Entity* collidable_entity);

    // Setters
    void const set_entity_type(EntityType new_entity_type) { m_entity_type = new_entity_type; };
    void const set_ai_type(AIType new_ai_type) { m_ai_type = new_ai_type; };
    void const set_ai_state(AIState new_state) { m_ai_state = new_state; };
    void const set_activity(Activity new_activity) { m_activity = new_activity; };
    void const set_gravity(double new_gravity) { gravity = new_gravity; };
    void const set_direction(int new_direction);

    void const set_height(float height) { m_height = height; };
    void const set_width(float width) { m_width = width; };

};

