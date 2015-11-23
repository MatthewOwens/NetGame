#ifndef NETWORKED_PLAYER
#define NETWORKED_PLAYER
#include "PlayerData.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
class NetworkedPlayer
{
public:
	NetworkedPlayer(PlayerData initialData);
	~NetworkedPlayer();
	void update(PlayerData newData);
	void render(sf::RenderWindow& window);

	inline const PlayerData& getData() { return data; };
	inline void setData(PlayerData newData) { data = newData; }
	inline sf::RectangleShape getAtkSprite() { return atkSprite; }
	inline float getUpdateTime() { return data.updateTime; }
private:
	PlayerData data;
	sf::RectangleShape sprite;
	sf::RectangleShape atkSprite;
};
#endif//NETWORKED_PLAYER
