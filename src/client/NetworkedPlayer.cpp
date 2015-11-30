#include "NetworkedPlayer.h"
#include <SFML/System/Vector2.hpp>
#include <math.h>

NetworkedPlayer::NetworkedPlayer(PlayerData initialData)
{
	data = initialData;
	interp = NONE;

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

void NetworkedPlayer::update()
{
	bool attackingRight = true;

	if (sprite.getPosition().x > atkSprite.getPosition().x)
		attackingRight = false;

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

	switch(interp)
	{
		case NONE:
		{
			// All good
			break;
		}
		case DESYNC:
		{
			// Calculating the velocity required to sync the NetworkPlayer to the
			// new data
			sf::Vector2f vel;
			velocity.x = (data.position.x - position.x);
			velocity.y = (data.position.y - position.y);

			float magnitude = std::sqrt( std::pow(data.position.x, 2) + std::pow(data.position.y, 2));

			// Normalising the velocity
			velocity /= magnitude;

			// Increasing the speed
			velocity *= 2.0f;
			break;
		}
		case PREDICTION:
		{
			// Interpolate position for this frame from the history + serverTime()
			break;
		}
	};

	// Updating our sprites
	sprite.move(velocity);
	atkSprite.setPosition(sprite.getPosition());
	position += velocity;

	// Updating our atkSprite
	if(velocity.x > 0)
	{
		atkSprite.move(0.0f, 32.0f);
	}
	else if(velocity.x < 0)
	{
		atkSprite.move(0.0f, -32.0f);
	}
	else	// If we wern't moving, use info from last frame
	{
		if(attackingRight)
			atkSprite.move(0.0f, 32.0f);
		else
			atkSprite.move(0.0f, -32.0f);
	}
}

void NetworkedPlayer::updateData(PlayerData newData)
{
	bool attackRight = true;
	interp = DESYNC;

	// Updating our history
	history.push_back(data);

	if(history.size() >= MAX_HISTORY)
		history.pop_front();

	// Updating our data with what we've recieved from the server
	data = newData;

}

void NetworkedPlayer::render(sf::RenderWindow& window)
{
	window.draw(sprite);
	window.draw(atkSprite);
}
