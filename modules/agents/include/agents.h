#pragma once

#include "transform.h"

struct agents
{
	struct Agent
	{
		transform::Color color;
	};

	struct Age
	{
		int age;
	};

	struct CarryingCapacity
	{
		int maxPop;
	};


	struct TotalPop {
		size_t value;
	};

	enum SystemType {
		Main,
		Draw
	};

	struct PopGained {};
	struct PopLost {};

	struct Impassable {};

	agents(flecs::world& world);

	/**
	*	@brief Create an agent with the same color as a given agent, with the same parent as that agent. 
	*	@param world The world to create the agent in.
	*	@param copy The agent to get the parent and color from.
	*	@return The created agent's entity.
	*/
	static flecs::entity makeAgent(flecs::world& world, const agents::Agent& copy);

	/**
	*	@brief create an agent with a random color and set it to a child of a given entity.
	*	@param world The world to create the agent in.
	*	@param parent The entity to be the parent of the agent. 
	*	@return The created agent's entity. 
	*/
	static flecs::entity makeRandomAgent(flecs::world& world, flecs::entity parent);


	/**
	*	@brief Clear all agents from the world.
	*	@param world The ecs world being used. 
	*/
	static void clearAllAgents(flecs::world& world);

	static void destroyAgent(flecs::entity& agent);
	static void fireLostAgentEvent(flecs::world& world);
	static void fireNewAgentEvent(flecs::world& world);

	/**
	*	@brief Stop all systems that control agents. Effectively pauses agents. 
	*	@param world The ecs world being used.
	*/
	static void stopAgentSystems(flecs::world& world);

	/**
	*	@brief Start all systems that control agents. Effectively unpauses agents.
	*	@param world The ecs world being used. 
	*/
	static void startAgentSystems(flecs::world& world);

	/**
	*	@brief Clear all impassible squares from the world.
	*	@param world The ecs world being used. 
	*/
	static void clearImpassableSquares(flecs::world& world);

};
