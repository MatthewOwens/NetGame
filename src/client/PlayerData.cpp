#include "PlayerData.h"

PlayerData::PlayerData()
{
	position = sf::Vector2f(0.0f, 0.0f);
	velocity = sf::Vector2f(0.0f, 0.0f);
	state = (sf::Int32)AnimationState::IDLE;
	updateTime = 0.0f;
	atkTimer = 0.0f;
	clientID = 0;
}

sf::Packet& operator >> (sf::Packet& packet, PlayerData& m)
{
	return packet >> m.position.x >> m.position.y  >> m.velocity.x >> m.velocity.y >> m.state >> m.updateTime >> m.atkTimer;
}

sf::Packet& operator << (sf::Packet& packet, const PlayerData& m)
{
	return packet << m.position.x << m.position.y  << m.velocity.x << m.velocity.y << m.state << m.updateTime << m.atkTimer;
}
