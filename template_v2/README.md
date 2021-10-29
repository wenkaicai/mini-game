Heng Wu
31696198

Adde 2 stucts for AI
1. FishAi
2. TurtleAi

Changed the frequency for fish and turtle spawning.
Give fish random velocities, now the fish can have random velocities when they spawn.

Fish can bounce off the wall. When salmon hits the wall, it also bounce.

Defined helper function "createBox" in physics_system.cpp for debugging.

Creative part:
Turtle chases salmon with velocity based on the distance, further away is faster. When close to salmon (I set the distance to 300), turtle's velocity will be boosted, so salmon needs reaction to avoid turtles when they are close.
At the same time, turtles are able to avoid fish when chasing salmon and when they are moving.
When passing the salmon, turtle's velocity becomes normal.

Both FishAi and TurtleAi are handled in ai_system.cpp
