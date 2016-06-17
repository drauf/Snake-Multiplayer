#pragma once

struct Position
{
	unsigned char x;
	unsigned char y;

	Position()
	{
		this->x = 0;
		this->y = 0;
	}

	Position(unsigned char x, unsigned char y)
	{
		this->x = x;
		this->y = y;
	}
};
