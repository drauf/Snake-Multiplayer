#pragma once
#include "Position.h"
#include "DirectionEnum.h"


#define MAX_SNAKE_LENGTH 200


struct Player
{
	unsigned char id;
	bool is_ready;
	bool is_alive;
	DirectionEnum direction;
	Position positions[MAX_SNAKE_LENGTH];

	Player()
	{
		this->id = 0;
		this->is_ready = false;
		this->is_alive = false;
		this->direction = RIGHT;
	}

	Player(unsigned char id, DirectionEnum direction, Position starting_position)
	{
		this->id = id;
		this->is_ready = false;
		this->is_alive = true;
		this->direction = direction;
		this->positions[0] = starting_position;
	}
};
