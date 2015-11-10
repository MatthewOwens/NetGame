#include "InputManager.h"
#include <iostream>

InputManager::InputManager()
{
    keyBinds["up"] = sf::Keyboard::Up;
    keyBinds["down"] = sf::Keyboard::Down;
    keyBinds["left"] = sf::Keyboard::Left;
    keyBinds["right"] = sf::Keyboard::Right;
    keyBinds["confirm"] = sf::Keyboard::Return;
    keyBinds["cancel"] = sf::Keyboard::BackSpace;
	keyBinds["close"] = sf::Keyboard::Escape;
	keyBinds["melee"] = sf::Keyboard::Z;
	keyBinds["shoot"] = sf::Keyboard::X;
}

InputManager::~InputManager()
{
    //dtor
}

void InputManager::update(sf::RenderWindow& window)
{
	// Updating the keyboard state
   	for(int i = 0; i < sf::Keyboard::KeyCount; i++)
   	{
       	previousPressedKeys[i] = pressedKeys[i];
       	pressedKeys[i] = sf::Keyboard::isKeyPressed( (sf::Keyboard::Key)i );
   	}

	// Updating the state of the mouse buttons
	for(int i = 0; i < sf::Mouse::ButtonCount; i++)
	{
		previousPressedMouseButtons[i] = pressedMouseButtons[i];
		pressedMouseButtons[i] = sf::Mouse::isButtonPressed( (sf::Mouse::Button)i );
	}

	// Updating the state of the mouse position
	previousMousePosition = mousePosition;
	//mousePosition = sf::Mouse::getPosition(window);
	sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
	mousePosition = window.mapPixelToCoords(pixelPos);
}

bool InputManager::keyHeld(std::string keyName)
{
	return pressedKeys[keyBinds[keyName]];
}

bool InputManager::pressedOnce(std::string keyName)
{
    if(pressedKeys[keyBinds[keyName]] && !previousPressedKeys[keyBinds[keyName]])
        return true;
    else return false;
}

bool InputManager::pressedOnce(sf::Mouse::Button button)
{
	if(pressedMouseButtons[button] && !previousPressedMouseButtons[button])
		return true;
	else return false;
}

bool InputManager::validKeyDown()
{
    std::map<std::string, sf::Keyboard::Key>::iterator mapItr;

    for(mapItr = keyBinds.begin(); mapItr != keyBinds.end(); ++mapItr)
    {
        if(pressedOnce(mapItr->first) &&
           (mapItr->first != "confirm" || mapItr->first != "cancel"))
        {
            return true;
        }
    }
    return false;
}
