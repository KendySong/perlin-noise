#include <iostream>

#include <ImGui/imgui.h>
#include <ImGui/imgui-SFML.h>

#include "Application.hpp"
#include "../Settings.hpp"
#include "Perlin.hpp"

Application* Application::s_app = nullptr;
Application::Application()
{
    p_window = new sf::RenderWindow(sf::VideoMode(Settings::instance.width, Settings::instance.height), "Barnes Hut Simulation");
    ImGui::SFML::Init(*p_window);
}

Application* Application::instance() noexcept
{
    if (s_app == nullptr)
    {
        s_app = new Application();
    }
    return s_app;
}

int Application::run()
{
    m_image.create(Settings::instance.width, Settings::instance.height, sf::Color::Black);
    m_texture.loadFromImage(m_image);
    m_sprite = sf::Sprite(m_texture, sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(Settings::instance.width, Settings::instance.height)));

    Perlin perlin;
    for (size_t x = 0; x < Settings::instance.width; x++)
    {
        m_image.setPixel(x, Settings::instance.height / 2 + perlin.noise1D(x), sf::Color::White);
    }
    
    while (p_window->isOpen())
    {
        while (p_window->pollEvent(m_event))
        {
            ImGui::SFML::ProcessEvent(*p_window, m_event);
            if (m_event.type == sf::Event::Closed)
            {
                p_window->close();
            }           
        }   

        ImGui::SFML::Update(*p_window, m_deltaClock.restart());
        m_texture.loadFromImage(m_image);
        m_sprite.setTexture(m_texture);

        p_window->clear();
        p_window->draw(m_sprite);

        ImGui::Begin("Settings");
        ImGui::End();

        ImGui::SFML::Render(*p_window);
        p_window->display();
    }

    return 0;
}