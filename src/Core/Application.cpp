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
    float midHeight = Settings::instance.height / 2;
    bool animate = false;
    bool render2D = true;

    while (p_window->isOpen())
    {
        while (p_window->pollEvent(m_event))
        {
            ImGui::SFML::ProcessEvent(*p_window, m_event);
            if (m_event.type == sf::Event::Closed)
            {
                ImGui::DestroyContext();
                p_window->close();
            }           
        }   
      
        if (!render2D)
        {
            m_image.create(Settings::instance.width, Settings::instance.height, sf::Color::Black);
            for (float x = 0; x < Settings::instance.width; x++)
            {
                float input = x / Settings::instance.width * perlin.random.size();
                m_image.setPixel(
                    x,
                    midHeight - perlin.noise1D(input) * 100,
                    sf::Color::White
                );

                m_image.setPixel(
                    x,
                    midHeight,
                    sf::Color::Red
                );
            }
        }
        
        if (animate)
        {
            perlin.offset.x -= m_deltaTime.asSeconds() * 10;
        }

        m_deltaTime = m_deltaClock.restart();
        ImGui::SFML::Update(*p_window, m_deltaTime);
        m_texture.loadFromImage(m_image);
        m_sprite.setTexture(m_texture);

        p_window->clear();
        p_window->draw(m_sprite);

        ImGui::Begin("Settings");
            ImGui::Checkbox("Animate", &animate);
            ImGui::Checkbox("Render for image (allow noise output greater than 1)", &perlin.renderForImage);
            ImGui::DragFloat("Amplitude", &perlin.amplitude, 0.25, -100, 100);
            ImGui::DragFloat("Frequency", &perlin.frequency, 0.001, -100, 100);
            ImGui::DragFloat("Lacunarity", &perlin.lacunarity, 0.01, -5, 5);
            ImGui::DragFloat("Persistance", &perlin.persistance, 0.01, -5, 5);
            ImGui::SliderInt("Octaves", &perlin.octaves, 1, 10);
            ImGui::DragFloat2("Offset", &perlin.offset.x, 5, -10, -10);     
            ImGui::Checkbox("Render 2D", &render2D);
            if (render2D)
            {
                if (ImGui::Button("Render"))
                {
                    for (float y = 0; y < Settings::instance.height; y++)
                    {
                        for (float x = 0; x < Settings::instance.width; x++)
                        {
                            Vec2 input(
                                x / Settings::instance.width * perlin.random.size(),
                                y / Settings::instance.height * perlin.random.size()
                            );

                            float color = perlin.noise2D(input) * 255;
                            m_image.setPixel(
                                x,
                                y,
                                sf::Color(color, color, color, 255)
                            );
                        }
                    }
                }
            }
        ImGui::End();

        ImGui::SFML::Render(*p_window);
        p_window->display();
    }

    return 0;
}