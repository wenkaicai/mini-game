// internal
#include "physics_system.hpp"
#include "world_init.hpp"

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Motion& motion1, const Motion& motion2)
{
	vec2 dp = motion1.position - motion2.position;
	float dist_squared = dot(dp, dp);
	const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

// helper function createBox for debugging
void createBox(vec2 position, vec2 size) {
	float scaleY = size.y * 0.05f;
	float scaleX = size.x * 0.05f;
	if (size.x >= size.y) {
		createLine(position + vec2(0, size.y / 2.0), vec2(size.y, scaleY));
		createLine(position + vec2(size.y / 2.0, 0.0), vec2(scaleX, size.y));

		createLine(position - vec2(0, size.y / 2.0), vec2(size.y, scaleY));
		createLine(position - vec2(size.y / 2.0, 0.0), vec2(scaleX, size.y));
	}
	else {
		createLine(position + vec2(0, size.x / 2.0), vec2(size.x, scaleX));
		createLine(position + vec2(size.x / 2.0, 0.0), vec2(scaleY, size.x));

		createLine(position - vec2(0, size.x / 2.0), vec2(size.x, scaleX));
		createLine(position - vec2(size.x / 2.0, 0.0), vec2(scaleY, size.x));
	}
}

void PhysicsSystem::step(float elapsed_ms, float window_width_px, float window_height_px)
{
	// Move fish based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++)
	{
		// !!! TODO A1: update motion.position based on step_seconds and motion.velocity
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = 1.0f * (elapsed_ms / 1000.f);
		(void)elapsed_ms; // placeholder to silence unused warning until implemented
		motion.position = motion.position + motion.velocity * step_seconds;
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A3: HANDLE PEBBLE UPDATES HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 3
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// Check for collisions between all moving entities
	ComponentContainer<Motion>& motion_container = registry.motions;
	for (uint i = 0; i < motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
		for (uint j = 0; j < motion_container.components.size(); j++) // i+1
		{
			if (i == j)
				continue;

			Motion& motion_j = motion_container.components[j];
			if (collides(motion_i, motion_j))
			{
				Entity entity_j = motion_container.entities[j];
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE SALMON - WALL collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// you may need the following quantities to compute wall positions
	(float)window_width_px; (float)window_height_px;
	for (uint i = 0; i < motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
		float left = motion_i.scale.x / -2;
		float right = window_width_px - motion_i.scale.x / -2;
		if (motion_i.position.x <= left) {
			motion_i.position.x = left;
		}
		else if (motion_i.position.x >= right) {
			motion_i.position.x = right;
		}
	}

	Entity player = registry.players.entities[0];
	Motion& player_m = registry.motions.get(player);

	// player collision with block
	for (uint i = 0; i < motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];


		if (!registry.block.has(entity_i)) {
			continue;
		}
		float playerScaleX = player_m.scale.x / -2;
		float playerScaleY = player_m.scale.y / 2;
		float blockScaleX = motion_i.scale.x / -2;
		float blockScaleY = motion_i.scale.y / 2;
		// when player is left to block
		if (player_m.position.x < motion_i.position.x) {
			if (player_m.position.x + playerScaleX >= motion_i.position.x - blockScaleX) {
				if (motion_i.position.y - player_m.position.y < playerScaleY + blockScaleY) {
					player_m.position.x = motion_i.position.x - blockScaleX - playerScaleX;
				}
			}
		}
		// when player is right to block
		if (player_m.position.x > motion_i.position.x) {
			if (player_m.position.x - playerScaleX <= motion_i.position.x + blockScaleX) {
				if (motion_i.position.y - player_m.position.y < playerScaleY + blockScaleY) {
					player_m.position.x = motion_i.position.x + blockScaleX + playerScaleX;
				}
			}
		}

		// when player is jumping
		/*if (player_m.position.y + playerScaleY <= (motion_i.position.y - blockScaleY)) {
			// in bound of block
			if ((player_m.position.x + playerScaleX) >= motion_i.position.x - blockScaleX) {
				if (player_m.position.x - playerScaleX <= motion_i.position.x + blockScaleX) {
					if (player_m.position.y + playerScaleY == motion_i.position.y - blockScaleY) {
					player_m.position.y = motion_i.position.y - blockScaleY - playerScaleY;
					}
				}
			}
		}

		// jump
		if (player_m.isJumping) {
			player_m.position.y -= player_m.gravity;
		} else {
			player_m.position.y += player_m.gravity;
		}
		// limit jump when not on blocks
		if (player_m.position.x + playerScaleX + motion_i.scale.x <= motion_i.position.x || player_m.position.x >= motion_i.position.x + blockScaleX + playerScaleX) {
			if (player_m.position.y >= 700) {
				player_m.position.y = 700;
				player_m.notJumping = true;
			}
			if (player_m.position.y <= 500) {
				player_m.isJumping = false;
			}
		}
		if (motion_i.position.x - player_m.position.x <= playerScaleX + blockScaleX && player_m.position.x - motion_i.position.x <= playerScaleX + blockScaleX) {
			if (player_m.position.y <= motion_i.scale.y - 200) {
				if (player_m.position.x + playerScaleX + motion_i.scale.x <= motion_i.position.x || player_m.position.x >= motion_i.position.x + blockScaleX + playerScaleX) {
					player_m.isJumping = false;
				}
			}
			if (motion_i.position.x - player_m.position.x <= playerScaleX + blockScaleX && player_m.position.x - motion_i.position.x <= playerScaleX + blockScaleX) {
				if (player_m.position.y >= motion_i.scale.y + playerScaleY) {
					player_m.position.y = motion_i.scale.y + playerScaleY;
					player_m.notJumping = true;
				}
			}
			if (player_m.position.x + playerScaleX + motion_i.scale.x <= motion_i.position.x || player_m.position.x >= motion_i.position.x + blockScaleX + playerScaleX) {
				if (player_m.position.y >= 700) {
					player_m.position.y = 700;
					player_m.notJumping = true;
				}
			}
		}*/
		// player jump
		if (player_m.isJumping) {
			player_m.position.y -= player_m.gravity;
		}
		else {
			player_m.position.y += player_m.gravity;
		}
		// jump limit
		if (player_m.position.y >= 700) {
			player_m.position.y = 700;
			player_m.notJumping = true;
		}
		if (player_m.position.y <= 500) {
			player_m.isJumping = false;
		}
	}


	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: DRAW DEBUG INFO HERE on Salmon mesh collision
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will want to use the createLine from world_init.hpp
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// debugging of bounding boxes
	if (debugging.in_debug_mode)
	{
		uint size_before_adding_new = (uint)motion_container.components.size();
		for (uint i = 0; i < size_before_adding_new; i++)
		{
			Motion& motion_i = motion_container.components[i];
			Entity entity_i = motion_container.entities[i];

			// visualize the radius with two axis-aligned lines
			const vec2 bonding_box = get_bounding_box(motion_i);
			float radius = sqrt(dot(bonding_box / 2.f, bonding_box / 2.f));
			vec2 line_scale1 = { motion_i.scale.x / 10, 2 * radius };
			Entity line1 = createLine(motion_i.position, line_scale1);
			vec2 line_scale2 = { 2 * radius, motion_i.scale.x / 10 };
			Entity line2 = createLine(motion_i.position, line_scale2);

			// !!! TODO A2: implement debugging of bounding boxes and mesh
			createBox(motion_i.position, motion_i.scale);
		}
	}


	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A3: HANDLE PEBBLE collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 3
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}