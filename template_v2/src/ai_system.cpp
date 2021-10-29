// internal
#include "ai_system.hpp"
#include "world_init.hpp"


void AISystem::step(int height)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE FISH AI HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will likely want to write new functions and need to create
	// new data structures to implement a more sophisticated Fish AI.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	//(void)elapsed_ms; // placeholder to silence unused warning until implemented
	ComponentContainer<Motion>& motion_container = registry.motions;
	Entity player = registry.players.entities[0];
	Motion& player_m = registry.motions.get(player);
	
	// enemyAI trcks player position and moves to player
	for (uint i = 0; i < motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];


		if (!registry.enemyAi.has(entity_i)) {
			continue;
		}
		float dist;
		dist = player_m.position.x - motion_i.position.x;
		if (dist > 0) {
			motion_i.velocity.x = 200;
		}
		if (dist == 0) {
			motion_i.velocity.x = 0;
		}
		if (dist < 0) {
			motion_i.velocity.x = -200;
		}
	}
}
	
