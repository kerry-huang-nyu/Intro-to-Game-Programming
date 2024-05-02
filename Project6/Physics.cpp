#include "Physics.h"


void Physics::apply_gravity(std::vector<Entity*>& entities) {
	for (Entity* entity : entities) {
		entity->accelerate(m_gravity);
	}

}

void Physics::bounded_velocity(Entity* entity, float dt) {

    glm::vec3 change = entity->m_curr_position - entity->m_prev_position;
    glm::vec3 velocity = change / dt; 

    if (glm::length(velocity) > 1) {
        velocity = velocity / glm::length(velocity);
    }
    change = velocity * dt;

    entity->m_prev_position = entity->m_curr_position - change;
}


void Physics::apply_constraint(std::vector<Entity*>& entities, float dt) {
	for (Entity* entity : entities) {
		glm::vec3 v = constraint.m_position - entity->m_curr_position;
		const float        dist = glm::length(v);

		if (dist > (constraint.m_radius - entity->m_radius)) { //under collision with the constraint 
			glm::vec3 n = v / dist;

			entity->m_curr_position = constraint.m_position - n * (constraint.m_radius - entity->m_radius);
            //current position is now brought back in line 
            // 
            
            //bounded velocity to make sure the velocity through mergers are not too much 
            bounded_velocity(entity, dt);
            //entity->m_prev_position = entity->m_curr_position; 
		}
	}

}

std::vector<Info> Physics::check_collisions(std::vector<Entity*>& entities)
{
    const float    response_coef = 0.2f;
    const uint64_t objects_count = entities.size(); //only iterate through the objects currently in our system 

    std::vector<Info> spawn;

    // Iterate on all objects
    //simple but effective n^2 algorithm. can consider optimizing in the future 
    for (uint64_t i{ 0 }; i < objects_count; ++i) {
        Entity* object_1 = entities[i];

        if (object_1->m_status == ALIVE) {

            // Iterate on object involved in new collision pairs
            for (uint64_t k{ i + 1 }; k < objects_count; ++k) {
                Entity* object_2 = entities[k];

                if (object_2->m_status == ALIVE) {
                    glm::vec3 v = object_1->m_curr_position - object_2->m_curr_position;
                    float dist = glm::length(v);
                    float min_dist = object_1->m_radius + object_2->m_radius;
                    // Check overlapping

                    if (dist < min_dist) { //overlaps in distances 

                        //if they are different types, give them collisions 
                        if (object_1->get_index() != object_2->get_index()) {
                            glm::vec3 n = v / dist; //the vector from 1 to 2 normalized 

                            const float mass_ratio_1 = object_1->m_radius / (object_1->m_radius + object_2->m_radius);
                            const float mass_ratio_2 = object_2->m_radius / (object_1->m_radius + object_2->m_radius);
                            const float delta = 0.5f * response_coef * (dist - min_dist);

                            // Update positions to prevent clipping 
                            object_1->m_curr_position -= n * (mass_ratio_2 * delta);
                            object_2->m_curr_position += n * (mass_ratio_1 * delta);

                        }
                        else {
                            //kill both objects 
                            object_1->set_status(DEAD);
                            object_2->set_status(DEAD);

                            //spawn a new object 
                            //find the middle of the two objects 
                            glm::vec3 newloc = (object_1->get_position() + object_2->get_position()) / 2.0f;
                            float x = newloc.x;
                            float y = newloc.y;

                            int index = (object_1->m_index + 1) % 6;

                            spawn.emplace_back(Info{x, y, index});

                            break; //leave this for loop since the first object is dead 
                        }
                    }
                }
            }
        }
    }

    return spawn;
}