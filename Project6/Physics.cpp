#include "Physics.h"

void Physics::apply_gravity(std::vector<Entity*>& entities) {
	for (Entity* entity : entities) {
		entity->accelerate(m_gravity);
	}

}


void Physics::apply_constraint(std::vector<Entity*>& entities) {
	for (Entity* entity : entities) {
		glm::vec3 v = constraint.m_position - entity->m_curr_position;
		const float        dist = glm::length(v);
		if (dist > (constraint.m_radius - entity->m_radius)) {
			glm::vec3 n = v / dist;

			entity->m_curr_position = constraint.m_position - n * (constraint.m_radius - entity->m_radius);
		}
	}

}

void Physics::check_collisions(std::vector<Entity*>& entities)
{
    const float    response_coef = 0.75f;
    const uint64_t objects_count = entities.size();

    // Iterate on all objects
    //simple but effective n^2 algorithm. can consider optimizing in the future 
    for (uint64_t i{ 0 }; i < objects_count; ++i) {
        Entity* object_1 = entities[i];

        // Iterate on object involved in new collision pairs
        for (uint64_t k{ i + 1 }; k < objects_count; ++k) {
            Entity* object_2 = entities[k];

            glm::vec3 v = object_1->m_curr_position - object_2->m_curr_position;
            float dist = glm::length(v);
            float min_dist = object_1->m_radius + object_2->m_radius;
            // Check overlapping

            if (dist < min_dist) { //overlaps in distances 
                glm::vec3 n = v / dist; //the vector from 1 to 2 normalized 

                const float mass_ratio_1 = object_1->m_radius / (object_1->m_radius + object_2->m_radius);
                const float mass_ratio_2 = object_2->m_radius / (object_1->m_radius + object_2->m_radius);
                const float delta = 0.5f * response_coef * (dist - min_dist);

                // Update positions to prevent clipping 
                object_1->m_curr_position -= n * (mass_ratio_2 * delta);
                object_2->m_curr_position += n * (mass_ratio_1 * delta);
            }
        }
    }
}