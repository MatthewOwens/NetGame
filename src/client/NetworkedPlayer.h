#ifndef NETWORKED_PLAYER
#define NETWORKED_PLAYER
#define MAX_HISTORY 2

#include "PlayerData.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <deque>
class NetworkedPlayer
{
public:
	enum InterpolationState { NONE, DESYNC, PREDICTION };
	NetworkedPlayer(PlayerData initialData);
	~NetworkedPlayer();
	void updateData(PlayerData newData);
	void update();
	void render(sf::RenderWindow& window);

	inline const PlayerData& getData() { return data; };
	inline void setData(PlayerData newData) { data = newData; }
	inline sf::RectangleShape getAtkSprite() { return atkSprite; }
	inline float getUpdateTime() { return data.updateTime; }
private:
	PlayerData data;
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::RectangleShape sprite;
	sf::RectangleShape atkSprite;
	std::deque<PlayerData> history;

	InterpolationState interp;
};
#endif//NETWORKED_PLAYER
