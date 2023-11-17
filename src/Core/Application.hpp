#pragma once
#include <SFML/Graphics.hpp>
#include "Perlin.hpp"

class Application
{
private:
	Application();
	const char* NoiseTypeStr(NoiseType type);

public:
	static Application* instance() noexcept;
	int run();

private:
	static Application* s_app;
	sf::RenderWindow* p_window;
	sf::Clock m_deltaClock;
	sf::Time m_deltaTime;
	sf::Event m_event;

	sf::Image m_image;
	sf::Texture m_texture;
	sf::Sprite m_sprite;	
};