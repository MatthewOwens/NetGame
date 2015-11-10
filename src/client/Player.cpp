#include "Player.h"
#include <iostream>

// Default constructor, just ensures that pointers are nulled
Player::Player()
{
	inputManager = NULL;
	playerID = 0;
}

Player::Player(InputManager* input)
{
	inputManager = input;
	playerID = 0;

	// Initilising our sprite
	sprite.setSize(sf::Vector2f(32.0f, 32.0f));

	switch (playerID)
	{
		// TODO: Place each player in different positions initially
		// TODO: Give each player different colors initially
	default:
		playerData.position = sf::Vector2f(32.0f, 32.0f);
		sprite.setPosition(playerData.position);
		sprite.setFillColor(sf::Color::Red);
	}
}

Player::~Player()
{
}

void Player::update(Tile** tiles)
{
	// Don't update if pointers are null
	if (inputManager == NULL)
		return;

	// Updating the velocity based on input
	if (inputManager->keyHeld("left"))
	{
		playerData.velocity.x = -speed;
		playerData.state = (sf::Int32)playerData.LEFT;
	}
	else if (inputManager->keyHeld("right"))
	{
		playerData.velocity.x = speed;
		playerData.state = (sf::Int32)playerData.RIGHT;
	}
	else
	{
		playerData.velocity.x = 0.0f;
		playerData.state = (sf::Int32)playerData.IDLE;
	}

	// Making the player fall if they're in the air and not jumping
	if (playerData.state != (sf::Int32)playerData.JUMP)
	{
		int gridX = playerData.position.x / Tile::getSize();
		int gridY = playerData.position.y / Tile::getSize();

		// Checking what tile's under us
		if (tiles[gridX][gridY + 1].getID() == 0)
		{
			playerData.velocity.y = speed * 2;
		}
		else playerData.velocity.y = 0.0f;
	}
	else
	{
		// TODO: Apply gravity, check for landings etc
	}

	// Moving based on velocity
	sprite.move(playerData.velocity);
	playerData.position = sprite.getPosition();
}

void Player::render(sf::RenderWindow& window)
{
	window.draw(sprite);
}