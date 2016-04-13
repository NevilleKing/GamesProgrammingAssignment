#pragma once
#include "AnimatedSprite.h"

#include <iostream>
#include <string>

#include <SDL.h>
#include "Vector.h"
#include "Size.h"

class Player :
	public AnimatedSprite
{
public:
	Player(SDL_Renderer* ren, std::string imagePath, std::string JSONPath, Vector velocity1 = Vector(), Vector location = Vector(100, 100), Size size1 = Size(100, 100));
	~Player();

	void Jump();
	void MoveLeft();
	void MoveRight();

	void StopMovingLeft();

	void StopMovingRight();

	void Update(float time);

	void setOnGround();

	enum MoveState {LEFT = -1, IDLE = 0, RIGHT = 1};

private:
	bool _isJumping = false;
	bool _isOnGround = false;
	MoveState _state = IDLE;
	MoveState _afterJump = IDLE;
	float _yVelocity = 0;
};

