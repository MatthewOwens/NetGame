#include "PlayerData.h"

PlayerData::PlayerData()
{
	position = sf::Vector2f(0.0f, 0.0f);
	velocity = sf::Vector2f(0.0f, 0.0f);
	state = (sf::Int32)AnimationState::IDLE;
	updateTime = sf::Time::Zero.asMilliseconds();
}

sf::Packet& operator >> (sf::Packet& packet, PlayerData& m)
{
	return packet >> m.position.x >> m.position.y  >> m.velocity.x >> m.velocity.y >> m.state >> m.updateTime;
}

sf::Packet& operator << (sf::Packet& packet, const PlayerData& m)
{
	return packet << m.position.x << m.position.y  << m.velocity.x << m.velocity.y << m.state << m.updateTime;
}
