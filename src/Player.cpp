#include "Player.h"

#define MOVE_SPEED 200
#define GRAVITY 150

Player::Player(SDL_Renderer* ren, std::string imagePath, std::string JSONPath, Vector velocity1, Vector location, Size size1) : AnimatedSprite(ren, imagePath, JSONPath, velocity1, location, size1)
{
}

Player::~Player()
{
}

void Player::Jump()
{
	if (!_isJumping)
	{
		if (_isOnLadder)
		{
			changeState(_afterState);
		}
		else
			_afterState = _state;
		_isJumping = true;
		_isOnGround = false;
		_yVelocity = -100;
	}
}

void Player::MoveLeft()
{
	if (!_isJumping && !_isFalling) // when jumping no movement can occur
	{
		changeState(LEFT);
	}
	else
	{
		_afterState = LEFT;
	}
}

void Player::MoveRight()
{
	if (!_isJumping && !_isFalling) // when jumping no movement can occur
	{
		changeState(RIGHT);
	}
	else
	{
		_afterState = RIGHT;
	}
}

void Player::MoveUp()
{
	changeLadderState(UP);
}

void Player::MoveDown()
{
	changeLadderState(DOWN);
}

void Player::StopMovingLeft()
{
	if (!_isJumping && !_isFalling) // when jumping no movement can occur
	{
		if (_state == LEFT) changeState(IDLE);

	}
	else
	{
		if (_afterState == LEFT)  _afterState = IDLE;
	}
}

void Player::StopMovingRight()
{
	if (!_isJumping && !_isFalling) // when jumping no movement can occur
	{
		if (_state == RIGHT) changeState(IDLE);
	}
	else
	{
		if (_afterState == RIGHT)  _afterState = IDLE;
	}
}

void Player::StopMovingUp()
{
	if (_ladderState == UP) changeLadderState(IDLE);
}

void Player::StopMovingDown()
{
	if (_ladderState == DOWN) changeLadderState(IDLE);
}

void Player::setOnGround()
{
	if (_isJumping && _yVelocity > 0)
	{
		changeState(_afterState); // reset state just in case player was jumping left or right
		_isJumping = false;
	}
	else
		_isOnGround = true;

	if (_isFalling)
	{
		if (!_isOnLadder) changeState(_afterState);
		_isFalling = false;
	}
}

void Player::UpdateCollisions(std::vector<std::unique_ptr<LevelPiece>> &level, Size windowSize)
{
	// reset variables
	_isOnLadder = false;
	_isOnGround = false;

	// check level collisions
	for (int i = 0; i < level.size(); i++)
	{
		if (level[i]->isColliding(*this))
		{
			switch (level[i]->getType())
			{
			case LevelPiece::TileType::LADDER:
				_isOnLadder = true;
				break;
			case LevelPiece::TileType::EGG:
				level.erase(level.begin() + i);
				break;
			default:
				switch (this->checkCollisionDirection(*level[i]))
				{
				case Sprite::collisionDirection::DOWN:
					_isOnGround = true;
					break;
				case Sprite::collisionDirection::UP:
					if (_yVelocity < 0) _yVelocity = 0;
					break;
				case Sprite::collisionDirection::LEFT:
					if (_isJumping) changeState(RIGHT, false);
					else if (_state == LEFT)
						changeState(IDLE);
					break;
				case Sprite::collisionDirection::RIGHT:
					if (_isJumping) changeState(LEFT, false);
					else if (_state == RIGHT)
						changeState(IDLE);
					break;
				}
				break;
			}
		}
	}

	if (_isOnGround) this->setOnGround();

	// check collisions with window
	float minX = this->getPosition().x - (this->getSize().width / 2);
	float maxX = this->getPosition().x + (this->getSize().width / 2);
	float minY = this->getPosition().y + (this->getSize().height / 2);
	float maxY = this->getPosition().y - (this->getSize().height / 2);

	// left
	if (minX < 0)
	{
		if (_isJumping) changeState(RIGHT, false);
		else if (_state == LEFT)
			changeState(IDLE);
	}
	// right
	else if (maxX > windowSize.width)
	{
		if (_isJumping) changeState(LEFT, false);
		else if (_state == RIGHT)
			changeState(IDLE);
	}
}

void Player::Update(float time, std::vector<std::unique_ptr<LevelPiece>> &level, Size windowSize)
{
	UpdateCollisions(level, windowSize);

	_xVelocity = _state;
	if (!_isOnGround)
	{
		if (_isJumping)
		{
			if (_yVelocity < GRAVITY)
				_yVelocity += GRAVITY * time;
			else
				_yVelocity = GRAVITY;
		}
		else
		{
			_yVelocity = GRAVITY;
			if (!_isFalling) _afterState = _state;
			_isFalling = true;
		}
		if (!_isJumping && !_isOnLadder)
			changeState(IDLE);
		else if (_isOnLadder && _ladderState != IDLE)
			_state = IDLE;
	}
	else
	{
		if (_yVelocity > 0) _yVelocity = 0;
	}

	if (_isOnLadder && (!_isJumping || _ladderState != IDLE))
	{
		_isJumping = false;
		if (_isJumping) _afterState = IDLE;
		_yVelocity = MOVE_SPEED * _ladderState;
		if (_isOnGround && _yVelocity > 0) _yVelocity = 0;
		if (_isOnGround && _ladderState == DOWN) changeLadderState(IDLE);
	}

	setVelocity(Vector(MOVE_SPEED * _xVelocity, _yVelocity));

	// call base class Update function
	this->AnimatedSprite::Update(time);
}

void Player::changeState(Player::MoveState newState, bool flip)
{
	_state = newState;
	switch (_state)
	{
	case LEFT:
		changeAnimationCycle("WALK");
		if (flip) flipAnimation(false);
		break;
	case IDLE:
		if (flip) changeAnimationCycle("IDLE");
		break;
	case RIGHT:
		changeAnimationCycle("WALK");
		if (flip) flipAnimation(true);
	}
}

void Player::changeLadderState(Player::MoveState newState)
{
	_ladderState = newState;
	switch (_ladderState)
	{
	case UP:
	case DOWN:
		changeAnimationCycle("LADDER");
		break;
	case IDLE:
		changeAnimationCycle("LADDER_IDLE");
		break;
	}
}