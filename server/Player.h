#pragma once
#include "Position.h"
#include "DirectionEnum.h"


struct Player
{
	unsigned char id;
	bool is_ready;
	bool is_alive;
	Position position;
	DirectionEnum direction;

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
		this->position = starting_position;
	}
	
	void move(unsigned char x, unsigned char y)
	{
		position.x += x;
		position.y += y;
	}
};
