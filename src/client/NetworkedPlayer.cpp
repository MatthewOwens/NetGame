#include "NetworkedPlayer.h"
#include <SFML/System/Vector2.hpp>

NetworkedPlayer::NetworkedPlayer(PlayerData initialData)
{
	data = initialData;

	// Constructing our boxes based on that initial data
	sprite.setPosition(data.position);
	sprite.setSize(sf::Vector2f(32.0f, 32.0f));

	atkSprite.setPosition(data.position);
	atkSprite.setSize(sf::Vector2f(32.0f, 32.0f));
	atkSprite.move(sf::Vector2f(32.0f, 0));
	atkSprite.setFillColor(sf::Color(0, 0, 0, 0));

	// Assigning each player a different color
	switch (data.clientID)
	{
	default:
		// Shouldn't be called, since we can't have more than 4 clients
		sprite.setFillColor(sf::Color(255, 255, 255));
		break;
	case 0:
		sprite.setFillColor(sf::Color(255, 0, 0));
		break;
	case 1:
		sprite.setFillColor(sf::Color(0, 255, 0));
		break;
	case 2:
		sprite.setFillColor(sf::Color(0, 0, 255));
		break;
	case 3:
		sprite.setFillColor(sf::Color(173, 173, 0));
		break;
	}
}

void NetworkedPlayer::update(PlayerData newData)
{
	bool attackRight = true;

	// Updating our data with what we've recieved from the server
	data = newData;

	// Checking if we were attacking right last frame
	if (sprite.getPosition().x > atkSprite.getPosition().x)
		attackRight = false;

	// Ensuring that our sprite positions are correct
	sprite.setPosition(data.position);
	atkSprite.setPosition(data.position);

	// Setting the position of our atkSprite based on velocity
	if (data.velocity.x > 0)
	{
			atkSprite.move(sf::Vector2f(32.0f, 0.0f));
	}
	else if (data.velocity.x < 0)
	{
			atkSprite.move(sf::Vector2f(-32.0f, 0.0f));
	}
	else
	{
		// If the x velocity is zero, assume no change from the last frame
		if (attackRight)
			atkSprite.move(sf::Vector2f(32.0f, 0.0f));
		else
			atkSprite.move(sf::Vector2f(-32.0f, 0.0f));
	}

	if (data.atkTimer <= 0.5f && data.atkTimer != 0)
	{
		atkSprite.setFillColor(sf::Color(0, 125, 125, 100));
	}

	if (data.atkTimer >= 0.6f)
	{
		atkSprite.setFillColor(sf::Color(255, 0, 0, 100));
	}

	if (data.atkTimer >= 0.7f || data.atkTimer == 0)
	{
		atkSprite.setFillColor(sf::Color(0, 0, 0, 0));
	}
}

void NetworkedPlayer::render(sf::RenderWindow& window)
{
	window.draw(sprite);
	window.draw(atkSprite);
}