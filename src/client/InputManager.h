#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window.hpp>
#include <map>

class InputManager
{
    public:
        InputManager();
        virtual ~InputManager();
        void update(sf::RenderWindow& window);
        bool pressedOnce(std::string keyName);
		bool pressedOnce(sf::Mouse::Button button);
		bool keyHeld(std::string keyName);
        bool validKeyDown();
		inline sf::Vector2f getMousePosition() { return mousePosition; }
		inline sf::Vector2f getPrevMousePosition() { return previousMousePosition; }
    private:
        bool pressedKeys[sf::Keyboard::KeyCount];
		bool pressedMouseButtons[sf::Mouse::ButtonCount];
		bool previousPressedMouseButtons[sf::Mouse::ButtonCount];
        bool previousPressedKeys[sf::Keyboard::KeyCount];
        std::map<std::string, sf::Keyboard::Key> keyBinds;
		sf::Vector2f mousePosition;
		sf::Vector2f previousMousePosition;
};

#endif // INPUTMANAGER_H

