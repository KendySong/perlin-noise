#include <iostream>

#include <ImGui/imgui.h>
#include <ImGui/imgui-SFML.h>

#include "Application.hpp"
#include "../Settings.hpp"

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
    Vec2 speedDirection(1, 0);

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
      
        if (perlin.type == NoiseType::Perlin1D)
        {
            m_image.create(Settings::instance.width, Settings::instance.height, sf::Color::Black);
            for (float x = 0; x < Settings::instance.width; x++)
            {
                float input = x / Settings::instance.width * perlin.random.size();
                m_image.setPixel(
                    x,
                    midHeight + perlin.noise1D(input) * 100,
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
            perlin.offset += speedDirection * m_deltaTime.asSeconds();
        }

        m_deltaTime = m_deltaClock.restart();
        ImGui::SFML::Update(*p_window, m_deltaTime);
        m_texture.loadFromImage(m_image);
        m_sprite.setTexture(m_texture);

        p_window->clear();
        p_window->draw(m_sprite);

        ImGui::Begin("Settings");
            ImGui::Checkbox("Animate", &animate);
            if (animate)
            {
                ImGui::DragFloat2("Input offset", &speedDirection.x, 0.1);
            }

            ImGui::Checkbox("Render for image (allow noise output greater than 1)", &perlin.renderForImage);
            ImGui::DragFloat("Amplitude", &perlin.amplitude, 0.25, -100, 100);
            ImGui::DragFloat("Frequency", &perlin.frequency, 0.001, -100, 100);
            ImGui::DragFloat("Lacunarity", &perlin.lacunarity, 0.01, -5, 5);
            ImGui::DragFloat("Persistance", &perlin.persistance, 0.01, -5, 5);
            ImGui::SliderInt("Octaves", &perlin.octaves, 0.01, 10);
            ImGui::DragFloat2("Offset", &perlin.offset.x, 5, -10, -10);  

            if (ImGui::BeginCombo("Render Type", NoiseTypeStr(perlin.type)))
            {
                for (size_t i = 0; i < static_cast<int>(NoiseType::End); i++)
                {
                    bool isSelect = i == static_cast<int>(perlin.type);
                    if (ImGui::Selectable(NoiseTypeStr(static_cast<NoiseType>(i)), isSelect))
                    {
                        perlin.type = static_cast<NoiseType>(i);
                    }

                    if (isSelect)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::BeginCombo("Lerp type", LerpTypeStr(perlin.lerp)))
            {
                for (size_t i = 0; i < static_cast<int>(LerpType::End); i++)
                {
                    bool isSelect = i == static_cast<int>(perlin.lerp);
                    if (ImGui::Selectable(LerpTypeStr(static_cast<LerpType>(i)), isSelect))
                    {
                        perlin.lerp = static_cast<LerpType>(i);
                    }

                    if (isSelect)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            
            if (perlin.type != NoiseType::Perlin1D)
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

const char* Application::NoiseTypeStr(NoiseType type)
{
    switch (type)
    {
    case NoiseType::Perlin1D:
        return "Perlin 1D";
        break;

    case NoiseType::Fractal:
        return "Fractal";
        break;

    case NoiseType::Turbulence:
        return "Turbulence";
        break;

    default:
        return "Unknwom";
        break;
    }
}

const char* Application::LerpTypeStr(LerpType type)
{
    switch (type)
    {
    case LerpType::Linear:
        return "Linear";
        break;

    case LerpType::Cosine:
        return "Cosine";
        break;

    default:
        return "Unknwom";
        break;
    }
}