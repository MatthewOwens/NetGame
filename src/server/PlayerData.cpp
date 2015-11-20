#include "PlayerData.h"

PlayerData::PlayerData()
{
	position = sf::Vector2f(0.0f, 0.0f);
	velocity = sf::Vector2f(0.0f, 0.0f);
	state = (sf::Int32)AnimationState::IDLE;
	updateTime = 0.0f;
	atkTimer = 0.0f;
}

sf::Packet& operator >> (sf::Packet& packet, PlayerData& m)
{
	return packet >> m.clientID >> m.position.x >> m.position.y  >> m.velocity.x >> m.velocity.y >> m.state >> m.updateTime >> m.atkTimer;
}

sf::Packet& operator << (sf::Packet& packet, const PlayerData& m)
{
	// Not reading out the client id, since we extract that first server side
	return packet << m.clientID << m.position.x << m.position.y  << m.velocity.x << m.velocity.y << m.state << m.updateTime << m.atkTimer;
}
