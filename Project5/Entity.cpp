
#include "Entity.h"
#include <fstream>
#include <cmath>


Entity::Entity()
{
    m_position = glm::vec3(0);
    m_speed = 0;
    m_model_matrix = glm::mat4(1.0f);
    m_activity = ALIVE;
    m_direction = IDLE; //idle at the start
}

Entity::~Entity()
{
    delete[] m_animation_up;
    delete[] m_animation_down;
    delete[] m_animation_left;
    delete[] m_animation_right;
    delete[] m_animation_idle;
    delete[] m_walking;
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    // Step 4: And render
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_position_attribute());
    
}

void Entity::update(float delta_time, Entity* player, std::vector<Entity*>& others, Map* map) //why do we need to pass in the entity as player????
{
    if (get_activity() == ALIVE) {
        if (get_position().y <= -25) {
            set_activity(DEAD);
        }


        m_model_matrix = glm::mat4(1.0f);
        clear_collision();
        simulate_gravity(delta_time);

        //check_collision_y(map, log);
        //check_collision_x(map, log);

        

        move(delta_time);
        m_model_matrix = glm::translate(m_model_matrix, m_position);

        if (m_entity_type == PLAYER) {

            for (int i = 0; i < others.size(); i++) {
                if (others[i]->get_activity() == ALIVE) {
                    clear_collision();

                    check_collision_entity(others[i]);

                    if (m_collided_right || m_collided_left || m_collided_top) {
                        set_activity(DEAD);
                        //log << " my current position " << m_position.x << "   " << m_position.y << "\n";
                        //log << " it is entity iiii " << i << " with their positions as " << others[i]->get_position().x << "   " << others[i]->get_position().y << " \n";

                    }

                    else if (m_collided_bottom) {
                        others[i]->set_activity(DEAD);
                    }
                }

            }
        }

        check_collision_y(map);
        check_collision_x(map);
        if (m_entity_type == ENEMY) ai_activate(delta_time, player);

        //maintain the jump attribute 
        

        animate(delta_time);
    }
}

void Entity::move(float delta_time) {
    switch (m_direction) {
        case RIGHT:
            m_velocity.x = 1;
            m_animation_indices = m_walking[RIGHT];
            break;

        case LEFT:
            m_velocity.x = -1;
            m_animation_indices = m_walking[LEFT];
            break;

        case UP:
            m_velocity.y = 1;
            m_animation_indices = m_walking[UP];
            break;

        case DOWN:
            m_animation_indices = m_walking[DOWN];
            break;

        default: //idle 
            m_velocity.x = 0;
            //m_movement.y = 0;
            m_animation_indices = m_walking[IDLE];
    }

    m_position += m_velocity * m_speed * delta_time;
}

void Entity::simulate_gravity(float delta_time) {
    m_velocity.y += gravity * delta_time; //gravity 
    m_position += m_velocity * delta_time;
}

void Entity::animate(float delta_time) {

    
    if (m_animation_indices != NULL)
    {
        //if (glm::length(m_movement) != 0) //if our movement deserves an animation 
        //{

        //no matter what we will animate and this will allow us to do idle animations 
        m_animation_time += delta_time;
        float frames_per_second = (float)1 / SECONDS_PER_FRAME;

        if (m_animation_time >= frames_per_second) //if the change in time warrants an animation
        {
            m_animation_time = 0.0f;
            m_animation_index++;

            m_animation_index %= m_animation_frames;//reset to 0 at some point 
        }
        //}
    }

}

void Entity::render(ShaderProgram* program)
{

    if (get_activity() == ALIVE || get_entity_type() == PLAYER) {
        program->set_model_matrix(m_model_matrix);

        if (m_animation_indices != NULL)
        {
            draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
            return;
        }

        float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
        float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

        glBindTexture(GL_TEXTURE_2D, m_texture_id);

        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDisableVertexAttribArray(program->get_position_attribute());
        glDisableVertexAttribArray(program->get_position_attribute());
    }
}

void Entity::ai_activate(float delta_time, Entity* player)
{
    switch (m_ai_type)
    {
    case WALKER:
        ai_walk();
        break;

    case JUMPER:
        ai_jump();
        break;

    //case FOLLOWER:
        //ai_follow(player, log);
        //break;

    case SORCERER:
        ai_sorcery();
        break;

    default:
        break;
    }
}

void Entity::ai_walk()
{
    //oscillation between left and right 
    action_frame++;
    action_frame %= 4;

    if (m_pit_left){
        action_frame = 0;
        ai_action[0] = RIGHT;
        ai_action[1] = RIGHT;
        ai_action[2] = RIGHT;
        ai_action[3] = RIGHT;
    }
    else if (m_pit_right) {
        action_frame = 0;
        ai_action[0] = LEFT;
        ai_action[1] = LEFT;
        ai_action[2] = LEFT;
        ai_action[3] = LEFT;
    }
   
    set_direction(ai_action[action_frame]);
}

