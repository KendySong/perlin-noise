#pragma once
#include <cstdint>

class Settings
{
public :
    static Settings instance;
    std::uint32_t width;
    std::uint32_t height;

private :
    Settings();
};