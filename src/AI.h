#pragma once

#include "Character.h"
#include "TileMap.h"

#include <stdlib.h>
#include <time.h>

#include <chrono>

using namespace std::chrono;

class AI :
	public Character
{
public:
	AI(SDL_Renderer* ren, std::string imagePath, std::string JSONPath, TileMap* tm, Vector velocity1 = Vector(), Vector location = Vector(100, 100), Size size1 = Size(100, 100));
	~AI();

	void Update(float time, std::vector<std::unique_ptr<LevelPiece>> &level, Size windowSize);

	bool checkIfReachedDestination();

	void ChooseNextDestination();

private:
	Character::MoveState _moveDirection = LEFT;

	TileMap* _tilemap;
	bool _onLadder = false;
	bool _horizontal = true;

	Vector _destination;
	Vector currentPos = Vector(0, 25);
};

