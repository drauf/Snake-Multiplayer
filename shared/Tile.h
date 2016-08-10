#pragma once

enum TileTypeEnum
{
	EMPTY,
	CURRENT_PLAYER,
	ANOTHER_PLAYER
};

struct Tile
{
	unsigned char x;
	unsigned char y;
	TileTypeEnum type;

	Tile()
	{
		this->x = 0;
		this->y = 0;
		this->type = EMPTY;
	}

	Tile(unsigned char x, unsigned char y, TileTypeEnum type)
	{
		this->x = x;
		this->y = y;
		this->type = type;
	}
};