void Entity::ai_jump()
{
    //oscillation between left and right 
    action_frame++;
    action_frame %= 4;

    if (m_pit_left) {
        action_frame = 0;
        ai_action[0] = RIGHT;
        ai_action[1] = RIGHT;
        ai_action[2] = RIGHT;
        ai_action[3] = UP;
    }
    else if (m_pit_right) {
        action_frame = 0;
        ai_action[0] = LEFT;
        ai_action[1] = LEFT;
        ai_action[2] = LEFT;
        ai_action[3] = UP;
    }

    set_direction(ai_action[action_frame]);
}

void Entity::ai_sorcery() {
    set_direction(IDLE);
}



/*
void Entity::ai_follow(Entity* player, std::ofstream& log)
{
    //oscillation between left and right 
    action_frame++;
    action_frame %= 4;


    if (glm::distance(m_position, player->get_position()) < 4.0f) {
        //log << " mypos    " << m_position.x << "    other pos   " << player->get_position().x << "\n";
        if (m_position.x > player->get_position().x || m_pit_right) {
            action_frame = 0;
            ai_action[0] = LEFT;
            ai_action[1] = LEFT;
            ai_action[2] = LEFT;
            ai_action[3] = LEFT;
        }

        else if (m_position.x < player->get_position().x || m_pit_left) {
            action_frame = 0;
            ai_action[0] = RIGHT;
            ai_action[1] = RIGHT;
            ai_action[2] = RIGHT;
            ai_action[3] = RIGHT;
        }
    }
    else {
        ai_action[0] = IDLE;
        ai_action[1] = IDLE;
        ai_action[2] = IDLE;
        ai_action[3] = IDLE;
    }
    
    set_direction(ai_action[action_frame]);
}*/

void const Entity::check_collision_y(Map* map)
{
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);

    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    // If the map is solid, check the top three points

    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }

    // And the bottom three points
    float dummy;
    if (!map->is_solid(bottom_left, &dummy, &dummy) && map->is_solid(bottom_right, &dummy, &dummy) && m_velocity.y < 0) {
        m_pit_left = true;
    }
    else if (!map->is_solid(bottom_right, &dummy, &dummy) && map->is_solid(bottom_left, &dummy, &dummy)  && m_velocity.y < 0) {
        m_pit_right = true;
    }

    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;

    }

    
}

void const Entity::check_collision_x(Map* map)
{
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}

void const Entity::check_collision_entity(Entity* collidable_entity) {
    //log << " nooo \n";
    if (check_collision(collidable_entity))
    {
        //from the checkcollision right 
        float y_distance = fabs(m_position.y - collidable_entity->get_position().y);
        float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));
        if (m_position.y < collidable_entity->get_position().y) {
            //m_position.y -= y_overlap;
            m_velocity.y = 0;
            m_collided_top = true;
        }
        else if (m_position.y > collidable_entity->get_position().y) {
            //m_position.y += y_overlap;
            m_velocity.y = 0;
            m_collided_bottom = true;
        }
        //if (m_collided_entity_bottom)
            //if (m_entity_type == PLAYER) collidable_entity->deactivate();

        float x_distance = fabs(m_position.x - collidable_entity->get_position().x);
        float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
        if (m_position.x < collidable_entity->get_position().x) {
            //m_position.x -= x_overlap;
            m_velocity.x = 0;
            m_collided_right = true;
        }
        else if (m_position.x > collidable_entity->get_position().x) {
            //m_position.x += x_overlap;
            m_velocity.x = 0;
            m_collided_left = true;
        }

        double sensitivity = 2;

        if (y_overlap < sensitivity * x_overlap) {
            m_collided_right = false;
            m_collided_left = false;
        }
        else {
            m_collided_top = false;
            m_collided_bottom = false;
        }

        //log << m_collided_right << " " << m_collided_left << " " << m_collided_top << " " << m_collided_bottom << "\n";
    }

}



bool const Entity::check_collision(Entity* collidable_entity) {
    float x_distance = fabs(m_position.x - collidable_entity->get_position().x) - ((m_width + collidable_entity->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - collidable_entity->get_position().y) - ((m_height + collidable_entity->m_height) / 2.0f);
    if (x_distance < 0 && y_distance < 0)
    {
        return true;
    }
    return false;
}




void const Entity::set_direction(int new_direction){
    if (new_direction == UP && m_collided_bottom == false) {
        //ignore this possibility 
    }
    else {
        m_direction = new_direction;
    }
};

void Entity::clear_collision() {
    m_collided_left = false;
    m_collided_bottom = false;
    m_collided_right = false;
    m_collided_top = false;

    //pit avoidance 
    m_pit_left = false;
    m_pit_right = false;
}